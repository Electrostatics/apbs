/* Author: Jiahui Chen
 * Advisor: Weihua Geng
 * treecode subroutines */
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "gl_variables.h"
#include "gl_constants.h"
#include "treecode.h"


/* treecode_initializaion(){
 *   setup()
 *   create_tree(){
 *     if cond{
 *       partition_8()
 *         partition()
 *       create_tree()
 *     }
 * } */
int treecode_initialization(int main_order,int main_maxparnode,double main_theta){
/* set up variables used in treecode */
  /* local variables*/
  int level, ierr, err, i, j, k, mm, nn, idx, ijk[3];

  /* variables needed for cpu time */
  double totaltime, timetree;
  double *temp_a, *temp_b;
  double * temp_q;

  printf("Initialize treecode~\n");
  /*  */
  numpars=nface;
  order=main_order;
  /* creating tree */
  level=0;
  minlevel=50000;
  maxlevel=0;
  maxparnode=main_maxparnode;
  theta=main_theta;


  /* initialize kk */
  kk[0][0]=0; kk[0][1]=0; kk[0][2]=0; /* Original Kernel */

  kk[1][0]=1; kk[1][1]=0; kk[1][2]=0; /* 1st Order */
  kk[2][0]=0; kk[2][1]=1; kk[2][2]=0;
  kk[3][0]=0; kk[3][1]=0; kk[3][2]=1;

  kk[4][0]=1; kk[4][1]=0; kk[4][2]=0;
  kk[5][0]=0; kk[5][1]=1; kk[5][2]=0;
  kk[6][0]=0; kk[6][1]=0; kk[6][2]=1;

  kk[7][0]=2; kk[7][1]=0; kk[7][2]=0;
  kk[8][0]=1; kk[8][1]=1; kk[8][2]=0;
  kk[9][0]=1; kk[9][1]=0; kk[9][2]=1;
  kk[10][0]=1; kk[10][1]=1; kk[10][2]=0;
  kk[11][0]=0; kk[11][1]=2; kk[11][2]=0;
  kk[12][0]=0; kk[12][1]=1; kk[12][2]=1;
  kk[13][0]=1; kk[13][1]=0; kk[13][2]=1;
  kk[14][0]=0; kk[14][1]=1; kk[14][2]=1;
  kk[15][0]=0; kk[15][1]=0; kk[15][2]=2;

  /* allocate der_cof */
  der_cof = (double****) calloc(order+1,sizeof(double***));
  if (der_cof==NULL){
    fprintf(stderr, "setup error in treecode_initialization: der_cof");
    return 1;
  }
  for (i=0;i<order+1;i++){
    der_cof[i] = (double***) calloc(order+1,sizeof(double**));
    if (der_cof[i]==NULL){
      fprintf(stderr, "setup error in treecode_initialization: der_cof");
      return 1;
    }
    for (j=0;j<order+1;j++){
      der_cof[i][j] = (double**) calloc(order+1,sizeof(double*));
      if (der_cof[i][j]==NULL){
        fprintf(stderr, "setup error in treecode_initialization: der_cof");
        return 1;
      }
      for (k=0;k<order+1;k++){
        der_cof[i][j][k] = (double*) calloc(16,sizeof(double));
        if (der_cof[i][j][k]==NULL){
          fprintf(stderr, "setup error in treecode_initialization: der_cof");
          return 1;
        }
      }
    }
  }

  /* the adjustment of der_cof for the recurrance relation */

  for (i=0;i<order+1;i++){
    for (j=0;j<order+1;j++){
      for (k=0;k<order+1;k++){
        for (idx=0;idx<16;idx++){
          der_cof[i][j][k][idx]=1.0;
        }
      }
    }
  }

  for (i=0;i<order+1;i++){
    for (j=0;j<order+1;j++){
      for (k=0;k<order+1;k++){
        ijk[0]=i; ijk[1]=j; ijk[2]=k;
        for (idx=0;idx<16;idx++){
          for (mm=0;mm<3;mm++){
            if (kk[idx][mm]!=0){
              for (nn=0;nn<kk[idx][mm];nn++)
              /* nn in fortran */
              der_cof[i][j][k][idx]=der_cof[i][j][k][idx]*(ijk[mm]+(nn+1));
            }
          }
        }
      }
    }
  }


  for (i=0;i<order+1;i++){
    for (j=0;j<order+1;j++){
      for (k=0;k<order+1;k++){
        for (idx=0;idx<16;idx++){
          der_cof[i][j][k][idx]=der_cof[i][j][k][idx]*one_over_4pi;
        }
      }
    }
  }

  /* set x y z q orderind for treecode */
  x = (double*)calloc(numpars, sizeof(double));
  y = (double*)calloc(numpars, sizeof(double));
  z = (double*)calloc(numpars, sizeof(double));
  q = (double*)calloc(numpars, sizeof(double));
  orderind = (int*)calloc(numpars,sizeof(int));
  if (x==NULL){
    fprintf(stderr, "setup error in treecode_initialization: x empty data array");
    return 1;
  }
  if (y==NULL){
    fprintf(stderr, "setup error in treecode_initialization: y empty data array");
    return 1;
  }
  if (z==NULL){
    fprintf(stderr, "setup error in treecode_initialization: z empty data array");
    return 1;
  }
  if (q==NULL){
    fprintf(stderr, "setup error in treecode_initialization: q empty data array");
    return 1;
  }
  if (orderind==NULL){
    fprintf(stderr, "setup error in treecode_initialization: orderind empty data array");
    return 1;
  }
  for (i=0;i<numpars;i++){
    x[i]=tr_xyz[i*3];
    y[i]=tr_xyz[i*3+1];
    z[i]=tr_xyz[i*3+2];
    q[i]=1.0;
  }

  /* set temporary temp_a(numpars), temp_b(2*numpars), temp_q(3*numpars) */
  temp_a = (double*)calloc(numpars, sizeof(double));
  temp_b = (double*)calloc(2*numpars, sizeof(double));
  temp_q = (double*)calloc(3*numpars, sizeof(double));
  if (temp_a==NULL){
    fprintf(stderr, "setup error in treecode_initialization: temp_a empty data array");
    return 1;
  }
  if (temp_b==NULL){
    fprintf(stderr, "setup error in treecode_initialization: temp_b empty data array");
    return 1;
  }
  if (temp_q==NULL){
    fprintf(stderr, "setup error in treecode_initialization: temp_q empty data array");
    return 1;
  }

/* Call SETUP to allocate arrays for Taylor expansions */
/* and setup global variables. Also, copy variables into global copy arrays. */
  setup(x,y,z,q,numpars,order,iflag,xyzminmax);

  troot = (tnode*)malloc(1*sizeof(tnode));
  printf("Creating tree for %d particles with max %d per node\n",numpars,maxparnode);

  create_tree(troot,0,numpars-1,xyzminmax,level); 

  for (i=0;i<numpars;i++){
    temp_a[i] = tr_area[i];
    temp_q[3*i] = tr_q[3*i];
    temp_q[3*i+1] = tr_q[3*i+1];
    temp_q[3*i+2] = tr_q[3*i+2];
    temp_b[i] = bvct[i];
    temp_b[i+numpars] = bvct[i+numpars];
  }
  for (i=0;i<numpars;i++){
    tr_area[i]=temp_a[orderarr[i]];
    tr_q[3*i]=temp_q[3*orderarr[i]];
    tr_q[3*i+1]=temp_q[3*orderarr[i]+1];
    tr_q[3*i+2]=temp_q[3*orderarr[i]+2];
    bvct[i]=temp_b[orderarr[i]];

    bvct[i+numpars]=temp_b[orderarr[i]+numpars];
    tr_xyz[3*i]=x[i];
    tr_xyz[3*i+1]=y[i];
    tr_xyz[3*i+2]=z[i];
  }

  free(temp_a);
  free(temp_b);
  free(temp_q);

  tchg = (double***) calloc(nface,sizeof(double**));
  for (i=0;i<nface;i++){
    tchg[i] = (double**) calloc(2,sizeof(double*));
    for (j=0;j<2;j++){
      tchg[i][j] = (double*) calloc(16,sizeof(double));
    }
  }

  schg = (double***) calloc(nface,sizeof(double**));
  for (i=0;i<nface;i++){
    schg[i] = (double**) calloc(2,sizeof(double*));
    for (j=0;j<2;j++){
      schg[i][j] = (double*) calloc(16,sizeof(double));
    }
  }

  return 0;
}
/********************************************************/
double minval(double* variables, int number){
  int i;
  double MinVal;
  MinVal = variables[0];
  for(i=1;i<number;i++){
    if(MinVal>variables[i]){
      MinVal = variables[i];
    }else{
      MinVal = MinVal;
    }
  }
  return MinVal;
}

