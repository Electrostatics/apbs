/* draw_membrane2.c                     09/02/08 *
 *-----------------------------------------------* 
 * By Michael Grabe                              *
 * This program takes the dielectric, kappa, and * 
 * charge  maps from APBS and accounts for the   *
 * membrane. the thickness and the bottom of the *
 * membrane must be given. we assume that the    *
 * membrane normal is along the z-axis. this     *
 * requires lining the protein along the z-axis  *
 * before running this program.                  *
 * We also output a charge_change_map that tells *
 * me which positions in the charge matrix were  *
 * edited by the addition of the membrane        *
 * NOTE: this program was changes in 2005 to     *
 * allow for conical channel geometries. An      *
 * extra command line argument was added that    *
 * will cause a fault if run without it from     *
 * older scripts pre 2005.                       *
 *                                               * 
 * INPUTS:                                       *
 * thses all come at the command line:           *
 * x_map  - name is used to find all maps        *
 * z_m0   - bottom of the membrane               *
 * l_m    - length of the membrane               *
 * pdie   - protein dielectric constant          *
 * V      - cytoplasmic potential (kT/e)         *
 * I      - molar conc. of one salt-species      *
 * R_m1   - excl. radius at top of  membrane     *
 * R_m0   - excl. radius at  bottom of membrane  *
 *                                               * 
 * OUTPUTS:                                      *
 *   maps                                        *
 *-----------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/********************************************************************/
/* INPUT LOOKS LIKE:                                                */
/* mgrabe% draw_mem  dielx*.dx z_m0 l_m pdie V I R_m1 R_m0          */
/********************************************************************/
void printhelp()
{
printf("* draw_membrane2.c                     09/02/08 *\n"
		"*-----------------------------------------------*\n"
		"* By Michael Grabe                              *\n"
		"* This program takes the dielectric, kappa, and *\n" 
		"* charge  maps from APBS and accounts for the   *\n"
		"* membrane. the thickness and the bottom of the *\n"
		"* membrane must be given. we assume that the    *\n"
		"* membrane normal is along the z-axis. this     *\n"
		"* requires lining the protein along the z-axis  *\n"
		"* before running this program.                  *\n"
		"* We also output a charge_change_map that tells *\n"
		"* me which positions in the charge matrix were  *\n"
		"* edited by the addition of the membrane        *\n"
		"* NOTE: this program was changes in 2005 to     *\n"
		"* allow for conical channel geometries. An      *\n"
		"* extra command line argument was added that    *\n"
		"* will cause a fault if run without it from     *\n"
		"* older scripts pre 2005.                       *\n"
		"*                                               *\n" 
		"* INPUTS:                                       *\n"
		"* thses all come at the command line:           *\n"
		"* x_map  - name is used to find all maps        *\n"
		"* z_m0   - bottom of the membrane               *\n"
		"* l_m    - length of the membrane               *\n"
		"* pdie   - protein dielectric constant          *\n"
		"* V      - cytoplasmic potential (kT/e)         *\n"
		"* I      - molar conc. of one salt-species      *\n"
		"* R_m1   - excl. radius at top of  membrane     *\n"
		"* R_m0   - excl. radius at  bottom of membrane  *\n"
		"*                                               *\n" 
		"* OUTPUTS:                                      *\n"
		"*   maps                                        *\n"
		"*-----------------------------------------------*\n\n"
		"********************************************************************\n"
		"* INPUT LOOKS LIKE:                                                *\n"
		"* ./draw_membrane2  dielx*.dx z_m0 l_m pdie V I R_m1 R_m0          *\n"
		"********************************************************************\n\n"
	  );
}

