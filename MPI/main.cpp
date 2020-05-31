#include <iostream>
#include <ctime>
#include <algorithm>
#include <mpi.h>
#include <vector>

#define MASTER 0
#define MAXN 500000

void parseArguments(int argc, char** argv, int& sz){
    sz = 10000;
    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], "-size") == 0) {
            sz = atoi(argv[i+1]);
        }
    }
}

void GenTestCase(int n, int m, int maxVal, int** table, int** diagonal) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            table[i][j] = rand() % maxVal;
            diagonal[i+j][i] = table[i][j];
        }
    }
}

long long SolveStupid(int n, int** diagonal) {
    int dp[2][MAXN] = {};
    for (int num = 0; num < 2*n - 1; num++) {
        int curActive = (num + 1) % 2;
        int curPrev = num % 2;
        int start = std::max(0, num - (n - 1));
        int end = std::min(num, n - 1);
        for (int i = start; i <= end; i++) {
            dp[curActive][i] = std::max(dp[curPrev][i], (i > 0 ? dp[curPrev][i-1] : 0)) +
                               diagonal[num][i];
        }
    }
    return dp[((2*n-1)%2)][n-1];
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int n;
    parseArguments(argc, argv, n);
    srand(10);
    int numProcesses, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    int blockLen = n / numProcesses;
    while (n % blockLen) {
        blockLen++;
    }

    int** table = new int*[n];
    for (int i = 0; i < n; i++ ) {
        table[i] = new int[n];
    }
    int** diagonal = new int*[2*n-1];
    for (int i = 0; i < 2*n - 1; i++ ) {
        diagonal[i] = new int[n];
    }
    int** diagonalPerProcess = new int*[2*n-1];
    for (int i = 0; i < 2*n - 1; i++ ) {
        diagonalPerProcess[i] = new int[blockLen];
    }

    double startTime, finishTime;

    if (rank == MASTER) {
        printf("Starting test for %d x %d\n", n, n);
        GenTestCase(n, n, 1000, table, diagonal);
        printf("Generated case\n");
        startTime = MPI_Wtime();
    }
    for (int num = 0; num < 2*n - 1; num++) {
        MPI_Scatter(diagonal[num], blockLen, MPI_INT, diagonalPerProcess[num], blockLen, MPI_INT,
                    MASTER, MPI_COMM_WORLD);
    }
    int dp[2][MAXN] = {};
    for (int num = 0; num < 2*n - 1; num++) {
        int curActive = (num + 1) % 2;
        int curPrev = num % 2;
        int start = std::max(0, num - (n - 1));
        int end = std::min(num, n - 1);
        int blockStart = blockLen * rank;
        int blockEnd = blockLen * (rank + 1) - 1;
        int l = std::max(blockStart, start);
        int r = std::min(blockEnd, end);
        for (int i = l; i <= r; i++) {
            dp[curActive][i] = std::max(dp[curPrev][i], (i > 0 ? dp[curPrev][i-1] : 0)) +
                               diagonalPerProcess[num][i - blockStart];
        }
        if (rank < numProcesses - 1) {
            MPI_Send(&dp[curActive][blockEnd], 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
        }
        if (rank > 0) {
            MPI_Recv(&dp[curActive][blockStart-1], 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
        }
    }

    if (rank == numProcesses - 1 && numProcesses > 1) {
        MPI_Send(&dp[((2*n)-1)%2][n-1], 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
    }

    if (rank == MASTER) {
        long long ans;
        if (numProcesses != 1) {
            MPI_Recv(&ans, 1, MPI_INT, numProcesses - 1, 1, MPI_COMM_WORLD, &status);
        } else {
            ans = dp[((2*n)-1)%2][n-1];
        }
        finishTime = MPI_Wtime();
        printf("Parallel result is %lld in %d ms\n", ans, (int)((finishTime - startTime)*1000));
        startTime = MPI_Wtime();
        ans = SolveStupid(n, diagonal);
        finishTime = MPI_Wtime();
        printf("Non-parallel result is %lld in %d ms\n", ans, (int)((finishTime - startTime)*1000));
    }
    MPI_Finalize();
}
