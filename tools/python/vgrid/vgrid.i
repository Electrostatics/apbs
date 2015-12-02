/* Input file for creating Python wrappers for Vgrid via swig
   Author: Todd Dolinsky
   Email: todd@ccb.wustl.edu
 
Header files:
-----------------------
*/ 

%module apbslib

%{
#include "routines.h"
#include "mg/vgrid.h"
%}

#define VEXTERNC extern

// Generic array of doubles:

%inline %{
double *null_array(){
     return NULL;
}
%}

typedef struct {
	Vgrid();
	~Vgrid();
	int nx;
	int ny;
	int nz;
	double hx;
	double hy;
	double hzed;
	double xmin;
	double ymin;
	double zmin;
    double *data;
} Vgrid;

%inline %{
void delete_vgrid(Vgrid *thee){
    if (thee != VNULL) {
        Vmem_free(thee->mem, (thee->nx*thee->ny*thee->nz), sizeof(double),
          (void **)&(thee->data));
        Vmem_free(VNULL, 1, sizeof(Vgrid), (void **)&thee);
        thee = VNULL;
    }
}
%}

extern int Vgrid_ctor2(Vgrid *thee, int nx, int ny, int nz, double hx, 
					double hy, double hzed, double xmin, double ymin, 
					double zmin, double *data);
extern void Vgrid_dtor(Vgrid **thee);
extern void Vgrid_dtor2(Vgrid *thee);

extern void Vgrid_writeUHBD(Vgrid *thee, const char *iodev, const char *iofmt, const char *thost, const char *fname, char *title, double *pvec);

extern void Vgrid_writeDX(Vgrid *thee, const char *iodev, const char *iofmt, const char *thost, const char *fname, char *title, double *pvec);

extern int Vgrid_readDX(Vgrid *thee, const char *iodev, const char *iofmt, const char *thost, const char *fname);

extern void startVio();

// Typemaps and functions for easy Python Access

%include typemaps.i

%typemap(in) double [3] {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (double *) malloc((size+1)*sizeof(double));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyFloat_Check(o))
	    $1[i] = PyFloat_AsDouble(PyList_GetItem($input,i));
      else {
	    PyErr_SetString(PyExc_TypeError,"list must contain floats");
	    free($1);
	    return NULL;
      }
    }
    $1[i] = 0;
  } else {
        PyErr_SetString(PyExc_TypeError,"not a list");
        return NULL;
  }
}

extern int Vgrid_value(Vgrid *thee, double x[3], double *INOUT);

%typemap(in) double * {
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (double *) malloc((size+1)*sizeof(double));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyFloat_Check(o))
	    $1[i] = PyFloat_AsDouble(PyList_GetItem($input,i));
      else {
	    PyErr_SetString(PyExc_TypeError,"list must contain floats");
	    free($1);
	    return NULL;
      }
    }
    $1[i] = 0;
  } else {
        PyErr_SetString(PyExc_TypeError,"not a list");
        return NULL;
  }
}


extern int Vgrid_curvature(Vgrid *thee, double pt[3], int cflag, double *curv);
extern int Vgrid_gradient(Vgrid *thee, double pt[3], double grad[3]);
extern Vgrid* Vgrid_ctor(int nx, int ny, int nz, double hx, double hy,
						 double hzed, double xmin, double ymin, double zmin, 
						 double *data);
