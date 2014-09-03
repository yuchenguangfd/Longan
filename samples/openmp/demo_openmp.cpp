/*
 * demo_openmp.cpp
 * Created on: Sep 3, 2014
 * Author: chenguangyu
 */

#include <omp.h>
#include <vector>
#include <cstdio>
#include <cassert>

using namespace std;

void HelloOpenMP() {
    #pragma omp parallel
    {
        printf("[%d] Hello, ", omp_get_thread_num());
        printf("[%d] World!\n", omp_get_thread_num());
    }
}

void ParallelCountPrimes() {
    int idx;
    vector<int> primes;
    #pragma omp parallel for private(idx) schedule(dynamic)
    for (idx = 0; idx < 10000000; ++idx) {
        bool isPrime = true;
        for (int i = 2; i * i < idx; ++i)
            if (idx % i == 0) {
                isPrime = false;
                break;
            }
        // critical region, only one thread could access
        #pragma omp critical
        {
        if (isPrime) primes.push_back(idx);
        }
    }
    assert(primes.size() == 665027);
}

int main(int argc, char **argv) {
    omp_set_num_threads(8);
    HelloOpenMP();
    ParallelCountPrimes();
    return 0;
}
