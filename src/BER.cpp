#include "BER.hpp"

// computes BER between the transmitted data (ground_truth) and the received data (data)
double compute_BER(vector<int> ground_truth, vector<int> data, int modulation_order) {
    int error_count = 0;
    int count = ground_truth.size()*log2(modulation_order);

    if (ground_truth.size() != data.size()) {
        cout << "[BER] Missmatched data sizes!" << endl;
        return -1;
    }

    for (size_t i = 0; i < ground_truth.size(); i++) {
        int xored = ground_truth[i] ^ data[i];
        while (xored > 0) {
            error_count += xored & 1;
            xored >>= 1;
        }
    }

    // for (size_t i = 0; i < ground_truth.size(); i++) {
    //     if (ground_truth[i] != data[i])
    //         error_count++;
    // }
    // return ((double) error_count) / ((double) ground_truth.size());
    return ((double) error_count) / ((double) count);
}

// computes BER for each row given in the matrixes ground_truth and data
vector<double> compute_BER(vector<vector<int>> ground_truth, vector<vector<int>> data, int modulation_order) {
    vector<double> out;

    for (size_t i = 0; i < ground_truth.size(); i++) {
        out.push_back(compute_BER(ground_truth[i], data[i], modulation_order));
    }
    return out;
}

// -- END OF FILE -- //