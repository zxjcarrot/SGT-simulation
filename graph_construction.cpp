#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include <bitset>
#include <fstream>
#include <utility>

#include "Random.h"
#include "Zipf.h"


const int N = 1000; // number of transactions in a batch

const int N_TUPLES = 1600000; // # tuples in the database
const int N_OPS = 10; // # operations per transaction

const int WAR = 0;
const int WAW = 1;
const int RAW = 2;
const int NONE = -1;
const int READ = 0;
const int WRITE = 1;
int main()
{
    std::ofstream ofile;
    double theta = 0;
    Random r;
    std::cin >> theta; // Input skew factor
    ofile.open (std::to_string(theta) + "_" +  std::to_string(N) + ".txt", std::ios::trunc);
    Zipf zipf;
    zipf.init(N_TUPLES, theta);
	std::vector<std::vector<std::pair<int, int>>> tuple_last_access_by(N_TUPLES, std::vector<std::pair<int,int>>());
    std::vector<int> txn_progress(N, 0);
    std::vector<int> txns(N);
    std::vector<bool> txn_aborted(N);
    std::vector<std::vector<int>> txn_tuples_accessed(N);
    for (int i = 0; i < N; ++i) {
        txns[i] = i;
    }
    const int threshold = 100;
    const int concurrency = 32;
	for (int i = 0; i < N_OPS; ++i) {
        std::random_shuffle(txns.begin(), txns.end());
        for (int j = 0; j < N; ++j) {
            int tid = txns[j];
            if (txn_aborted[tid]) {
                continue;
            }
            int tuple_idx = zipf.value(r.next_double());
            bool good = true;
            do {
                tuple_idx = zipf.value(r.next_double());
                good = true;
                for (int j = 0; j < txn_tuples_accessed[tid].size(); ++j) {
                    if (txn_tuples_accessed[tid][j] == tuple_idx) {
                        good = false;
                        break;
                    }
                }
            }while (good == false);

            //int my_op = READ;
            //int my_op = WRITE;
            //int my_op = r.next() % 10 < 8 ? 0 : 1; // 80% reads, 20% writes.
            int my_op = r.next() % 10 < 8 ? 1 : 0; // 80% writes, 20% reads.
            //int my_op = r.next() % 2;
            int num_conflicts = 0;

            if (tuple_last_access_by[tuple_idx].empty() == false) {
                if (my_op == READ) {
                    int k;
                    for (k = tuple_last_access_by[tuple_idx].size() - 1; k >= 0; --k) {
                        if (k >= 0 && 
                        tuple_last_access_by[tuple_idx][k].first != tid &&
                        tuple_last_access_by[tuple_idx][k].second == WRITE) { // It is ok to read your own write
                            ++num_conflicts;
                        }
                    }
                } else { // WRITE
                    int k;
                    for (k = tuple_last_access_by[tuple_idx].size() - 1; k >= 0; --k) {
                        if (tuple_last_access_by[tuple_idx][k].first != tid) { // It is ok to overwrite your own read
                            if (tuple_last_access_by[tuple_idx][k].second == READ) {
                                ++num_conflicts;
                            } else {
                                ++num_conflicts;
                            }
                        }
                    }
                }
            }
            if (tuple_last_access_by[tuple_idx].size() > threshold) {
                txn_aborted[tid] = true;
                continue;
            }
            if (tuple_last_access_by[tuple_idx].empty() == false) {
                if (my_op == READ) {
                    int k;
                    for (k = tuple_last_access_by[tuple_idx].size() - 1; k >= 0; --k) {
                        if (k >= 0 && 
                        tuple_last_access_by[tuple_idx][k].first != tid &&
                        tuple_last_access_by[tuple_idx][k].second == WRITE) { // It is ok to read your own write
                            ofile << tid << " " << tuple_last_access_by[tuple_idx][k].first << " " << RAW << std::endl;
                        }
                    }
                } else { // WRITE
                    int k;
                    for (k = tuple_last_access_by[tuple_idx].size() - 1; k >= 0; --k) {
                        if (tuple_last_access_by[tuple_idx][k].first != tid) { // It is ok to overwrite your own read
                            if (tuple_last_access_by[tuple_idx][k].second == READ) {
                                ofile << tid << " " << tuple_last_access_by[tuple_idx][k].first << " " << WAR << std::endl;
                            } else {
                                ofile << tid << " " << tuple_last_access_by[tuple_idx][k].first << " " << WAW << std::endl;
                            }
                        }
                    }
                }
            }
            tuple_last_access_by[tuple_idx].push_back(std::make_pair(tid, my_op));
            txn_tuples_accessed[tid].push_back(tuple_idx);
        }
    }
    ofile << "-1" << std::endl;
    std::vector<int> tuple_history_length(N_TUPLES);
    for (size_t i = 0; i < N_TUPLES; ++i) {
        tuple_history_length[i] = tuple_last_access_by[i].size();
    }
    int num_aborted = 0;
    for (int j = 0; j < N; ++j) {
        int tid = txns[j];
        if (txn_aborted[tid]) {
            num_aborted++;
        }
    }
    ofile << num_aborted << std::endl;
    for (int j = 0; j < N; ++j) {
        int tid = txns[j];
        if (txn_aborted[tid]) {
            ofile << tid << std::endl;
        }
    }
    std::cout << "# Aborted txns " << num_aborted << std::endl;
    std::sort(tuple_history_length.begin(), tuple_history_length.end(),std::greater<int>());
    for (size_t i = 0; i < 30; ++i) {
        std::cout << "i=" << i << ", freq=" << tuple_history_length[i] << std::endl;
    }
	return 0;
}