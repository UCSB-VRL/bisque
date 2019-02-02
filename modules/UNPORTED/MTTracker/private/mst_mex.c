/*
 * ==============================================================
 * mst_mex.c The mex interface to the matlab bgl wrapper.
 *
 * David Gleich
 * 20 April 2006
 * =============================================================
 */


/*
 * 20 April 2006
 * Initial version
 *
 * 10 November 2006:
 * Alterted the code to produce the correct number of edges 
 * in the minimum spanning tree by resizing the arrays.  The previous code
 * had a bug where graphs with disconnected components would have errors 
 * in the edges in the minimum spanning trees.
 *
 * 19 February 2007
 * Updated to use Matlab 2006b sparse matrix interface
 *
 * 1 March 2007
 * Updated to use expand macros
 *
 * 15 April 2007
 * Fixed error with mallocing the maximum amount of memory by checking
 * for an empty or singleton graph and simply returning that.
 */

#include "mex.h"

#if MX_API_VER < 0x07030000
typedef int mwIndex;
typedef int mwSize;
#endif /* MX_API_VER */

#include "matlab_bgl.h"
#include "expand_macros.h"
#include "common_functions.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/*
 * The mex function runs a MST problem.
 */
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{ 
    mwIndex mrows, ncols;
    
    mwIndex n,nz;
    
    /* sparse matrix */
    mwIndex *ia, *ja;
    double *a;
    
    /* true if this function is reweighted */
    int reweighted = 0;
       
    /* output data */
    double *it, *jt, *vt;
    
    /* mst type string */
    char *algname;
    
    mwIndex nedges;
    
    /* 
     * The current calling pattern is
     * mst_mex(A,algname,reweight)
     * so visitor is an optional paramter.
     * h is either a vector with the heuristic specified for all nodes or 
     *   a function handle that computes the heuristic on the fly
     * reweight is either a string of a length nnz vector
     *
     * if reweight is a length nnz vector, then we use that as the values
     * for the matrix, if its a string, then we use the values from the 
     * the matrix.
     */
    
    const mxArray* arg_matrix;
    const mxArray* arg_algname;
    const mxArray* arg_reweight;
    
    if (nrhs != 3) 
    {
        mexErrMsgTxt("3 inputs required.");
    }
    
    arg_matrix = prhs[0];
    arg_algname = prhs[1];
    arg_reweight = prhs[2];
    
    algname = load_string_arg(arg_algname, 1);
    
    /* First test if they are going to reweight or not */
    if (!mxIsChar(arg_reweight)) 
    {
        reweighted = 1;
    }

    /* The first input must be a sparse matrix. */
    mrows = mxGetM(arg_matrix);
    ncols = mxGetN(arg_matrix);
    if (!reweighted && (mrows != ncols ||
        !mxIsSparse(arg_matrix) ||
        !mxIsDouble(arg_matrix) || 
        mxIsComplex(arg_matrix))) 
    {
        mexErrMsgTxt("A non-reweighted input matrix must be a noncomplex square sparse matrix.");
    }
    else if (reweighted && (mrows != ncols ||
        !mxIsSparse(arg_matrix)))
    {
        mexErrMsgTxt("A reweighted input matrix must be a square sparse matrix.");
    }
    
    n = mrows;
         
    
    /* Get the sparse matrix */
    
    /* recall that we've transposed the matrix */
    ja = mxGetIr(arg_matrix);
    ia = mxGetJc(arg_matrix);
    
    nz = ia[n];
    
    if (!reweighted) 
    {
        a = mxGetPr(arg_matrix);
    }
    else
    {
        /* check the reweighting array to make sure it is acceptable */
        if (mxGetNumberOfElements(arg_reweight) < nz) 
            mexErrMsgTxt("The reweight array must have length at least nnz(A)");
        if (!mxIsDouble(arg_reweight))
            mexErrMsgTxt("The reweight array must be of type double.");
        
        a = mxGetPr(arg_reweight);
    }
    
    /* Get the algorithm type */
    
    if (n > 1)
    {
        plhs[0] = mxCreateDoubleMatrix(n-1,1,mxREAL);
        plhs[1] = mxCreateDoubleMatrix(n-1,1,mxREAL);
        plhs[2] = mxCreateDoubleMatrix(n-1,1,mxREAL);
    }
    else
    {
        /* 
         * in this case, we have no output so we'll
         * just return immediately.
         */
        n = 1;
        plhs[0] = mxCreateDoubleMatrix(n-1,1,mxREAL);
        plhs[1] = mxCreateDoubleMatrix(n-1,1,mxREAL);
        plhs[2] = mxCreateDoubleMatrix(n-1,1,mxREAL);
        return;
    }
    /* create the output vectors */    
    it = mxGetPr(plhs[0]);
    jt = mxGetPr(plhs[1]);
    vt = mxGetPr(plhs[2]);
    
    #ifdef _DEBUG
    mexPrintf("mst_alg_%s...",algname);
    #endif 
    
    nedges = 0;
    if (strcmp(algname, "prim") == 0)
    {
        prim_mst(n, ja, ia, a,
            (mwIndex*)it,(mwIndex*)jt, vt, &nedges);
    }
    else if (strcmp(algname, "kruskal") == 0)
    {
        kruskal_mst(n, ja, ia, a,
            (mwIndex*)it,(mwIndex*)jt, vt, &nedges);
    }
    else
    {
        mexErrMsgTxt("Unknown algname.");
    }
    #ifdef _DEBUG
    mexPrintf("done\n");
    #endif 
    
    expand_index_to_double((mwIndex*)it, it, n-1, 1.0);
    expand_index_to_double((mwIndex*)jt, jt, n-1, 1.0);
    
    /* resize the output to the correct number of edges */
    mxSetM(plhs[0], nedges);
    mxSetM(plhs[1], nedges);
    mxSetM(plhs[2], nedges);
    
    #ifdef _DEBUG
    mexPrintf("return\n");
    #endif 
}