int main(int argc, char *argv[])
{

int dim_x,dim_y,dim_z,dim3,i,j,k,cnt;
int tmp1,tmp2,tmp3,tmp4;
int l, *map;
FILE *out, *in; 
float *d_x, *d_y, *d_z;      
float *x_x, *x_y, *x_z;
float *y_x, *y_y, *y_z;
float *z_x, *z_y, *z_z;
float *kk, *cc;
float *x, *y, *z;
float tmp,tmp_x,tmp_y,tmp_z,dx,dy,dz,l_c_x,l_c_y,l_c_z,l_m;
float x0_p, y0_p, z0_p;
float x0_x, y0_x, z0_x;
float x0_y, y0_y, z0_y; 
float x0_z, y0_z, z0_z; 
float x0, y0, z0;
float V, I, sdie;
float z_m0, z_m1, R_m0, R_m1, R_x, R_y, R_z, R, pdie, mdie;
float R_temp;
char s[100], file_name_x[20], file_name_y[20], file_name_z[20];
char file_name_k[20], file_name_c[20];
char f1[21], f2[21], f3[21], f4[21], f5[21], f6[21];
char ext[5]="m.dx", mid[21];

if (argc <= 1) {
	printhelp();
	return 0;
}

strcpy(file_name_x,argv[1]);

printf("Using your naming scheme to find other APBS maps.\n");

/* Find the y-shifted dielectric map */
l=strlen(file_name_x);
strncpy(mid, &file_name_x[5], l - 3 - 5); 
mid[2]=0;
strncpy(file_name_y, "diely", 5);  /* take off the .dx */  
file_name_y[5]=0;
strcat(file_name_y,mid);
file_name_y[l-3]=0;
strcat(file_name_y,".dx");

/* Find the z-shifted dielectric map */
strncpy(file_name_z, "dielz", 5);  /* take off the .dx */
file_name_z[5]=0;
strcat(file_name_z,mid);
file_name_z[l-3]=0;
strcat(file_name_z,".dx");

/* Find the kappa map */
strncpy(file_name_k, "kappa", 5);  
file_name_k[5]=0;
strcat(file_name_k,mid); 
file_name_k[l-3]=0;
strcat(file_name_k,".dx");

/* Find the charge map */
strncpy(file_name_c, "charge", 6); 
file_name_c[6]=0;
strcat(file_name_c, mid);   /* add in the extension */ 
file_name_c[l-2]=0;
strcat(file_name_c,".dx");  /* add back .dx         */

z_m0=atof(argv[2]);

l_m=atof(argv[3]);

pdie=atof(argv[4]);

V=atof(argv[5]);

I=atof(argv[6]);

R_m1=atof(argv[7]);
R_m0=atof(argv[8]);

z_m1=z_m0+l_m;   /* top of the membrane */
mdie = 2.0;    /* watch out for this it used to be 10.0 */ 
sdie = 80.0;
/*****************************************************/
/* read in the x-shifted dielectric data             */
/*****************************************************/

in = fopen(file_name_x,"r");
if (in == NULL) {
	printhelp();
	printf("Make sure %s exists in current directory!!!\n\n", argv[1]);
	return 0;
}

/* First read the header */

fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);   
fscanf(in, "%6s %1s %5s %13s %6s %i %i %i \n", s,s,s,s,s,&dim_x,&dim_y,&dim_z);
fscanf(in, "%6s %f %f %f \n",s, &x0_x, &y0_x, &z0_x);
fscanf(in, "%5s %f %f %f \n",s, &dx, &tmp, &tmp);
fscanf(in, "%5s %f %f %f \n",s, &tmp, &dy, &tmp);
fscanf(in, "%5s %f %f %f \n",s, &tmp, &tmp, &dz);
fgets(s,100,in);
fscanf(in, "%6s %i %5s %5s %4s %6s %4s %i %5s %i %4s %7s \n",s,&tmp,s,s,s,s,s,&tmp,s,&dim3,s,s);


/* assign the memory to the arrays */

x_x= (float *) calloc(dim_x+1,sizeof(float));
y_x= (float *) calloc(dim_y+1,sizeof(float));
z_x= (float *) calloc(dim_z+1,sizeof(float));
x_y= (float *) calloc(dim_x+1,sizeof(float));
y_y= (float *) calloc(dim_y+1,sizeof(float));
z_y= (float *) calloc(dim_z+1,sizeof(float));
x_z= (float *) calloc(dim_x+1,sizeof(float));
y_z= (float *) calloc(dim_y+1,sizeof(float));
z_z= (float *) calloc(dim_z+1,sizeof(float));
d_x= (float *) calloc(dim_x*dim_y*dim_z+1,sizeof(float));
d_y= (float *) calloc(dim_x*dim_y*dim_z+1,sizeof(float));
d_z= (float *) calloc(dim_x*dim_y*dim_z+1,sizeof(float));
/* Now the Kappa and charge Arrays */
x= (float *) calloc(dim_x+1,sizeof(float));
y= (float *) calloc(dim_y+1,sizeof(float));
z= (float *) calloc(dim_z+1,sizeof(float));
kk= (float *) calloc(dim_x*dim_y*dim_z+1,sizeof(float));
cc= (float *) calloc(dim_x*dim_y*dim_z+1,sizeof(float));
map= (int *) calloc(dim_x*dim_y*dim_z+1,sizeof(int));

