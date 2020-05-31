#ifndef PARALLEL_SOLVER_H
#define PARALLEL_SOLVER_H

#include <vector>
#include <cstdio>
#include "omp.h"

class Solver {
public:
    long long Solve(const std::vector<std::vector<int> >& table) {
        int n = table.size();
        int m = table[0].size();
        int** dp = new int*[2];
        dp[0] = new int[n];
        dp[1] = new int[n];
        for (int num = 0; num < 2*n - 1; num++) {
            int curActive = (num + 1) % 2;
            int curPrev = num % 2;
            int start = std::max(0, num - (n - 1));
            int end = std::min(num, n - 1);
            for (int i = start; i <= end; i++) {
            dp[curActive][i] = std::max(dp[curPrev][i], (i > 0 ? dp[curPrev][i-1] : 0)) +
                               table[i][num-i];
            }
        }
        return dp[((2*n-1)%2)][n-1];
    }
    long long SolveParallel(std::vector<std::vector<int> > table) {
        int  n = table.size();
        int** dp = new int*[2];
        dp[0] = new int[n];
        dp[1] = new int[n];
        for (int num = 0; num < 2*n - 1; num++) {
            int curActive = (num + 1) % 2;
            int curPrev = num % 2;
            int start = std::max(0, num - (n - 1));
            int end = std::min(num, n - 1);
#pragma omp parallel for schedule(static)
            for (int i = start; i <= end; i++) {
                dp[curActive][i] = std::max(dp[curPrev][i], (i > 0 ? dp[curPrev][i-1] : 0)) +
                    table[i][num-i];
            }
        }
        return dp[((2*n-1)%2)][n-1];
    }
};

#endif //PARALLEL_SOLVER_H
