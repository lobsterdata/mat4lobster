/*=================================================================
 *
 * FINDPRECEDING Find the indice of an increasing series that immediately 
 * precede another increasing series. 
 *
 * The calling syntax is:
 *
 *		[idx] = findPreceding(series_1,series_2)
 *
 *
 *  idx -- the length is equal to this of series_1. Value is the idx of 
 *  series_2(idx) < series_1 and series_2(idx+1)>series_1  
 *
 * This is a MEX-file for MATLAB. 
 *
 * Copyright 2012 Ruihong Huang (ruihong.huang@wiwi.hu-berlin.de)
 * Chair of Economics, Humboldt Universitaet zu Berlin
 * $Revision: 1.0.0 $  $Date: 2012/11/12$ 
 *
 *=================================================================*/

#include "mex.h"
#include <math.h>
#include <stdlib.h>

/* Input Arguments */

#define	TIME_1	prhs[0]
#define	TIME_2	prhs[1]

/* Output Arguments */

#define	IDX_OUT	plhs[0]

/* real working C function*/
int findPreceding(double* s1, double* s2, int n1, int n2, double *ix){
  int i=0;
  int j=0;

  while (i<n1 && j<n2){
    if (*(s1+i) > *(s2+j)){
      j++;
      continue;
    }
    if (j==0)
        ix[i]=1;
    else{
        ix[i] = j;
        j--;
    }
      i++;
  }
  if (i<n1 && j==n2){
    for (;i<n1;i++)
      ix[i] = n2;
  }
  return 0;

}

/* mex function to matlab*/
void mexFunction( int nlhs, mxArray *plhs[],
int nrhs, const mxArray*prhs[] )
{
  int N1,N2;
  double* time1, *time2,*idx;
  int res;
  int i=0;
  if (nrhs != 2) {
        mexErrMsgTxt("Two input arguments required.");
    } else if (nlhs > 1) {
        mexErrMsgTxt("Too many output arguments.");
    }
     N1= mxGetM(TIME_1);
     N2= mxGetM(TIME_2);
    IDX_OUT = mxCreateNumericMatrix(N1, 1, mxDOUBLE_CLASS,mxREAL);
    idx = mxGetPr(IDX_OUT);
    time1 = mxGetPr(TIME_1);
    time2 = mxGetPr(TIME_2);
    res=findPreceding(time1,time2,N1,N2,idx);
    
    return;
}