/* initialize x,y,z, and diel vectors */

l_c_x=dim_x*dx;
l_c_y=dim_y*dx;
l_c_z=dim_z*dx;

tmp_x=x0_x;
tmp_y=y0_x;
tmp_z=z0_x;

for (i=1; i <= dim_x; ++i)
{
x_x[i]=tmp_x;
tmp_x+=dx;
}

for (i=1; i <= dim_y; ++i)
{
y_x[i]=tmp_y;
tmp_y+=dy;
}

for (i=1; i <= dim_z; ++i)
{
z_x[i]=tmp_z;
tmp_z+=dz;
}

/* Read in the rest of the dielectric data */

tmp1 = fmod(dim3, 3); 
tmp2 =(dim3-tmp1)/3;    /* total lines less one left in file */ 
tmp3 =1;

for (i=1; i <= tmp2; ++i)
{
fscanf(in,"%f %f %f \n", &d_x[tmp3], &d_x[tmp3+1], &d_x[tmp3+2]); 
tmp3+=3;
}

if (tmp1 == 1)
fscanf(in,"%f \n", &d_x[tmp3]);    /* reading in the last line */
else if (tmp1 == 2)
fscanf(in,"%f %f \n", &d_x[tmp3], &d_x[tmp3+1]);

/*****************************************************/

/****************************************************/
/* Construct the protein cooridinate center based   */
/* on the x-dielectric map.                         */
/* This will be used to determine where to add      */
/* membrane.                                        */ 
/****************************************************/

x0_p=x0_x+l_c_x/2-dx/2;  /* this is the shift term that */ 
                         /* moves half-step off grid    */ 
y0_p=y0_x+l_c_y/2;
z0_p=z0_x+l_c_z/2;

fclose(in);

/*****************************************************/
/* read in the y-shifted dielectric data             */
/*****************************************************/

in = fopen(file_name_y,"r");

/* First read the header */

fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fscanf(in, "%6s %f %f %f \n",s, &x0_y, &y0_y, &z0_y);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fscanf(in, "%6s %i %5s %5s %4s %6s %4s %i %5s %i %4s %7s \n",s,&tmp,s,s,s,s,s,&tmp,s, &tmp,s,s); 
 
/* initialize x,y,z, and diel vectors */

tmp_x=x0_y;
tmp_y=y0_y;
tmp_z=z0_y;

for (i=1; i <= dim_x; ++i) {
x_y[i]=tmp_x;
tmp_x+=dx;
}

for (i=1; i <= dim_y; ++i)
{
y_y[i]=tmp_y;
tmp_y+=dy;
}

for (i=1; i <= dim_z; ++i)
{
z_y[i]=tmp_z;
tmp_z+=dz;
}

/* Read in the rest of the dielectric data */

tmp3 =1;

for (i=1; i <= tmp2; ++i)
{
fscanf(in,"%f %f %f \n", &d_y[tmp3], &d_y[tmp3+1], &d_y[tmp3+2]);
tmp3+=3;
}

if (tmp1 == 1)
fscanf(in,"%f \n", &d_y[tmp3]);    /* reading in the last line */
else if (tmp1 == 2)
fscanf(in,"%f %f \n", &d_y[tmp3], &d_y[tmp3+1]);

fclose(in);

/*****************************************************/
/* read in the z-shifted dielectric data             */
/*****************************************************/

in = fopen(file_name_z,"r");

/* First read the header */

fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fscanf(in, "%6s %f %f %f \n",s, &x0_z, &y0_z, &z0_z);
fgets(s,100,in);                                                 
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fscanf(in, "%6s %i %5s %5s %4s %6s %4s %i %5s %i %4s %7s \n",s,&tmp,s,s,s,s,s,&tmp,s, &tmp,s,s);

