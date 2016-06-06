#include <time.h>
#include <stdio.h>
#include <math.h>
#include "gl_variables.h"
#include "gl_constants.h"
#include <stdlib.h>
#include <string.h>
//#include "treecode.h"  /* try to use less variables */

int apbs2tabipb_(char** apbs_pqr_filename, int* nion, double* ionc,
                double* ionq, double* ionr, double* pdie,
                double* sdie, double* sdens, double* temp,
                int* tree_order, int* tree_n0, double* mac){

  /* variables local to main */
  int i,j,k;
  double s[3],pot=0.0,sum=0.0,pot_temp=0.0;
  double ptl,soleng,t1,t2;
  char fname[5],density[16],fnamepqr[16];
  extern void readin(char *fname, char *density);
  extern double potential_molecule(double s[3]);
  extern int comp_source();
  /* variables used to compute potential solution */
  double units_para;
  double *chrptl;
  extern int comp_pot(double *chrptl);
  /* variables used in treecode */
  int order,maxparnode;
  double theta;
  extern int treecode_initialization(int order,int maxparnode,double theta);
  extern int treecode_finalization();
  /* GMRES related variables */
  static long int info;
  long int RESTRT,ldw,ldh,iter,N;
  double resid;

  extern int *matvec(),*psolve();
  extern int gmres_(long int *n,double *b,double *x,long int *restrt, 
                    double *work,long int *ldw,double *h,long int *ldh,
                    long int *iter,double *resid,int (*matvec) (), 
                    int (*psolve) (),long int *info);

  timer_start("TOTAL_TIME");

  printf("Address of nion is %x\n",nion);
  printf("Value of *nion is %d\n",*nion);
  printf("Portein is %p\n",apbs_pqr_filename);
  printf("Portein is %s\n",&apbs_pqr_filename[0]);
  printf("Density is %f\n",*sdens);
  printf("Value of ionc is %f\n",ionc[0]);
  printf("Value of ionq is %f\n",ionq[0]);
  printf("Treecode order is %d\n",*tree_order);
  printf("Maxparnode is %d\n",*tree_n0);
  printf("Theta is %f\n",*mac);

  /***************input****************/
//  fnamepqr=*apbs_pqr_filename;

  strncpy(fname,&*apbs_pqr_filename,4);
  fname[4]='\0';
  printf("fname is %s\n",&fname[0]);

  sprintf(density,"%f",*sdens);

  printf("density is %s\n",density);
  epsp=*pdie;
  epsw=*sdie;
  printf("epsp %f epsw %f\n",epsp,epsw);
  bulk_strength=0.0;
  for (i=0;i<*nion;i++)
    bulk_strength+=ionc[i]*ionq[i]*ionq[i];
  printf("bulk_strength is %f\n",bulk_strength);
  order=*tree_order;
  maxparnode=*tree_n0;
  theta=*mac;
  printf("order max theta %d %d %f\n",order,maxparnode,theta);

  printf("set up the input\n");

  /***************constant*****************/
  pi=3.14159265358979324;
  one_over_4pi=0.079577471545948;
  bulk_coef=8.430325455;
  units_coef=332.0716;
  eps=epsw/epsp;
  kappa2=bulk_coef*bulk_strength/epsw;
  kappa=sqrt(kappa2);

  printf("eps kappa %f %f\n",eps,kappa);

  readin(fname,density);

  for(i=0;i<10;i++) printf("chrpos is %f\n",chrpos[i]);
  for(i=0;i<9;i++) printf("tr_xyz is %f, %f\n",tr_xyz[i],tr_q[i]);


  comp_source();
  /* tr_xyz=[x[i],y[i],z[i]] */
  /* tr_q=[qx[i],qy[i],qz[i]] */

  /* set up treecode */
  treecode_initialization(order,maxparnode,theta);

  /* parameters for GMRES */
  RESTRT=10;
  N=2*nface;
  ldw=N;
  ldh=RESTRT+1;
  iter=100;
  resid=1e-4;
  xvct=(double *) calloc(N,sizeof(double));

  work=(double *) calloc(ldw*(RESTRT+4),sizeof(double));
  h=(double *) calloc(ldh*(RESTRT+2),sizeof(double));

  for(i=0;i<10;i++) printf("bvct is %f\n",bvct[i]);
  gmres_(&N,bvct,xvct,&RESTRT,work,&ldw,h,&ldh,&iter,
         &resid,matvec,psolve,&info);

  /* the solvation energy computation */
  units_para=2.0;
  units_para=units_para*units_coef;
  units_para=units_para*pi; 

  chrptl=(double*) malloc(nface*sizeof(double));
  comp_pot(chrptl);

  soleng=0.0;
  for (i=0;i<nface;i++) soleng=soleng+chrptl[i];
  soleng=soleng*units_para;
  printf("solvation energy = %f kcal/mol\n",soleng);

  timer_end();

  /* free memory */
  for(i=0;i<3;i++) {
    free(extr_v[i]);
  }
  free(extr_v);

  for(i=0;i<3;i++) {
    free(vert[i]);
  }
  free(vert);

  for(i=0;i<3;i++) {
    free(snrm[i]);
  }
  free(snrm);

  for(i=0;i<3;i++) {
    free(face[i]);
  }
  free(face);

  for(i=0;i<3;i++) {
    free(extr_f[i]);
  }
  free(extr_f);
	
  for(i=0;i<3;i++) {
    free(atmpos[i]);
  }
  free(atmpos);

  free(tr_xyz);
  free(tr_q);

  free(tr_area);
  free(bvct);
  free(xvct);
  free(atmchr);
  free(chrpos);
  free(chrptl);

  /* dellocate treecode variables */
  treecode_finalization();

  return 0;

}

