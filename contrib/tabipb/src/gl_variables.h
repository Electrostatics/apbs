/*constant variables */

/*#define pi 3.14159265358979324;
#define one_over_4pi 0.079577471545948; //Note: Stupid putting f

#define bulk_coef 8.430325455;
#define units_coef 332.0716;

#define epsw 80.0;
#define epsp 1.0;
#define bulk_strength 0.15;   //ion_strength in M
double eps,kappa2,kappa;
*/


/*const double pi=3.14159265358979324;
const double one_over_4pi=0.079577471545948;
const double bulk_coef=8.430325455;
const double units_coef=332.0716;
const double epsw=80.0; 
const double epsp=1.0;
const double eps=80.0;
const double bulk_strength=0.15;   	//ion_strength in M
const double kappa2=0.0158068602;	//kappa2=bulk_coef*bulk_strength/epsw;
const double kappa=0.1257253365;
*/

      
/*global scalar variables*/
int nface, nspt, natm, nchr;

/*dynamic allocated variables*/
int **extr_v; //[3][nspt]
int **extr_f; //[2][nface]
int **face,**face_copy;//[3][nface]


double **vert, **snrm; //[3][nspt];
double *tr_xyz, *tr_q; //[3][nface]
double *tr_area,*bvct,*xvct; //[nface];
double **atmpos; //[3][natm/nchr];
double *atmrad, *atmchr, *chrpos; //[natm/nchr]; 

double *work, *h;
//extern int readin(void);

/*device pointers*/
double *h_pot;
double *dev_xp, *dev_yp, *dev_zp, *dev_q, *dev_pot;