/* initialize x,y,z, and diel vectors */

tmp_x=x0_z;
tmp_y=y0_z;
tmp_z=z0_z;


for (i=1; i <= dim_x; ++i)
{
x_z[i]=tmp_x;
tmp_x+=dx;
}

for (i=1; i <= dim_y; ++i)
{
y_z[i]=tmp_y;
tmp_y+=dy;
}

for (i=1; i <= dim_z; ++i)
{
z_z[i]=tmp_z;
tmp_z+=dz;
}


/* Read in the rest of the dielectric data */

tmp3 =1;

for (i=1; i <= tmp2; ++i)
{ 
fscanf(in,"%f %f %f \n", &d_z[tmp3], &d_z[tmp3+1], &d_z[tmp3+2]);
tmp3+=3;
}

if (tmp1 == 1) 
fscanf(in,"%f \n", &d_z[tmp3]);    /* reading in the last line */
else if (tmp1 == 2)
fscanf(in,"%f %f \n", &d_z[tmp3], &d_z[tmp3+1]);

fclose(in);

/*****************************************************/

/*****************************************************/
/* read in the kappa data                            */
/*****************************************************/

in = fopen(file_name_k,"r");

/* First read the header */

fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fscanf(in, "%6s %f %f %f \n",s, &x0, &y0, &z0);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);

/* initialize x,y,z, and kappa vectors */

tmp_x=x0;
tmp_y=y0;
tmp_z=z0;


for (i=1; i <= dim_x; ++i)
{
x[i]=tmp_x;
tmp_x+=dx;
}

for (i=1; i <= dim_y; ++i)
{
y[i]=tmp_y;
tmp_y+=dy;
}

for (i=1; i <= dim_z; ++i)
{
z[i]=tmp_z;
tmp_z+=dz;
}

/* Read in the rest of the Kappa data */


tmp3 =1;

for (i=1; i <= tmp2; ++i)
{
fscanf(in,"%f %f %f \n", &kk[tmp3], &kk[tmp3+1], &kk[tmp3+2]);
tmp3+=3;
}

if (tmp1 == 1)
fscanf(in,"%f \n", &kk[tmp3]);    /* reading in the last line */
else if (tmp1 == 2)
fscanf(in,"%f %f \n", &kk[tmp3], &kk[tmp3+1]);

fclose(in);

/*****************************************************/

/*****************************************************/
/* read in the charge data                           */
/*****************************************************/

in = fopen(file_name_c,"r");

/* First read the header */

fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);
fgets(s,100,in);

/* Read in the rest of the charge data */

tmp3 =1;

for (i=1; i <= tmp2; ++i)
{
fscanf(in,"%f %f %f \n", &cc[tmp3], &cc[tmp3+1], &cc[tmp3+2]);
tmp3+=3;
}

if (tmp1 == 1)
fscanf(in,"%f \n", &cc[tmp3]);    /* reading in the last line */
else if (tmp1 == 2)
fscanf(in,"%f %f \n", &cc[tmp3], &cc[tmp3+1]);

fclose(in);

/*****************************************************/
/* MANIPULATE THE DATA BY ADDING THE MEMBRANE        */      
/*****************************************************/


/******************************************************/
/* set up the vector                                  */
/******************************************************/


cnt=1;

