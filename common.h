#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <utility>
#include <memory>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

int Ks[] = {1, 10, 100, -1};

float *read_memory(const char *file, int n, int dim) {
    float *data = new float[n * dim];

    FILE *fd;
    if ((fd = fopen(file, "rb")) == NULL)
        return NULL;

    int read = fread(data, sizeof(float), n * dim, fd);
    if (read != n * dim)
        return NULL;

    fclose(fd);
    return data;
}

float *read_mmap(const char *file, int n, int dim) {
    FILE *fd;
    if ((fd = fopen(file, "rb")) == NULL)
        return NULL;

    float *data;

    if ((data = reinterpret_cast<float *> (
#ifdef MAP_POPULATE
            mmap(0, n * dim * sizeof(float), PROT_READ,
            MAP_SHARED | MAP_POPULATE, fileno(fd), 0))) == MAP_FAILED) {
#else
            mmap(0, n * dim * sizeof(float), PROT_READ,
            MAP_SHARED, fileno(fd), 0))) == MAP_FAILED) {
#endif
            return NULL;
    }

    fclose(fd);
    return data;
}


float *get_data(const char *file, int dim, int *n) {
    struct stat sb;
    stat(file, &sb);
    int N = sb.st_size / (sizeof(float) * dim);
    *n = N;

    float *data = new float[N * dim];

    FILE *fd;
    fd = fopen(file, "rb");
    fread(data, sizeof(float), N * dim, fd);
    fclose(fd);

    return data;
}


void results(int k, const vector<double> &times, const vector<set<int>> &idx, const char *truth, bool verbose = false) {
    double time;
    vector<set<int>> correct;

    ifstream fs(truth);
    for (int j = 0; fs >> time; ++j) {
        set<int> res;
        for (int i = 0; i < k; ++i) {
            int r;
            fs >> r;
            res.insert(r);
        }
        correct.push_back(res);
    }

    vector<pair<double, double>> results;

    double accuracy, total_time = 0, total_accuracy = 0;
    for (unsigned i = 0; i < times.size(); ++i) {
        set<int> intersect;
        set_intersection(correct[i].begin(), correct[i].end(), idx[i].begin(), idx[i].end(),
                         inserter(intersect, intersect.begin()));
        accuracy = intersect.size() / static_cast<double>(k);
        total_time += times[i];
        total_accuracy += accuracy;
        results.push_back(make_pair(times[i], accuracy));
    }

    double mean_accuracy = total_accuracy / results.size(), variance = 0;
    for (auto res : results)
        variance += (res.second - mean_accuracy) * (res.second - mean_accuracy);
    variance /= (results.size() - 1);

    cout << setprecision(5);
    if(verbose)
        cout << "accuracy: " << mean_accuracy << ", variance:  " << variance << ", query time: " << total_time << endl;
    else
        cout << mean_accuracy << " " << variance << " " << total_time << endl;


}
