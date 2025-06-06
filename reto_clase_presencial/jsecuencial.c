#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 4
#define TOL 0.0001
#define MAX_IT 1000

int main() {
    double A[N][N] = {{10, -1, 2, 0},
                      {-1, 11, -1, 3},
                      {2, -1, 10, -1},
                      {0, 3, -1, 8}};
    double b[N] = {6, 25, -11, 15};
    double x_old[N] = {0};
    double x_new[N] = {0};

    int it;
    for (it = 0; it < MAX_IT; it++) {
        for (int i = 0; i < N; i++) {
            double sum = b[i];
            for (int j = 0; j < N; j++) {
                if (j != i)
                    sum -= A[i][j] * x_old[j];
            }
            x_new[i] = sum / A[i][i];
        }

        double error = 0.0;
        for (int i = 0; i < N; i++) {
            error += fabs(x_new[i] - x_old[i]);
            x_old[i] = x_new[i];
        }

        if (error < TOL) break;
    }

    printf("Solución en %d iteraciones:\n", it);
    for (int i = 0; i < N; i++) {
        printf("x[%d] = %lf\n", i, x_new[i]);
    }

    return 0;
}