for (k=1; k <= dim_x; ++k)  /* loop over z */
{
        for (j=1; j <= dim_y; ++j)  /* loop over y */
	{	
		for (i=1; i <= dim_z; ++i)  /* loop over x */
	        {	
                        R_x = sqrt((x_x[k]-x0_p)*(x_x[k]-x0_p) + (y_x[j]-y0_p)*(y_x[j]-y0_p));	
		        R_temp = (R_m1*(z_x[i]-z_m0) - R_m0*(z_x[i]-z_m1))/(z_m1 - z_m0);  	

                        if (z_x[i] <= z_m1 && z_x[i] >= z_m0 && R_x > R_temp && d_x[cnt] > pdie+0.05) 
		        {	
                        d_x[cnt] = mdie;   /* bilayer dielectric constant */
                        }

                        R_y = sqrt((x_y[k]-x0_p)*(x_y[k]-x0_p) + (y_y[j]-y0_p)*(y_y[j]-y0_p));
                        R_temp = (R_m1*(z_y[i]-z_m0) - R_m0*(z_y[i]-z_m1))/(z_m1 - z_m0);
 
                        if (z_y[i] <= z_m1 && z_y[i] >= z_m0 && R_y > R_temp && d_y[cnt] > pdie+0.05)
                        {      
                        d_y[cnt] = mdie;   /* bilayer dielectric constant */
                        }

                        R_z = sqrt((x_z[k]-x0_p)*(x_z[k]-x0_p) + (y_z[j]-y0_p)*(y_z[j]-y0_p));
                        R_temp = (R_m1*(z_z[i]-z_m0) - R_m0*(z_z[i]-z_m1))/(z_m1 - z_m0);

                        if (z_z[i] <= z_m1 && z_z[i] >= z_m0 && R_z > R_temp && d_z[cnt] > pdie+0.05)
                        {      
                        d_z[cnt] = mdie;   /* bilayer dielectric constant */
                        }

                        R = sqrt((x[k]-x0_p)*(x[k]-x0_p) + (y[j]-y0_p)*(y[j]-y0_p));

                        if (z[i] <= z_m0 && kk[cnt] != 0.0)
                        {
                        /* charge for mem V */
                        /* see my notes for this expression */
                        cc[cnt] = 0.0012045*I*V;
                        /* update the change map */
                        map[cnt] = 1;
                        }
                        else
                        {
                        /* position was not changed */
                        map[cnt] = 0;
                        }
                      
                        R_temp = (R_m1*(z[i]-z_m0) - R_m0*(z[i]-z_m1))/(z_m1 - z_m0);

                        if (z[i] <= z_m1 && z[i] >= z_m0 && R > R_temp)
                        {
                        kk[cnt] = 0.0;   /* Zero ion accessibility */
                        }

                        ++cnt;

  	        }			
	}

}

/********************************************************/
/* now we must save diel as a text document             */
/* in the proper 3 column output                        */
/********************************************************/



/********************************************************/

/* add the "m" extension to the file */
l=strlen(file_name_x);
strncpy(f1, file_name_x, l-3);  /* take off the .dx */
f1[l-3]=0;                      /* add the null terminal */
strcat(f1,ext);            /* add back m.dx  */
out = fopen(f1,"w");

/* MAKE THE X HEADER FILE */

fprintf(out, "# Data from draw_membrane.c \n");
fprintf(out, "# \n");
fprintf(out, "# X-SHIFTED DIELECTRIC MAP with membrane: zmem = %4.2f, Lmem = %4.2f \n",z_m0, l_m);
fprintf(out, "# \n");
fprintf(out, "object 1 class gridpositions counts %i %i %i \n", dim_x, dim_y, dim_z);
fprintf(out, "origin %12.6E %12.6E %12.6E \n", x0_x, y0_x, z0_x);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", dx,0.000000E+00,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,dy,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,0.000000E+00,dz);
fprintf(out, "object 2 class gridconnections counts %i %i %i\n", dim_x, dim_y, dim_z);
fprintf(out, "object 3 class array type double rank 0 items %i data follows\n", dim_x*dim_y*dim_z);

/* ADD THE X DATA */

cnt=1;

for (i=1; i <= tmp2  ; ++i)
{	
fprintf(out, "%12.6E %12.6E %12.6E \n", d_x[cnt], d_x[cnt+1], d_x[cnt+2]);
cnt=cnt+3;
}


if (tmp1 == 1)
fprintf(out,"%12.6E \n", d_x[cnt]);    /* saving in the last line */
else if (tmp1 == 2)
fprintf(out,"%12.6E %12.6E \n", d_x[cnt], d_x[cnt+1]);

fclose(out);

/********************Y-DATA******************************/

/* give the file an "m" extension */

l=strlen(file_name_y);
strncpy(f2, file_name_y, l-3);  /* take off the .dx */
f2[l-3]=0;                      /* add the null terminal */
strcat(f2,ext);               /* add back m.dx  */
out = fopen(f2,"w");
     
/* MAKE THE Y HEADER FILE */

