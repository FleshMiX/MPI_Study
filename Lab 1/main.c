#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#include "InitialConditions.h"

double f(int t, int x) {
    return -(t+1)*x/1000.0;
}

int count(int rank, int M, int size) {
    return (M /size + ((rank < M%size) ? 1:0));
}

int main(int argc, char** argv) {

    int rank, size, i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int M = 10000, K = 10000;
    int cnt = count(rank, M, size), cnt_temp = 0, shift = 0;
    int *counts = (int*)calloc(size, sizeof(int));
    int *shifts = (int*)calloc(size, sizeof(int));

    double* data = (double*)calloc(K*M, sizeof(double)); //data - решение уравнения
    double t1, t2; //
    double tao = 0.00001, h = 0.00001;

    for (i = 0; i < size; i++) {
        cnt_temp = count(i, M, size);
        counts[i] = cnt_temp*K;
        if (i != 0)
            shifts[i] = shifts[i - 1] + counts[i];
    }

    //задаем начальные условия
    for (i = 0; i < K; i++)
        data[i] = psi(i);

    for (i = 0; i < M; i++)
        data[i*K] = phi(i);

    if (rank == 0) {

        t1 = MPI_Wtime();

        for (i = 1; i < K; i++) {

            for (j = 1; j < cnt; j++) {

                data[j*K + i] = data[j*K + i - 1] + tao * (f(i-1,j) - (data[j*K + i - 1] - data[(j-1)*K + i - 1])/h);

            }

        if (size != 1)
        MPI_Send(&data[(cnt-1)*K+i], 1, MPI_DOUBLE, 1, i, MPI_COMM_WORLD);

        }

        MPI_Allgatherv(&data[shift*K], cnt*K, MPI_DOUBLE, data, counts, shifts, MPI_DOUBLE, MPI_COMM_WORLD);


        //запись в файлы
        FILE *fp1, *fp2;
            char dataFile[] = "data.txt";
            char timeFile[] = "time.txt";
            fp1 = fopen(dataFile, "w+");
            fp2 = fopen(timeFile, "a+");

            t2 = MPI_Wtime();
            fprintf(fp2, "%f ", t2-t1);

            for (i = 0; i < K; i++)
                for (j = 0; j < M; j++)
                    fprintf(fp1, "%f ", data[i + j*K]);

            fclose(fp1);
            fclose(fp2);

            MPI_Finalize();

    }

    else {

        for (i = 0; i < rank; i++)
            shift += count(i, M, size);

        for (i = 1; i < K; i++) {

            j = cnt + shift - 1;
            data[j*K + i] = data[j*K + i - 1] + tao * (f(i-1,j) - (data[j*K + i - 1] - data[(j-1)*K + i - 1])/h);

            if (rank != size - 1)

                MPI_Send(&data[j*K+i], 1, MPI_DOUBLE, rank+1, i, MPI_COMM_WORLD);

            for (j = cnt + shift - 2; j > shift; j--)

                data[j*K + i] = data[j*K + i - 1] + tao * (f(i-1,j) - (data[j*K + i - 1] - data[(j-1)*K + i - 1])/h);

            MPI_Recv(&data[(shift-1)*K+i], 1, MPI_DOUBLE, rank-1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            data[shift*K + i] = data[shift*K + i - 1] + tao * (f(i-1,shift) - (data[shift*K + i - 1] - data[(shift-1)*K + i - 1])/h);

        }

        MPI_Allgatherv(&data[shift*K], cnt*K, MPI_DOUBLE, data, counts, shifts, MPI_DOUBLE, MPI_COMM_WORLD);

        MPI_Finalize();

    }

    free(data);

    return 0;
}