double maxval(double* variables, int number){
  int i;
  double MaxVal;
  MaxVal = variables[0];
  for(i=1;i<number;i++){
    if(MaxVal<variables[i])
      MaxVal = variables[i];
  }
  return MaxVal;
}
/********************************************************/
int setup(double* x,double* y,double* z,double* q,int numpars,
           int order,int iflag,double xyzminmax[6]){

/* SETUP allocates and initializes arrays needed for the Taylor expansion.
 Also, global variables are set and the Cartesian coordinates of
 the smallest box containing the particles is determined. The particle
 postions and charges are copied so that they can be restored upon exit.*/  
  int err,i,j,k;
  double t1;

  printf("Set up right now\n");

/* global integers and reals:  TORDER, TORDERLIM and THETASQ */
  /* keep the accuracy of first and second derivative of funtion G */
  torder = order+2;
  torder2 = order;
  if (iflag == 1)  orderoffset=0;
  else orderoffset = 1;
  torderlim = torder+orderoffset;

/* allocate global Taylor expansion variables */
  /* cf(0:torder) */
  cf = (double*)calloc(torder+1, sizeof(double));
  if (cf == NULL){
    fprintf(stderr, "setup error: cf empty data array\n");
    return 1;
  }
  cf1 = (double*)calloc(torderlim, sizeof(double));
  if (cf1 == NULL){
    fprintf(stderr, "setup error: cf1 empty data array\n");
    return 1;
  }
  cf2 = (double*)calloc(torderlim, sizeof(double));
  if (cf2 == NULL){
    fprintf(stderr, "setup error: cf2 empty data array\n");
    return 1;
  }
  cf3 = (double*)calloc(torderlim, sizeof(double));
  if (cf3 == NULL){
    fprintf(stderr, "setup error: cf3 empty data array\n");
    return 1;
  }

  /* a(-2:torderlim,-2:torderlim,-2:torderlim) */
  a = (double***)calloc(torderlim+3, sizeof(double**));
  b = (double***)calloc(torderlim+3, sizeof(double**));
  if (a == NULL){
    fprintf(stderr, "setup error: a empty data array\n");
    return 1;
  }
  if (b == NULL){
    fprintf(stderr, "setup error: b empty data array\n");
    return 1;
  }
  for (i=0;i<torderlim+3;i++){
    a[i] = (double**)calloc(torderlim+3, sizeof(double*));
    b[i] = (double**)calloc(torderlim+3, sizeof(double*));
    if (a == NULL){
      fprintf(stderr, "setup error: a empty data array\n");
      return 1;
    }
    if (b == NULL){
      fprintf(stderr, "setup error: b empty data array\n");
      return 1;
    }
    for (j=0;j<torderlim+3;j++){
      a[i][j]=(double*)calloc(torderlim+3, sizeof(double));
      b[i][j]=(double*)calloc(torderlim+3, sizeof(double));
      if (a == NULL){
        fprintf(stderr, "setup error: a empty data array\n");
        return 1;
      }
      if (b == NULL){
        fprintf(stderr, "setup error: b empty data array\n");
        return 1;
      }
    }
  }


  for (i=0;i<torderlim+3;i++){
    for (j=0;j<torderlim+3;j++){
      for (k=0;k<torderlim+3;k++){
        a[i][j][k]=0.0;
        b[i][j][k]=0.0;
      }
    }
  }

  for (i=0;i<torder+1;i++)
    cf[i] = i+1.0;

  for (i=0;i<torderlim;i++){
    t1=1.0/(i+1.0);
    cf1[i]=t1;
    cf2[i]=1.0-0.5*t1;
    cf3[i]=1.0-t1;
  }

/* find bounds of Cartesion box enclosing the particles */

  xyzminmax[0]=minval(x,numpars);
  xyzminmax[1]=maxval(x,numpars);
  xyzminmax[2]=minval(y,numpars);
  xyzminmax[3]=maxval(y,numpars);
  xyzminmax[4]=minval(z,numpars);
  xyzminmax[5]=maxval(z,numpars);

printf("%f,%f,%f,%f,%f,%f\n",xyzminmax[0],xyzminmax[1],xyzminmax[2],
       xyzminmax[3],xyzminmax[4],xyzminmax[5]);

  orderarr = (double*)calloc(numpars, sizeof(double));
  if (orderarr == NULL){
    fprintf(stderr, "setup error: orderarr empty data array\n");
    return 1;
  }
  for (i=0;i<numpars;i++)
    orderarr[i]=i;

  return 0;
}
/********************************************************/
int create_tree(tnode* p,int ibeg,int iend,double xyzmm[6],int level){
/*CREATE_TREE recursively create the tree structure. Node P is
  input, which contains particles indexed from IBEG to IEND. After
  the node parameters are set subdivision occurs if IEND-IBEG+1 > MAXPARNODE.
  Real array XYZMM contains the min and max values of the coordinates
  of the particle in P, thus defining the box. */

  /* local variables */
  double x_mid,y_mid,z_mid,xl,yl,zl,lmax,t1,t2,t3;
  int ind[8][2];
  double xyzmms[6][8];
  int i,j,limin,limax,err,loclev,numposchild;
  double lxyzmm[6];

/* set node fields: number of particles, exist_ms and xyz bounds */
  p->numpar=iend-ibeg+1;
  p->exist_ms=0;

  p->x_min=xyzmm[0];
  p->x_max=xyzmm[1];
  p->y_min=xyzmm[2];
  p->y_max=xyzmm[3];
  p->z_min=xyzmm[4];
  p->z_max=xyzmm[5];
/* compute aspect ratio */
  xl=p->x_max-p->x_min;
  yl=p->y_max-p->y_min;
  zl=p->z_max-p->z_min;

  lmax = xl;
  if (lmax<yl) lmax = yl;
  if (lmax<zl) lmax = zl;

  t1 = lmax;
  t2 = xl;
  if (t2>yl) t2 = yl;
  if (t2>zl) t2 = zl;

  if (t2!=0.0) p->aspect=t1/t2;
  else p->aspect=0.0;
/* midpoint coordinates, RADIUS and SQRADIUS */
  p->x_mid = (p->x_max+p->x_min)/2.0;
  p->y_mid = (p->y_max+p->y_min)/2.0;
  p->z_mid = (p->z_max+p->z_min)/2.0;
  t1=p->x_max-p->x_mid;
  t2=p->y_max-p->y_mid;
  t3=p->z_max-p->z_mid;
  p->radius=sqrt(t1*t1+t2*t2+t3*t3);

/* set particle limits, tree level of node, and nullify children pointers */
  p->ibeg=ibeg;
  p->iend=iend;
  p->level=level;
  if (maxlevel < level) maxlevel=level;
  p->num_children = 0;
/* old version */  /* struct tnode* child[8] */
  p->child=(tnode**)malloc(8*sizeof(tnode*));
  for (i=0;i<8;i++) p->child[i]=(tnode*)malloc(1*sizeof(tnode));

  if (p->numpar > maxparnode){
/* set IND array to 0 and then call PARTITION routine. IND array holds indices
 * of the eight new subregions. Also, setup XYZMMS array in case SHRINK=1 */

    xyzmms[0][0]=p->x_min;
    xyzmms[1][0]=p->x_max;
    xyzmms[2][0]=p->y_min;
    xyzmms[3][0]=p->y_max;
    xyzmms[4][0]=p->z_min;
    xyzmms[5][0]=p->z_max;
    for (i=0;i<8;i++){
      ind[i][0]=0;
      ind[i][1]=0;
    }
    ind[0][0]=ibeg;
    ind[0][1]=iend;
    x_mid=p->x_mid;
    y_mid=p->y_mid;
    z_mid=p->z_mid;

    numposchild = partition_8(xyzmms,xl,yl,zl,lmax,x_mid,y_mid,z_mid,ind);
/* Shrink the box */
  for (i=0;i<8;i++){
    if (ind[i][0] < ind[i][1]){
      xyzmms[0][i]=minval(&x[ind[i][0]],ind[i][1]-ind[i][0]);
      xyzmms[1][i]=maxval(&x[ind[i][0]],ind[i][1]-ind[i][0]);
      xyzmms[2][i]=minval(&y[ind[i][0]],ind[i][1]-ind[i][0]);
      xyzmms[3][i]=maxval(&y[ind[i][0]],ind[i][1]-ind[i][0]);
      xyzmms[4][i]=minval(&z[ind[i][0]],ind[i][1]-ind[i][0]);
      xyzmms[5][i]=maxval(&z[ind[i][0]],ind[i][1]-ind[i][0]);
    }
  }
/* create children if indicated and store info in parent */
    loclev = level+1;

    for (i=0;i<numposchild;i++){
      if (ind[i][0] <= ind[i][1]){
        p->num_children = p->num_children+1;
        for (j=0;j<6;j++) {
          lxyzmm[j]=xyzmms[j][i];
        }
        create_tree(p->child[p->num_children-1],ind[i][0],ind[i][1],lxyzmm,loclev);
      }
    }
  }
  else {
    if(level < minlevel) minlevel = level;
  }

  return 0;
}
/********************************************************/
int partition_8(double xyzmms[6][8],double xl,double yl,double zl,double lmax,
                double x_mid,double y_mid, double z_mid,int ind[8][2]){
/* PARTITION_8 determines the particle indices of the eight sub boxes
 * containing the particles after the box defined by particles I_BEG
 * to I_END is divided by its midpoints in each coordinate direction.
 * The determination of the indices is accomplished by the subroutine
 * PARTITION. A box is divided in a coordinate direction as long as the
 * resulting aspect ratio is not too large. This avoids the creation of
 * "narrow" boxes in which Talyor expansions may become inefficient.
 * On exit the INTEGER array IND (dimension 8 x 2) contains
 * the indice limits of each new box (node) and NUMPOSCHILD the number 
 * of possible children.  If IND(J,1) > IND(J,2) for a given J this indicates
 * that box J is empty.*/
  int temp_ind,i,j;
  double critlen;
  int numposchild;

  numposchild = 1;
  critlen = lmax/sqrt(2.0);

  if (xl >= critlen) {
    temp_ind = partition(x,y,z,q,orderarr,ind[0][0],ind[0][1],x_mid,
                         numpars);
    ind[1][0]=temp_ind+1;
    ind[1][1]=ind[0][1];
    ind[0][1]=temp_ind;
    for (i=0;i<6;i++) xyzmms[i][1]=xyzmms[i][0];
    xyzmms[1][0]=x_mid;
    xyzmms[0][1]=x_mid;
    numposchild=2*numposchild;
  }

  if (yl >= critlen) {
    for (i=0;i<numposchild;i++){
      temp_ind = partition(y,x,z,q,orderarr,ind[i][0],ind[i][1],y_mid,
                           numpars);
      ind[numposchild+i][0]=temp_ind+1;
      ind[numposchild+i][1]=ind[i][1];
      ind[i][1]=temp_ind;
      for (j=0;j<6;j++) xyzmms[j][numposchild+i]=xyzmms[j][i];
      xyzmms[3][i]=y_mid;
      xyzmms[2][numposchild+i]=y_mid;
    }
    numposchild = 2*numposchild;
  }

  if (zl >= critlen) {
    for (i=0;i<numposchild;i++){
      temp_ind = partition(z,x,y,q,orderarr,ind[i][0],ind[i][1],z_mid,
                           numpars);
      ind[numposchild+i][0]=temp_ind+1;
      ind[numposchild+i][1]=ind[i][1];
      ind[i][1]=temp_ind;
      for (j=0;j<6;j++) xyzmms[j][numposchild+i]=xyzmms[j][i];
      xyzmms[5][i]=z_mid;
      xyzmms[4][numposchild+i]=z_mid;
    }
    numposchild=2*numposchild;
  }

  return (numposchild);
}
/********************************************************/
int partition(double *a,double *b,double *c,double *q,int *indarr,int ibeg,
              int iend,double val,int numpars){
/* PARTITION determines the index MIDIND, after partitioning
 * in place the  arrays A,B,C and Q,  such that 
 * A(IBEG:MIDIND) <= VAL and  A(MIDIND+1:IEND) > VAL. 
 * If on entry IBEG > IEND or  A(IBEG:IEND) > VAL then MIDIND
 * is returned as IBEG-1.  */
  double ta,tb,tc,tq;
  int lower,upper,tind;
  int midind;

  if (ibeg<iend){
/* temporarily store IBEG entries and set A(IBEG)=VAL for 
 * the partitoning algorithm.  */
    ta=a[ibeg];
    tb=b[ibeg];
    tc=c[ibeg];
    tq=q[ibeg];
    tind=indarr[ibeg];
    a[ibeg]=val;/*val=mid val on that direction*/
    upper=ibeg;
    lower=iend;

    while(upper!=lower){
      while(upper < lower && val < a[lower])
        lower=lower-1;
      if(upper != lower){
        a[upper] = a[lower];
        b[upper] = b[lower];
        c[upper] = c[lower];
        q[upper] = q[lower];
        indarr[upper]=indarr[lower];
      }
      while(upper < lower && val >= a[upper])
        upper=upper+1;
      if (upper != lower){
        a[lower]=a[upper];
        b[lower]=b[upper];
        c[lower]=c[upper];
        q[lower]=q[upper];
        indarr[lower]=indarr[upper];
      }
    }
    midind = upper;
/* replace TA in position UPPER and change MIDIND if TA > VAL */
    if (ta > val)
      midind = upper - 1;
    a[upper]=ta;
    b[upper]=tb;
    c[upper]=tc;
    q[upper]=tq;
    indarr[upper]=tind;
  }
  else if (ibeg == iend){
    if (a[ibeg] <= val) midind = ibeg;
    else midind = ibeg - 1;
  }
  else midind = ibeg - 1;
  return (midind);
}
/********************************************************/
int *matvec(double *alpha, double *tpoten_old, double *beta, double *tpoten){
/* the main part of treecode */
/* in gmres *matvec(Alpha, X, Beta, Y) where y := alpha*A*x + beta*y */
  /* local variables */
  int i, j, k;
  double area, rs, irs, sumrs;
  double tempx, temp_area, sl[4];
  double time1, time2, tempq[2][16];
  double pre1, pre2;
  double peng[2],peng_old[2];

  pb_kernel(tpoten_old);

  /* Generate the moments if not allocated yet */
  comp_ms_all(troot,1);

  pre1 = 0.50*(1.0+eps);
  pre2 = 0.50*(1.0+1.0/eps);

  for (i=0;i<numpars;i++){

    peng[0]=0.0; peng[1]=0.0;
    peng_old[0]=tpoten_old[i];
    peng_old[1]=tpoten_old[i+numpars];
    tarpos[0]=x[i];
    tarpos[1]=y[i];
    tarpos[2]=z[i];
    tarq[0]=tr_q[3*i];
    tarq[1]=tr_q[3*i+1];
    tarq[2]=tr_q[3*i+2];
    for (j=0;j<2;j++){
      for (k=0;k<16;k++){
        tempq[j][k]=tchg[i][j][k];
      }
    }

    /* remove the singularity */
    tempx=x[i];
    temp_area=tr_area[i];
    x[i]+=100.123456789;
    tr_area[i]=0.0;

    /* start to use Treecode */
    compp_tree(troot,peng,tpoten_old,tempq);

    tpoten[i]=tpoten[i]* *beta+(pre1*peng_old[0]-peng[0])* *alpha;
    tpoten[numpars+i]=tpoten[numpars+i]* *beta+(pre2*peng_old[1]-peng[1])* *alpha;

    x[i]=tempx;
    tr_area[i]=temp_area;
  }


  remove_mmt(troot);

  return 0;
}
/********************************************************/
int pb_kernel(double *phi){
  int i,j,ikp,iknl,ixyz,jxyz,indx;

  for (i=0;i<numpars;i++){

    indx=0;
    for (ikp=0;ikp<2;ikp++){
      tchg[i][ikp][indx]=1.0;
      schg[i][ikp][indx]=tr_area[i]*phi[numpars+i];
    }

    for (iknl=0;iknl<2;iknl++){
      for (ixyz=0;ixyz<3;ixyz++){
        indx+=1;
        for (ikp=0;ikp<2;ikp++){
          tchg[i][ikp][indx]=1.0*(1-iknl)+tr_q[3*i+ixyz]*iknl;
          schg[i][ikp][indx]=(tr_q[3*i+ixyz]*(1-iknl)+1.0*iknl)*
                             tr_area[i]*phi[iknl*numpars+i];
        }
      }
    }

    for (ixyz=0;ixyz<3;ixyz++){
      for (jxyz=0;jxyz<3;jxyz++){
        indx+=1;
        for (ikp=0;ikp<2;ikp++){
          tchg[i][ikp][indx]=tr_q[3*i+jxyz];
          schg[i][ikp][indx]=-tr_q[3*i+ixyz]*tr_area[i]*phi[i];
        }
      }
    }
  }

  return 0;
}
/********************************************************/
int comp_ms_all(tnode *p, int ifirst){
/* REMOVE_NODE recursively removes each node from the tree and deallocates
 * its memory for MS array if it exits. */


  int i,j,k,err;
  int k1,k2,k3;
  double dx,dy,dz,tx,ty,tz;

  if (p->exist_ms == 0 && ifirst == 0){
    p->ms = (double****)calloc(16, sizeof(double***));
    for (i=0;i<16;i++){
      p->ms[i] = (double***)calloc(torder+1, sizeof(double**));
      for (j=0;j<torder+1;j++){
        p->ms[i][j]=(double**)calloc(torder+1, sizeof(double*));
        for (k=0;k<torder+1;k++)
          p->ms[i][j][k]=(double*)calloc(torder+1, sizeof(double));
      }
    }
    /* if null */
    comp_ms(p);
    p->exist_ms=1;
  }

  if(p->num_children > 0){
    for (i=0;i<p->num_children;i++){
      comp_ms_all(p->child[i],0);
    }
  }

  return 0;
}
/********************************************************/
int comp_ms(tnode *p){
/* COMP_MS computes the moments for node P needed in the Taylor 
 * approximation */

  int i,j,k1,k2,k3,n,m,k;
  double dx,dy,dz,tx,ty,tz,txyz;

  for (n=0;n<16;n++){
    for (i=0;i<torder+1;i++){
      for (j=0;j<torder+1;j++){
        for (k=0;k<torder+1;k++)
          p->ms[n][i][j][k] = 0.0;
      }
    }
  }

  for (i=p->ibeg;i<p->iend+1;i++){
    dx=x[i]-p->x_mid;
    dy=y[i]-p->y_mid;
    dz=z[i]-p->z_mid;
    for (j=0;j<7;j++){

      tx=1.0;
      for (k1=0;k1<torder+1;k1++){
        ty=1.0;
        for (k2=0;k2<torder+1-k1;k2++){
          tz=1.0;
          for (k3=0;k3<torder+1-k1-k2;k3++){
  /*****************************************/
            txyz=tx*ty*tz;
            p->ms[j][k1][k2][k3]+=schg[i][0][j]*txyz;

  /*****************************************/
            tz=tz*dz;
          }
          ty=ty*dy;
        }
        tx=tx*dx;
      }
    }
    tx=1.0;
    for (k1=0;k1<torder+1;k1++){
      ty=1.0;
      for (k2=0;k2<torder+1-k1;k2++){
        tz=1.0;
        for (k3=0;k3<torder+1-k1-k2;k3++){
  /*****************************************/
          txyz=tx*ty*tz;
          p->ms[7][k1][k2][k3]+=schg[i][0][7]*txyz;
          p->ms[10][k1][k2][k3]+=schg[i][0][10]*txyz;
          p->ms[13][k1][k2][k3]+=schg[i][0][13]*txyz;
  /*****************************************/
          tz=tz*dz;
        }
        ty=ty*dy;
      }
      tx=tx*dx;
    }
  }
  /*****************************************/
  for (k1=0;k1<torder+1;k1++){
    for (k2=0;k2<torder+1-k1;k2++){
      for (k3=0;k3<torder+1-k1-k2;k3++){
        p->ms[8][k1][k2][k3]=p->ms[7][k1][k2][k3];
        p->ms[9][k1][k2][k3]=p->ms[7][k1][k2][k3];
        p->ms[11][k1][k2][k3]=p->ms[10][k1][k2][k3];
        p->ms[12][k1][k2][k3]=p->ms[10][k1][k2][k3];
        p->ms[14][k1][k2][k3]=p->ms[13][k1][k2][k3];
        p->ms[15][k1][k2][k3]=p->ms[13][k1][k2][k3];
      }
    }
  }
  return 0;
}
/********************************************************/
int compp_tree(tnode* p,double peng[2],double *tpoten_old,double tempq[2][16]){
  /* compp_tree() is self recurrence function */
  double tx,ty,tz,dist,penglocal[2],pengchild[2];
  int i;

  /* determine DISTSQ for MAC test */
  tx = p->x_mid - tarpos[0];
  ty = p->y_mid - tarpos[1];
  tz = p->z_mid - tarpos[2];
  dist = sqrt(tx*tx+ty*ty+tz*tz);

  /* initialize potential energy */
  peng[0]=0.0; peng[1]=0.0;

/* If MAC is accepted and there is more than 1 particale in the */
/* box use the expansion for the approximation. */

  if (p->radius < dist*theta && p->numpar > 40){
    compp_tree_pb(p,peng,tempq);
  }
  else {
    if (p->num_children == 0){
      penglocal[0]=0.0;penglocal[0]=0.0;
      compp_direct_pb(penglocal,p->ibeg,p->iend,tpoten_old);
      peng[0]=penglocal[0];peng[1]=penglocal[1];
    }
    else {
      /* If MAC fails check to see if there are children. If not, perform */
      /* direct calculation.  If there are children, call routine */
      /* recursively for each. */
      for (i=0;i<p->num_children;i++){
        pengchild[0]=0.0; pengchild[1]=0.0;
        compp_tree(p->child[i],pengchild,tpoten_old,tempq);
        peng[0] += pengchild[0];
        peng[1] += pengchild[1];
      }
    }
  }

  return 0;
}
/********************************************************/
int compp_tree_pb(tnode* p,double peng[2],double tempq[2][16]){
  int i,j,k,ikp,indx;
  double kapa[2],sl[4],pt_comp[2][16];

  kapa[0]=0.0; kapa[1]=kappa;
  for (ikp=0;ikp<2;ikp++){
    comp_tcoeff(p, kapa[ikp]);

    for (indx=0;indx<16;indx++){
      peng[ikp]=0.0;
      for (i=0;i<torder2+1;i++){
        for (j=0;j<torder2+1-i;j++){
          for (k=0;k<torder2+1-i-j;k++){
            peng[ikp]+=der_cof[i][j][k][indx]
                       *a[i+2+kk[indx][0]][j+2+kk[indx][1]][k+2+kk[indx][2]]
                       *p->ms[indx][i][j][k];
          }
        }
      }
      pt_comp[ikp][indx]=tempq[ikp][indx]*peng[ikp];
    }
  }

  sl[0] = pt_comp[0][0]-pt_comp[1][0];
  sl[1] = eps*(pt_comp[1][1]+pt_comp[1][2]+pt_comp[1][3])
          -(pt_comp[0][1]+pt_comp[0][2]+pt_comp[0][3]);
  sl[2] = -(pt_comp[0][4]+pt_comp[0][5]+pt_comp[0][6])
          +(pt_comp[1][4]+pt_comp[1][5]+pt_comp[1][6])/eps;
  sl[3] = 0.0;
  for (i=7;i<16;i++)
    sl[3] += pt_comp[1][i]-pt_comp[0][i];
  peng[0]=sl[0]+sl[1];
  peng[1]=sl[2]+sl[3];

  return 0;
}
/********************************************************/
int comp_tcoeff(tnode *p, double kappa){
/* COMP_TCOEFF computes the Taylor coefficients of the potential
 * using a recurrence formula.  The center of the expansion is the
 * midpoint of the node P.  TARPOS and TORDERLIM are globally defined. */
  double dx,dy,dz,ddx,ddy,ddz,dist,fac,cf1_new[torderlim];
  double kappax,kappay,kappaz;
  int i,j,k;

  /* setup variables */
  for (i=0;i<torderlim;i++) cf1_new[i]=cf1[i]*kappa;

  dx=tarpos[0]-p->x_mid;
  dy=tarpos[1]-p->y_mid;
  dz=tarpos[2]-p->z_mid;

  ddx=2.0*dx;
  ddy=2.0*dy;
  ddz=2.0*dz;

  kappax=kappa*dx;
  kappay=kappa*dy;
  kappaz=kappa*dz;

  dist=dx*dx+dy*dy+dz*dz;
  fac=1.0/dist;
  dist=sqrt(dist);

  /* 0th coeff or function val */
  b[2][2][2]=exp(-kappa*dist);
  a[2][2][2]=b[2][2][2]/dist;

  /* 2 indices are 0 */

  b[3][2][2]=kappax*a[2][2][2];
  b[2][3][2]=kappay*a[2][2][2];
  b[2][2][3]=kappaz*a[2][2][2];

  a[3][2][2]=fac*dx*(a[2][2][2]+kappa*b[2][2][2]);
  a[2][3][2]=fac*dy*(a[2][2][2]+kappa*b[2][2][2]);
  a[2][2][3]=fac*dz*(a[2][2][2]+kappa*b[2][2][2]);

  for (i=2;i<torderlim+1;i++){
    b[i+2][2][2]=cf1[i-1]*kappa*(dx*a[i+1][2][2]-a[i][2][2]);
    b[2][i+2][2]=cf1[i-1]*kappa*(dy*a[2][i+1][2]-a[2][i][2]);
    b[2][2][i+2]=cf1[i-1]*kappa*(dz*a[2][2][i+1]-a[2][2][i]);

    a[i+2][2][2]=fac*(ddx*cf2[i-1]*a[i+1][2][2]-cf3[i-1]*a[i][2][2]+
                 cf1[i-1]*kappa*(dx*b[i+1][2][2]-b[i][2][2]));
    a[2][i+2][2]=fac*(ddy*cf2[i-1]*a[2][i+1][2]-cf3[i-1]*a[2][i][2]+
                 cf1[i-1]*kappa*(dy*b[2][i+1][2]-b[2][i][2]));
    a[2][2][i+2]=fac*(ddz*cf2[i-1]*a[2][2][i+1]-cf3[i-1]*a[2][2][i]+
                 cf1[i-1]*kappa*(dz*b[2][2][i+1]-b[2][2][i]));
  }

  /* 1 index 0, 1 index 1, other >=1 */
  b[3][3][2]=kappax*a[2][3][2];
  b[3][2][3]=kappax*a[2][2][3];
  b[2][3][3]=kappay*a[2][2][3];

  a[3][3][2]=fac*(dx*a[2][3][2]+ddy*a[3][2][2]+kappax*b[2][3][2]);
  a[3][2][3]=fac*(dx*a[2][2][3]+ddz*a[3][2][2]+kappax*b[2][2][3]);
  a[2][3][3]=fac*(dy*a[2][2][3]+ddz*a[2][3][2]+kappay*b[2][2][3]);

  for (i=2;i<torderlim;i++){
    b[3][2][i+2]=kappax*a[2][2][i+2];
    b[2][3][i+2]=kappay*a[2][2][i+2];
    b[2][i+2][3]=kappaz*a[2][i+2][2];
    b[3][i+2][2]=kappax*a[2][i+2][2];
    b[i+2][3][2]=kappay*a[i+2][2][2];
    b[i+2][2][3]=kappaz*a[i+2][2][2];

    a[3][2][i+2]=fac*(dx*a[2][2][i+2]+ddz*a[3][2][i+1]-a[3][2][i]+
                 kappax*b[2][2][i+2]);
    a[2][3][i+2]=fac*(dy*a[2][2][i+2]+ddz*a[2][3][i+1]-a[2][3][i]+
                 kappay*b[2][2][i+2]);
    a[2][i+2][3]=fac*(dz*a[2][i+2][2]+ddy*a[2][i+1][3]-a[2][i][3]+
                 kappaz*b[2][i+2][2]);
    a[3][i+2][2]=fac*(dx*a[2][i+2][2]+ddy*a[3][i+1][2]-a[3][i][2]+
                 kappax*b[2][i+2][2]);
    a[i+2][3][2]=fac*(dy*a[i+2][2][2]+ddx*a[i+1][3][2]-a[i][3][2]+
                 kappay*b[i+2][2][2]);
    a[i+2][2][3]=fac*(dz*a[i+2][2][2]+ddx*a[i+1][2][3]-a[i][2][3]+
                 kappaz*b[i+2][2][2]);
  }

  /* 1 index 0, others >=2 */
  for (i=2;i<torderlim-1;i++){
   for (j=2;j<torderlim-i+1;j++){
      b[i+2][j+2][2]=cf1[i-1]*kappa*(dx*a[i+1][j+2][2]-a[i][j+2][2]);
      b[i+2][2][j+2]=cf1[i-1]*kappa*(dx*a[i+1][2][j+2]-a[i][2][j+2]);
      b[2][i+2][j+2]=cf1[i-1]*kappa*(dy*a[2][i+1][j+2]-a[2][i][j+2]);

      a[i+2][j+2][2]=fac*(ddx*cf2[i-1]*a[i+1][j+2][2]+ddy*a[i+2][j+1][2]
                     -cf3[i-1]*a[i][j+2][2]-a[i+2][j][2]+
                     cf1[i-1]*kappa*(dx*b[i+1][j+2][2]-b[i][j+2][2]));
      a[i+2][2][j+2]=fac*(ddx*cf2[i-1]*a[i+1][2][j+2]+ddz*a[i+2][2][j+1]
                     -cf3[i-1]*a[i][2][j+2]-a[i+2][2][j]+
                     cf1[i-1]*kappa*(dx*b[i+1][2][j+2]-b[i][2][j+2]));
      a[2][i+2][j+2]=fac*(ddy*cf2[i-1]*a[2][i+1][j+2]+ddz*a[2][i+2][j+1]
                     -cf3[i-1]*a[2][i][j+2]-a[2][i+2][j]+
                     cf1[i-1]*kappa*(dy*b[2][i+1][j+2]-b[2][i][j+2]));
    }
  }

  /* 2 indices 1,other >= 1 */
  b[3][3][3]=kappax*a[2][3][3];
  a[3][3][3]=fac*(dx*a[2][3][3]+ddy*a[3][2][3]+ddz*a[3][3][2]+
             kappax*b[2][3][3]);

  for (i=2;i<torderlim-1;i++){
    b[3][3][i+2]=kappax*a[2][3][i+2];
    b[3][i+2][3]=kappax*a[2][i+2][3];
    b[i+2][3][3]=kappay*a[i+2][2][3];

    a[3][3][i+2]=fac*(dx*a[2][3][i+2]+ddy*a[3][2][i+2]+ddz*a[3][3][i+1]
                 -a[3][3][i]+kappax*b[2][3][i+2]);
    a[3][i+2][3]=fac*(dx*a[2][i+2][3]+ddy*a[3][i+1][3]+ddz*a[3][i+2][2]
                 -a[3][i][3]+kappax*b[2][i+2][3]);
    a[i+2][3][3]=fac*(dy*a[i+2][2][3]+ddx*a[i+1][3][3]+ddz*a[i+2][3][2]
                 -a[i][3][3]+kappay*b[i+2][2][3]);
  }

  /* 1 index 1, others >=2 */
  for (i=2;i<torderlim-2;i++){
    for (j=2;j<torderlim-i+1;j++){
      b[3][i+2][j+2]=kappax*a[2][i+2][j+2];
      b[i+2][3][j+2]=kappay*a[i+2][2][j+2];
      b[i+2][j+2][3]=kappaz*a[i+2][j+2][2];

      a[3][i+2][j+2]=fac*(dx*a[2][i+2][j+2]+ddy*a[3][i+1][j+2]
                     +ddz*a[3][i+2][j+1]-a[3][i][j+2]
                     -a[3][i+2][j]+kappax*b[2][i+2][j+2]);
      a[i+2][3][j+2]=fac*(dy*a[i+2][2][j+2]+ddx*a[i+1][3][j+2]
                     +ddz*a[i+2][3][j+1]-a[i][3][j+2]
                     -a[i+2][3][j]+kappay*b[i+2][2][j+2]);
      a[i+2][j+2][3]=fac*(dz*a[i+2][j+2][2]+ddx*a[i+1][j+2][3]
                     +ddy*a[i+2][j+1][3]-a[i][j+2][3]
                     -a[i+2][j][3]+kappaz*b[i+2][j+2][2]);
    }
  }

  /* all indices >=2 */
  for (k=2;k<torderlim-3;k++){
    for (j=2;j<torderlim-1-k;j++){
      for (i=2;i<torderlim+1-k-j;i++){
        b[i+2][j+2][k+2]=cf1[i-1]*kappa*(dx*a[i+1][j+2][k+2]-a[i][j+2][k+2]);
        a[i+2][j+2][k+2]=fac*(ddx*cf2[i-1]*a[i+1][j+2][k+2]
                         +ddy*a[i+2][j+1][k+2]+ddz*a[i+2][j+2][k+1]
                         -cf3[i-1]*a[i][j+2][k+2]-a[i+2][j][k+2]-a[i+2][j+2][k]
                         +cf1[i-1]*kappa*(dx*b[i+1][j+2][k+2]-b[i][j+2][k+2]));
      }
    }
  }

  return 0;
}
/********************************************************/
int compp_direct_pb(double peng[2],int ibeg,int iend,double *tpoten_old){
  /* COMPF_DIRECT directly computes the force on the current target 
 * particle determined by the global variable TARPOS.*/
  int i,j;
  double dist,dist2,tx,ty,tz,soupos[3],souq[3];
  double peng_old[2],L1,L2,L3,L4,area,temp_area;
  double tp[3],tq[3],sp[3],sq[3],r_s[3];
  double rs,irs,sumrs;
  double G0,kappa_rs,exp_kappa_rs,Gk;
  double cos_theta,cos_theta0,tp1,tp2,dot_tqsq;
  double G10,G20,G1,G2,G3,G4;

  peng[0]=0.0;peng[1]=0.0;

  for (j=ibeg;j<iend+1;j++){
    tp[0]=tarpos[0];tp[1]=tarpos[1];tp[2]=tarpos[2];
    tq[0]=tarq[0];tq[1]=tarq[1];tq[2]=tarq[2];

    sp[0]=x[j];sp[1]=y[j];sp[2]=z[j];
    sq[0]=tr_q[3*j];sq[1]=tr_q[3*j+1];sq[2]=tr_q[3*j+2];

    r_s[0]=sp[0]-tp[0];r_s[1]=sp[1]-tp[1];r_s[2]=sp[2]-tp[2];
    sumrs=r_s[0]*r_s[0]+r_s[1]*r_s[1]+r_s[2]*r_s[2];
    rs=sqrt(sumrs);
    irs=1/rs;
    G0=one_over_4pi*irs;
    kappa_rs=kappa*rs;
    exp_kappa_rs=exp(-kappa_rs);
    Gk=exp_kappa_rs*G0;

    cos_theta =(sq[0]*r_s[0]+sq[1]*r_s[1]+sq[2]*r_s[2])*irs;
    cos_theta0=(tq[0]*r_s[0]+tq[1]*r_s[1]+tq[2]*r_s[2])*irs;
    tp1=G0*irs;
    tp2=(1.0+kappa_rs)*exp_kappa_rs;

    G10=cos_theta0*tp1;
    G20=tp2*G10;

    G1=cos_theta*tp1;
    G2=tp2*G1; 

    dot_tqsq=sq[0]*tq[0]+sq[1]*tq[1]+sq[2]*tq[2];
    G3=(dot_tqsq-3.0*cos_theta0*cos_theta)*irs*tp1;
    G4=tp2*G3-kappa2*cos_theta0*cos_theta*Gk;

    L1=G1-eps*G2;
    L2=G0-Gk;
    L3=G4-G3;
    L4=G10-G20/eps; 

    peng_old[0]=tpoten_old[j];peng_old[1]=tpoten_old[j+numpars];
    area=tr_area[j];

    peng[0]=peng[0]+(L1*peng_old[0]+L2*peng_old[1])*area;
    peng[1]=peng[1]+(L3*peng_old[0]+L4*peng_old[1])*area;
  }

  return 0;
}
/********************************************************/
int remove_mmt(tnode *p){
/* REMOVE_NODE recursively removes each node from the
 * tree and deallocates its memory for MS array if it exits. */
  int i,j,k,n;

  if (p->exist_ms == 1){
    for (n=0;n<16;n++){
      for (i=0;i<torder+1;i++){
        for (j=0;j<torder+1;j++){
          free(p->ms[n][i][j]);
        }
        free(p->ms[n][i]);
      }
      free(p->ms[n]);
    }
    free(p->ms);
  
    p->exist_ms=0;
  }

  if (p->num_children > 0){
    for (i=0;i<p->num_children;i++)
      remove_mmt(p->child[i]);
  }

  return 0;
}
/********************************************************/
int treecode_finalization(){

  int i, j, k, m;

/***********treecode_initialization*******/
  for(i=0;i<order+1;i++){
    for(j=0;j<order+1;j++){
      for(k=0;k<order+1;k++){
        free(der_cof[i][j][k]);
      }
      free(der_cof[i][j]);
    }
    free(der_cof[i]);
  }
  free(der_cof);

  free(x);
  free(y);
  free(z);
  free(q);
  free(orderind);

  for (i=0;i<nface;i++){
    for (j=0;j<2;j++) {
      free(tchg[i][j]);
    }
    free(tchg[i]);
  }
  free(tchg);

  for (i=0;i<nface;i++){
    for (j=0;j<2;j++) {
      free(schg[i][j]);
    }
    free(schg[i]);
  }
  free(schg);
/***********clean tree structure**********/

  remove_node(troot);
  printf("Clean up the tree structure\n");

/***********variables in setup************/
  free(cf);
  free(cf1);
  free(cf2);
  free(cf3);

  for (i=0;i<torderlim+3;i++){
    for (j=0;j<torderlim+3;j++){
      free(a[i][j]);
      free(b[i][j]);
    }
    free(a[i]);
    free(b[i]);
  }
  free(a);
  free(b);

  free(orderarr);
/*****************************************/

  printf("Clean up the memory!\n");

  return 0;
}
/**************************************************************/
int remove_node(tnode* p){
/* REMOVE_NODE recursively removes each node from the
 * tree and deallocates its memory for MS array if it exits. */
  int i;

  if (p->num_children > 0){
    for (i=0;i<8;i++){
      remove_node(p->child[i]);
      free(p->child[i]);
    }
    free(p->child);
  }

  return 0;
}