fprintf(out, "# Data from draw_membrane.c \n");
fprintf(out, "# \n");
fprintf(out, "# Y-SHIFTED DIELECTRIC MAP with membrane: zmem = %4.2f, Lmem = %4.2f \n",z_m0, l_m);
fprintf(out, "# \n");
fprintf(out, "object 1 class gridpositions counts %i %i %i \n", dim_x, dim_y, dim_z);
fprintf(out, "origin %12.6E %12.6E %12.6E \n", x0_y, y0_y, z0_y);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", dx,0.000000E+00,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,dy,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,0.000000E+00,dz);
fprintf(out, "object 2 class gridconnections counts %i %i %i\n", dim_x, dim_y, dim_z)
;
fprintf(out, "object 3 class array type double rank 0 items %i data follows\n", dim_x*dim_y*dim_z);

/* ADD THE Y DATA */

cnt=1;

for (i=1; i <= tmp2  ; ++i)
{      
fprintf(out, "%12.6E %12.6E %12.6E \n", d_y[cnt], d_y[cnt+1], d_y[cnt+2]);
cnt=cnt+3;
}

if (tmp1 == 1)
fprintf(out,"%12.6E \n", d_y[cnt]);    /* saving in the last line */ 
else if (tmp1 == 2)
fprintf(out,"%12.6E %12.6E \n", d_y[cnt], d_y[cnt+1]);

fclose(out);

/**********************Z-DATA*****************************/


/* give the file an "m" extension */
l=strlen(file_name_z);
strncpy(f3, file_name_z, l-3);  /* take off the .dx */
f3[l-3]=0;                      /* add the null terminal */ 
strcat(f3,ext);            /* add back m.dx  */
out = fopen(f3,"w");


/* MAKE THE Z HEADER FILE */

fprintf(out, "# Data from draw_membrane.c \n");
fprintf(out, "# \n");
fprintf(out, "# Z-SHIFTED DIELECTRIC MAP with membrane: zmem = %4.2f, Lmem = %4.2f \n",z_m0, l_m);
fprintf(out, "# \n");
fprintf(out, "object 1 class gridpositions counts %i %i %i \n", dim_x, dim_y, dim_z);
fprintf(out, "origin %12.6E %12.6E %12.6E \n", x0_z, y0_z, z0_z);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", dx,0.000000E+00,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,dy,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,0.000000E+00,dz);
fprintf(out, "object 2 class gridconnections counts %i %i %i\n", dim_x, dim_y, dim_z);
fprintf(out, "object 3 class array type double rank 0 items %i data follows\n", dim_x*dim_y*dim_z);

/* ADD THE Z DATA */

cnt=1;

for (i=1; i <= tmp2  ; ++i)
{      
fprintf(out, "%12.6E %12.6E %12.6E \n", d_z[cnt], d_z[cnt+1], d_z[cnt+2]);
cnt=cnt+3;
}


if (tmp1 == 1)
fprintf(out,"%12.6E \n", d_z[cnt]);    /* saving in the last line */ 
else if (tmp1 == 2)
fprintf(out,"%12.6E %12.6E \n", d_z[cnt], d_z[cnt+1]);

fclose(out);

/*********************KAPPA******************************/

/* give the file an "m" extension */
l=strlen(file_name_k);
strncpy(f4, file_name_k, l-3);  /* take off the .dx */
f4[l-3]=0;                      /* add the null terminal */
strcat(f4,ext);               /* add back m.dx  */
out = fopen(f4,"w");


/* MAKE THE KAPPA HEADER FILE */

fprintf(out, "# Data from draw_membrane.c \n");
fprintf(out, "# \n");
fprintf(out, "# KAPPA MAP with membrane: zmem = %4.2f, Lmem = %4.2f \n",z_m0, l_m);
fprintf(out, "# \n");
fprintf(out, "object 1 class gridpositions counts %i %i %i \n", dim_x, dim_y, dim_z);
fprintf(out, "origin %12.6E %12.6E %12.6E \n", x0, y0, z0);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", dx,0.000000E+00,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,dy,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,0.000000E+00,dz);
fprintf(out, "object 2 class gridconnections counts %i %i %i\n", dim_x, dim_y, dim_z);
fprintf(out, "object 3 class array type double rank 0 items %i data follows\n", dim_x*dim_y*dim_z);

/* ADD THE KAPPA DATA */

cnt=1;

