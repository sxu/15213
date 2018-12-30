/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <assert.h>
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans(int M, int N, int A[N][M], int B[M][N]);
void transpose_32x32(int N, int A[N][N], int B[N][N]);
void transpose_64x64(int N, int A[N][N], int B[N][N]);
void transpose_61x67(int M, int N, int A[N][N], int B[N][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  if (M == 32 && N == 32) {
    transpose_32x32(32, A, B);
  } else if (M == 64 && N == 64) {
    transpose_64x64(64, A, B);
  } else if (M == 61 && N == 67) {
    transpose_61x67(61, 67, A, B);
  } else {
    trans(M, N, A, B);
  }
}

void transpose_32x32(int N, int A[N][N], int B[N][N]) {
  int i, j, row, col, blk;
  assert(N == 32);
  for (row = 0; row < N; row += 8) {
    for (col = 0; col < N; col += 8) {
      if (row == col) {
        continue;
      }
      for (i = row; i < row + 8; i++) {
        for (j = col; j < col + 8; j++) {
          B[j][i] = A[i][j];
        }
      }
    }
  }
  for (blk = 8; blk < N; blk += 8) {
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
        B[j][i] = A[blk + i][blk + j];
      }
    }
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
        B[blk + i][blk + j] = B[i][j];
      }
    }
  }
  blk = 0;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      B[blk + j][blk + i] = A[blk + i][blk + j];
    }
  }
}

void transpose_64x64(int N, int A[N][N], int B[N][N]) {
  int i, j, row, col, blk;
  assert(N == 64);
  // Transpose non-diagonal 8x8 blocks.
  for (row = 0; row < N; row += 8) {
    for (col = 0; col < N; col += 8) {
      if (row == col) {
        continue;
      }
      // Depending on which column of B we are working with, use the first
      // diagonal block or the last diagonal block as buffer.
      if (row == 0) {
        blk = N - 8;
      } else {
        blk = 0;
      }
      for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
          B[col + j][row + i] = A[row + i][col + j];
        }
        for (j = 4; j < 8; j++) {
          // Writes to the buffer to avoid conflict miss.
          B[blk + j][blk + i] = A[row + i][col + j];
        }
      }
      for (i = 4; i < 8; i++) {
        for (j = 0; j < 8; j++) {
          // Transfer back from buffer.
          B[col + i][row + j] = B[blk + i][blk + j];
        }
      }
    }
  }
  // Use the first two diagonal blocks as buffers for the rest.
  for (blk = 16; blk < N; blk += 8) {
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 4; j++) {
        B[j][i] = A[blk + i][blk + j];
      }
      for (j = 4; j < 8; j++) {
        B[j + 8][i + 8] = A[blk + i][blk + j];
      }
    }
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 8; j++) {
        B[blk + i][blk + j] = B[i][j];
      }
    }
    for (i = 4; i < 8; i++) {
      for (j = 0; j < 8; j++) {
        B[blk + i][blk + j] = B[i + 8][j + 8];
      }
    }
  }
  // Use first diagonal block as buffer for the second.
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 4; j++) {
      B[j][i] = A[8 + i][8 + j];
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 4; j < 8; j++) {
      B[j][i] = A[8 + i][8 + j];
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      B[8 + i][8 + j] = B[i][j];
    }
  }
  // Transpose the first block.
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 4; j++) {
      B[j][i] = A[i][j];
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 4; j < 8; j++) {
      B[j][i] = A[i][j];
    }
  }
}

void transpose_61x67(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, row, col;
  assert(M == 61 && N == 67);
  for (col = 0; col < M - M % 8; col += 8) {
    for (row = 0; row < N - N % 8; row += 8) {
      for (i = row; i < row + 8; i++) {
        for (j = col; j < col + 8; j++) {
          B[j][i] = A[i][j];
        }
      }
    }
  }
  for (col = 0; col < M - M % 8; col += 8) {
    for (i = N - N % 8; i < N; i++) {
      for (j = col; j < col + 8; j++) {
        B[j][i] = A[i][j];
      }
    }
  }
  for (row = 0; row < N - N % 8; row += 8) {
    for (i = row; i < row + 8; i++) {
      for (j = M - M % 8; j < M; j++) {
        B[j][i] = A[i][j];
      }
    }
  }
  for (i = N - N % 8; i < N; i++) {
    for (j = M - M % 8; j < M; j++) {
      B[j][i] = A[i][j];
    }
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
void trans(int M, int N, int A[N][M], int B[M][N]) {
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
void registerFunctions() {
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
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
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
