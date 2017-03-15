#include <vector>
#include <cstdio>
#include <stdint.h>
#include <omp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Mrpt.h"
#include "Eigen/Core"

#include "common.h"

using namespace Eigen;

int main()
{
    int n_test;
    int n;
    int dim = 784;
    int n_trees = 256;
    int depth = 10;
    float sparsity = 0.035;
    std::vector<int> vote = {1,2,3,4,5,10};

    std::string path = "/home/hyvi/HYVI/git/MRPT_test/data/mnist";
    float *train_data = get_data((path + "/train.bin").c_str(), dim, &n);
    float *test_data = get_data((path + "/test.bin").c_str(), dim, &n_test);


    const Map<MatrixXf> *X = new Map<MatrixXf>(train_data, dim, n);

    std::cout << "Data read.\n";

    Mrpt index(X, n_trees, depth, sparsity);
    index.grow();

    std::cout << "Index grown\n";

    for (int ind = 0; Ks[ind] != -1; ++ind) {
        int k = Ks[ind];

        for (int i = 0; i < vote.size(); i++) {
            int votes = vote[i];
            if (votes > n_trees) continue;

            std::vector<double> times;
            std::vector<std::set<int>> idx;

            for (int i = 0; i < n_test; ++i) {
                std::vector<int> result(k);
                double start = omp_get_wtime();
                int nn = index.query(Map<VectorXf>(&test_data[i * dim], dim), k, votes, 0, &result[0]);

                double end = omp_get_wtime();
                times.push_back(end - start);
                idx.push_back(std::set<int>(result.begin(), result.end()));
            }

            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << " ";
            results(k, times, idx, (std::string(path) + "/truth_" + std::to_string(k)).c_str());
        }
    }

    delete[] train_data;
    delete[] test_data;
    delete X;

    return 0;
}