/************************************/
int *psolve(double *z, double *r){
/* r as original while z as scaled */
  int i;
  double scale1, scale2;
  scale1=0.5*(1.0+eps);
  scale2=0.5*(1.0+1.0/eps);
  for (i=0;i<nface;i++){
    z[i]=r[i]/scale1;
    z[i+nface]=r[i+nface]/scale2;
  }
  return 0;
}

/************************************/
int comp_source(){
  /* this compute the source term where
 *   S1=sum(qk*G0)/e1 S2=sim(qk*G0')/e1 */

  /* local variables */
  int i,j;
  double sumrs,cos_theta,irs,G0,G1,tp1;
  double r_s[3];

  for (i=0;i<nface;i++){
    bvct[i]=0.0;
    bvct[i+nface]=0.0;
    for (j=0;j<nchr;j++){
  /* r_s = distance of charge position to triangular */
      r_s[0]=chrpos[3*j]-tr_xyz[3*i];
      r_s[1]=chrpos[3*j+1]-tr_xyz[3*i+1];
      r_s[2]=chrpos[3*j+2]-tr_xyz[3*i+2];
      sumrs=r_s[0]*r_s[0]+r_s[1]*r_s[1]+r_s[2]*r_s[2];
  /* cos_theta = <tr_q,r_s>/||r_s||_2 */
      cos_theta=tr_q[3*i]*r_s[0]+tr_q[3*i+1]*r_s[1]+tr_q[3*i+2]*r_s[2];
      irs=1/sqrt(sumrs);
      cos_theta=cos_theta*irs;
  /* G0 = 1/(4pi*||r_s||_2) */
      G0=one_over_4pi;
      G0=G0*irs;
  /* G1 = cos_theta*G0/||r_s||_2 */
      tp1=G0*irs;
      G1=cos_theta*tp1;
  /* update bvct */
      bvct[i]+=atmchr[j]*G0;
      bvct[nface+i]+=atmchr[j]*G1;
    }
  }

  return 0;
}
/************************************/