for (i=1; i <= tmp2  ; ++i)
{
fprintf(out, "%12.6E %12.6E %12.6E \n", kk[cnt], kk[cnt+1], kk[cnt+2]);
cnt=cnt+3;
}

if (tmp1 == 1)
fprintf(out,"%12.6E \n", kk[cnt]);    /* saving in the last line */
else if (tmp1 == 2)
fprintf(out,"%12.6E %12.6E \n", kk[cnt], kk[cnt+1]);

fclose(out);

/********************CHARGE*******************************/

/* give the file an "m" extension */
l=strlen(file_name_c);
strncpy(f5, file_name_c, l-3);  /* take off the .dx */
f5[l-3]=0;                      /* add the null terminal */
strcat(f5,ext);               /* add back m.dx  */
out = fopen(f5,"w");

/* MAKE THE CHARGE HEADER FILE */

fprintf(out, "# Data from draw_membrane.c \n");
fprintf(out, "# \n");
fprintf(out, "# CHARGE MAP with membrane: zmem = %4.2f, Lmem = %4.2f \n",z_m0, l_m);
fprintf(out, "# \n");
fprintf(out, "object 1 class gridpositions counts %i %i %i \n", dim_x, dim_y, dim_z);
fprintf(out, "origin %12.6E %12.6E %12.6E \n", x0, y0, z0);      
fprintf(out, "delta %12.6E %12.6E %12.6E \n", dx,0.000000E+00,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,dy,0.000000E+00); 
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,0.000000E+00,dz);
fprintf(out, "object 2 class gridconnections counts %i %i %i\n", dim_x, dim_y, dim_z);
fprintf(out, "object 3 class array type double rank 0 items %i data follows\n", dim_x*dim_y*dim_z);

/* ADD THE CHARGE DATA */ 

cnt=1;

for (i=1; i <= tmp2  ; ++i)
{
fprintf(out, "%12.6E %12.6E %12.6E \n", cc[cnt], cc[cnt+1], cc[cnt+2]);   
cnt=cnt+3;
}

if (tmp1 == 1)
fprintf(out,"%12.6E \n", cc[cnt]);    /* saving in the last line */ 
else if (tmp1 == 2)
fprintf(out,"%12.6E %12.6E \n", cc[cnt], cc[cnt+1]);  

fclose(out);

/********************CHARGE CHANGE MAP*************************/

strcpy (f6,"change_map");
strcat(f6, &file_name_c[6]);  /* add the appropriate _ext.dx */
out = fopen(f6,"w");

/* MAKE THE CHARGE HEADER FILE */

fprintf(out, "# Data from draw_membrane.c \n");
fprintf(out, "# \n");
fprintf(out, "# CHARGE CHANGE MAP with membrane: zmem = %4.2f, Lmem = %4.2f \n",z_m0, l_m);
fprintf(out, "# \n");
fprintf(out, "object 1 class gridpositions counts %i %i %i \n", dim_x, dim_y, dim_z);
fprintf(out, "origin %12.6E %12.6E %12.6E \n", x0, y0, z0);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", dx,0.000000E+00,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,dy,0.000000E+00);
fprintf(out, "delta %12.6E %12.6E %12.6E \n", 0.000000E+00,0.000000E+00,dz);
fprintf(out, "object 2 class gridconnections counts %i %i %i\n", dim_x, dim_y, dim_z);
fprintf(out, "object 3 class array type double rank 0 items %i data follows\n",
dim_x*dim_y*dim_z);

/* ADD THE CHARGE CHANGE DATA */

cnt=1;

for (i=1; i <= tmp2  ; ++i)
{
fprintf(out, "%i %i %i \n", map[cnt], map[cnt+1], map[cnt+2]);
cnt=cnt+3;
}

if (tmp1 == 1)
fprintf(out,"%i \n", map[cnt]);    /* saving in the last line */
else if (tmp1 == 2)
fprintf(out,"%i %i \n", map[cnt], map[cnt+1]);

fclose(out);

/***********************************************************/
free(x_x);
free(y_x);
free(z_x);
free(x_y);
free(y_y);
free(z_y); 
free(x_z);
free(y_z);
free(z_z); 
free(x);
free(y);
free(z);

printf("Your files have been written.\n");

}
