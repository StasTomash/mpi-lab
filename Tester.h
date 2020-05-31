//
// Created by stomash-ua on 5/31/20.
//

#ifndef PARALLEL_TESTER_H
#define PARALLEL_TESTER_H

#include "Solver.h"
#include <string>
#include <vector>
#include <cstdio>
#include <chrono>

class Tester {
private:
    static void GenTestCase(int n, int m, int maxVal, std::vector<std::vector<int>> &table) {
        table = std::vector<std::vector<int> >(n, std::vector<int>(m, 0));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                table[i][j] = rand() % maxVal;
            }
        }
    }
public:
    static void Test(int n, int m, int maxVal, Solver solver) {
        printf("Starting test for %d x %d\n", n, m);
        std::vector<std::vector<int> > table;
        GenTestCase(n, m, maxVal, table);
        printf("Generated case\n");
        auto start = std::chrono::high_resolution_clock::now();
        long long resStraight = solver.Solve(table);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        printf("Non-parallel result is %lld in %d ms\n", resStraight, (int)duration.count());

        start = std::chrono::high_resolution_clock::now();
        long long resParallel = solver.SolveParallel(table);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        printf("Parallel result is %lld in %d ms\n", resStraight, (int)duration.count());

    }
};

#endif //PARALLEL_TESTER_H