/************************************/
int comp_pot(double *chrptl){
  /* local variables */
  int i,j;
  double sumrs,irs,rs,G0,Gk,kappa_rs,exp_kappa_rs;
  double cos_theta,G1,G2,L1,L2,tp1,tp2;
  double r[3],v[3],s[3],r_s[3];

  for (j=0;j<nface;j++){
    chrptl[j]=0.0;
  /* r[] = tr_xyz[] & v[] = tr_q[] */
    r[0]=tr_xyz[3*j];r[1]=tr_xyz[3*j+1];r[2]=tr_xyz[3*j+2];
    v[0]=tr_q[j*3];v[1]=tr_q[j*3+1];v[2]=tr_q[j*3+2];
    for (i=0;i<nchr;i++){
  /* s = chrpos[] & r_s = r[]-s[] */
      s[0]=chrpos[3*i];s[1]=chrpos[3*i+1];s[2]=chrpos[3*i+2];
      r_s[0]=r[0]-s[0];r_s[1]=r[1]-s[1];r_s[2]=r[2]-s[2];
      sumrs=r_s[0]*r_s[0]+r_s[1]*r_s[1]+r_s[2]*r_s[2];
      rs=sqrt(sumrs);
      irs=1/rs;

      G0=one_over_4pi;
      G0=G0*irs;
      kappa_rs=kappa*rs;
      exp_kappa_rs=exp(-kappa_rs);
      Gk=exp_kappa_rs*G0;

      cos_theta=(v[0]*r_s[0]+v[1]*r_s[1]+v[2]*r_s[2])*irs;
      
      tp1=G0*irs;
      tp2=(1.0+kappa_rs)*exp_kappa_rs;

      G1=cos_theta*tp1;
      G2=tp2*G1;

      L1=G1-eps*G2;
      L2=G0-Gk;

      chrptl[j]=chrptl[j]+atmchr[i]*(L1*xvct[j]+L2*xvct[nface+j])*tr_area[j];
    }
  }
  return 0;  
}
/************************************/

int *matvec_direct(double *alpha, double *x, double *beta, double *y){
  int i,j;
  double pre1,pre2;
  double area,rs,irs,sumrs;
  double G0,kappa_rs,exp_kappa_rs,Gk;
  double cos_theta,cos_theta0,tp1,tp2,dot_tqsq;
  double G10,G20,G1,G2,G3,G4;
  double L1,L2,L3,L4;
  double tp[3],tq[3],sp[3],sq[3],r_s[3];
  double peng[2],peng_old[2];

  pre1=0.50*(1.0+eps); /* eps=80.0 a constant */
  pre2=0.50*(1.0+1.0/eps); /* fdivide */
  for(i=0;i<nface;i++){

    tp[0]=tr_xyz[3*i];tp[1]=tr_xyz[3*i+1];tp[2]=tr_xyz[3*i+2];
    tq[0]=tr_q[3*i];tq[1]=tr_q[3*i+1];tq[2]=tr_q[3*i+2];

    peng[0]=0.0;peng[1]=0.0;
    for(j=0;j<nface;j++){
      if (j!=i){
        sp[0]=tr_xyz[3*j];sp[1]=tr_xyz[3*j+1];sp[2]=tr_xyz[3*j+2];
        sq[0]=tr_q[3*j];sq[1]=tr_q[3*j+1];sq[2]=tr_q[3*j+2];
        r_s[0]=sp[0]-tp[0];r_s[1]=sp[1]-tp[1];r_s[2]=sp[2]-tp[2];
        sumrs=r_s[0]*r_s[0]+r_s[1]*r_s[1]+r_s[2]*r_s[2];
        rs=sqrt(sumrs);
        irs=1/rs;
        G0=one_over_4pi;
        G0=G0*irs;
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
        L4=G10-G20/eps; /* fdivide */
        /* x involve first */
        peng_old[0]=x[j];peng_old[1]=x[j+nface];
        area=tr_area[j];
        peng[0]=peng[0]+(L1*peng_old[0]+L2*peng_old[1])*area;
        peng[1]=peng[1]+(L3*peng_old[0]+L4*peng_old[1])*area;
      }
    }
    /* update the y value */
    y[i]=y[i]* *beta+(pre1*x[i]-peng[0])* *alpha;
    y[nface+i]=y[nface+i]* *beta+(pre2*x[nface+i]-peng[1])* *alpha;

  }
  return 0;
}

