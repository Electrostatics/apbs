#ifndef GL_FUNCTION
#define GL_FUNCTION

#include <stdlib.h>
#include <stdio.h>

/*dynamic allocation of 2d double variables*/
double** Make2DDoubleArray(int arraySizeX, int arraySizeY, char info[]) 
{  
	int i;
	double** theArray;  
	if ((theArray = (double**) calloc(arraySizeX,sizeof(double*)))== NULL) {
		printf("error in allocating pointer arrays of %s\n",info);
	}

	for (i=0; i<arraySizeX; i++)
	{
		if ((*(theArray+i) = (double*) calloc(arraySizeY,sizeof(double)))== NULL) {
			printf("error in allocating arrays of %s\n",info);
		};  
		//for (j=0; j<arraySizeY; j++) theArray[i][j]=0.0;
	}
	return theArray;  
} 

/*dynamic allocation of 2d integer variables*/
int** Make2DIntArray(int arraySizeX, int arraySizeY,char info[]) 
{  
	int i;
	int** theArray;  
	if ((theArray = (int**) calloc(arraySizeX,sizeof(int*)))==NULL) {
		printf("error in allocating pointer arrays of %s\n",info);
	}; 
	for (i=0; i<arraySizeX; i++)
	{
		if ((*(theArray+i) = (int*) calloc(arraySizeY,sizeof(int)))==NULL) {
			printf("error in allocating arrays of %s\n",info);
		}
		//for (j=0; j<arraySizeY; j++) theArray[i][j]=0;
	}
	return theArray;
}
/*dynamic allocation of 2d float variables*/
float** Make2DFloatArray(int arraySizeX, int arraySizeY, char info[]) 
{  
	int i;
	float** theArray;  
	if ((theArray = (float**) calloc(arraySizeX,sizeof(float*)))== NULL) {
		printf("error in allocating pointer arrays of %s\n",info);
	}

	for (i=0; i<arraySizeX; i++)
	{
		if ((*(theArray+i) = (float*) calloc(arraySizeY,sizeof(float)))== NULL) {
			printf("error in allocating arrays of %s\n",info);
		};  
		//for (j=0; j<arraySizeY; j++) theArray[i][j]=0.0;
	}
	return theArray;  
} 




#endif
 
