/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 * 
 * Maor Shutman.
 */ 
#include <stdio.h>
#include "cachelab.h"

#include <stdlib.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void trans_32(int M, int N, int A[N][M], int B[M][N]) {
    int sz = 8;
    int n = 4;

    for (int q = 0; q < n; ++q) {
        for (int p = 0; p < n; ++p) {
            if (q != p) {
                for (int i = sz*q; i < (sz*(q+1)); i++) {
                    for (int j = sz*p; j < (sz*(p+1)); j++) {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }

    int diag, r, div_8;
    for (int q = 0; q < n; ++q) {
        for (int i = sz*q; i < (sz*(q+1)); i++) {
            div_8 = ((i+1) % 8) == 0;
            for (int j = sz*q; j < (sz*(q+1)); j++) {
                if ((i == j) && (!div_8)) {
                    diag = A[i][i];
                    r = i;
                } else {
                    B[j][i] = A[i][j];
                }
                
                if ((i == (r + 1)) && (j == r)) {
                    B[r][r] = diag;
                }
            }
        }
    }
}

void trans_64_submat(int M, int N, int A[N][M], int B[M][N], int i0, int j0, 
    int sz_w, int sz_h, int n_w, int n_h) {

    for (int q = 0; q < n_h; ++q) {
        for (int p = 0; p < n_w; ++p) {
            for (int i = i0+sz_h*q; i < i0+(sz_h*(q+1)); i++) {
                for (int j = j0+sz_w*p; j < j0+(sz_w*(p+1)); j++) {
                    B[j][i] = A[i][j];
                }
            }
        
        }
    }
}

void trans_block(int M, int N, int A[N][M], int B[M][N], int i0, int j0,
    int itmp, int jtmp)
{
    /* Tranpose a 8x8 block of the form
      
       1 2
       3 4

       by storing it temporarily in B.

    */

    // Store 1.
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            B[itmp + j][jtmp + i] = A[i0 + i][j0 + j];
        }
    }

    // Store 2.
    for (int i = 0; i < 4; ++i) {
        for (int j = 4; j < 8; ++j) {
            B[itmp + j - 4][jtmp + i + 4] = A[i0 + i][j0 + j];
        }
    }

    // 4 final.
    for (int i = 4; i < 8; ++i) {
        for (int j = 4; j < 8; ++j) {
            B[j0 + j][i0 + i] = A[i0 + i][j0 + j];
        }
    }

    // 2 final.
    for (int i = 4; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            B[j0 + i][i0 + j] = B[itmp + i - 4][jtmp + j + 4];
        }
    }

    // Store 3.
    for (int i = 4; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            B[itmp + j][jtmp + i] = A[i0 + i][j0 + j];
        }
    }

    // 1 and 3 final.
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j) {
            B[j0 + i][i0 + j] = B[itmp + i][jtmp + j];
        }
    }    
}

void trans_64(int M, int N, int A[N][M], int B[M][N]) {
    int n = 8;

    for (int i = 0; i < (n-1); ++i) {
        for (int j = 0; j < (n-1); ++j) {
            if (i != j) {
                trans_block(M, N, A, B, 8*i, 8*j, (n-1)*8, (n-1)*8);
            }
        }
    }

    // Right column.
    for (int i = 1; i < (n-1); ++i) {
        trans_block(M, N, A, B, 8*i, 8*7, 0, 0);
    }    

    // Lower row.
    for (int i = 1; i < (n-1); ++i) {
        trans_block(M, N, A, B, 8*7, 8*i, 0, 0);
    }

    // Bottom left and top right corners.
    trans_block(M, N, A, B, 8*7, 0, 8, 8);
    trans_block(M, N, A, B, 0, 8*7, 8, 8);

    // Diagonal.
    for (int i = 0; i < (n-1); ++i) {
        trans_block(M, N, A, B, 8*i, 8*i, (n-1)*8, (n-1)*8); 
    }

    // Bottom right corner.
    trans_64_submat(M, N, A, B, 7*8, 7*8, 8, 8, 1, 1);
}

void trans_67x61(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, p, q, sz = 16;
    for (i = 0; i < N; i += sz) {
        for (j = 0; j < M; j += sz) {
            for (p = i; (p < i+sz) && (p < N); ++p) {
                for (q = j; (q < j+sz) && (q < M); ++q) {
                    B[q][p] = A[p][q];
                }    
            }
        }
    }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if ((N == 32) && (M == 32)) {
        trans_32(M, N, A, B);
    } 
    else if ((N == 64) && (M == 64)) {
        trans_64(M, N, A, B);
    }
    else if ((N == 67) && (M == 61)) {
        trans_67x61(M, N, A, B);
    } 
    else {
        abort();
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
