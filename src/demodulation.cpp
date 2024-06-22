#include "demodulation.hpp"

vector<int> qamdemod_interface(vector<double> I, vector<double> Q, int modulationOrder) {
    vector<int> out;
    vector<vector<double>> I_matrix, Q_matrix;

    I_matrix.push_back(I);
    Q_matrix.push_back({});

    for (double value: Q) { // correcting for mirrored gray code
        Q_matrix[0].push_back(-1 * value);
    }

    vector<vector<int>> received_seq = QAMdemod(I_matrix, Q_matrix, modulationOrder);


    for (int i = 0; i < received_seq[0].size(); i++) {
        out.push_back(received_seq[0][i]);
    }

    return out;
}

vector<int> pskdemod(vector<complex<double>> data, int modulationOrder) {
    vector<int> out;

    double step = PI / modulationOrder;
    for (complex<double> value: data) {
        double angle = std::arg(value);

        for (int i = 0; i < QPSK.size(); i++) {
            complex<double> symbol = QPSK.at(i);
            double symbol_angle = std::arg(symbol);
            
            if ((symbol_angle - step < angle) && (angle <= symbol_angle + step )) {
                out.push_back(i);
            }
        }
    }

    return out;
}

vector<int> demod(vector<complex<double>> data, int modulationOrder, string modulationType) {
    if (modulationType == "QAM") {
        vector<double> I, Q;
        tie(I, Q) = separate_real_imaginary(data);
        return qamdemod_interface(I, Q, modulationOrder);
    }
    else if (modulationType == "PSK") {
        return pskdemod(data, modulationOrder);
    }
    else {
        cout << "Unrecognized Modulation Type" << endl;
        return {-1};
    }
}