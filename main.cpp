#include "Solver.h"
#include "Tester.h"
#include <string>
#include <fstream>
#include <fcntl.h>
#include <omp.h>

int main() {
    srand(10);
    omp_set_num_threads(2);
    printf("%d\n", omp_get_num_threads());
    Tester::Test(10000, 10000, 1000, Solver());
    Tester::Test(20000, 20000, 1000, Solver());
    Tester::Test(30000, 30000, 1000, Solver());
    Tester::Test(40000, 40000, 1000, Solver());

    return 0;
}
