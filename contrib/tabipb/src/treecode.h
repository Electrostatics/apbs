/* Author Jiahui Chen
 * Advisor Weihua Geng
 * 4/5/2016
 * A new version for treecode PB */

/* runtime parameters */
int numpars, order, maxparnode, iflag, forcedim;
double theta;
double center[3], r0[3], v0[3];

/* arrays for coordinates, charge, potential & force */
double *x, *y, *z, *q;
double *x_copy, *y_copy, *z_copy, *q_copy;
//double *tpoten, *dpoten;
double **tforce, **dforce;
int *orderind, *n_clst;

/* timing variables */
double timebeg, timeend;

/* local variables */
double xyzminmax[6];
double t1, abserr, relerr, adsinf_err, relinf_err;
double f_inferr[3], f_relinferr[3], t[3]; 

double ***tchg, ***schg;
double ****der_cof;
int kk[16][3];

/* global variables for taylor expansions */
int torder, torder2, torderlim;
double *cf, *cf1, *cf2, *cf3;
double ***a, ***b;

/* global variables to track tree levels  */
int minlevel, maxlevel;

/* global variables used when computing potential/force */
int orderoffset;
double tarpos[3], tarq[3];
double tarchr;
double ***tchg, ***schg;

/* global variables for position and charge storage */
/* arrays are not copied in this version!! orderarr is till valid*/
int *orderarr;
double *xcopy,*ycopy,*zcopy,*qcopy;

/* node pointer and node struct declarations */
//typedef struct tnode tnode;

typedef struct tnode{
  int node_idx;
  int numpar, ibeg, iend;
  double x_min, y_min, z_min;
  double x_max, y_max, z_max;
  double x_mid, y_mid, z_mid;
  double radius, aspect;
  int level, num_children, exist_ms;
  double ****ms;
  struct tnode** child;
}tnode;

tnode* troot;
