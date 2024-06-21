// Include the header file
#include "sequence_generator.hpp"

// Generate a random sequence of numbers 
vector<int> generate_sequence_bins(int mod_complexity, int n) {
    cout << "Generating sequence of complexity " << mod_complexity << endl;
    // Get bit per symbol
    int bps = log2(mod_complexity);
    // determine max value for each simbol
    int max = pow(2, bps) - 1;
    // And how many to create
    int size = MAX_SEQUENCE_LENGTH/bps;
    cout << "Sequence of size " << size << " with max value " << max << " Which in binary looks like " << bitset<2>(max) << endl;
    // Create sequence and add random numbers
    vector<int> sequence(size);
    for (int i = 0; i < size; i++) {
        sequence[i] = rand() % (max + 1);
    }
    return sequence;
}
// Modulate data into complex numbers
vector<complex<double>> modulate_sequence(const vector<int>& sequence, int mod_complexity) {
    cout << "Modulating sequence" << endl;
    // Get modulation map
    const unordered_map<int, complex<double>>* modulation;
    if (mod_complexity == 4) {
        modulation = &QPSK;
    } else if (mod_complexity == 16) {
        modulation = &QAM16;
    } else {
        cerr << "Error: Modulation complexity not supported." << endl;
        return {};
    }
    // Get size of sequence
    int size = sequence.size();
    // Create array to store modulated data
    vector<complex<double>> modulated_sequence(size);
    // Get modulations
    for (int i = 0; i < size; i++) {
        modulated_sequence[i] = modulation->at(sequence[i]);
        // Normalize
        if (mod_complexity == 4) {
            modulated_sequence[i] /= sqrt(2);
        } else if (mod_complexity == 16) {
            modulated_sequence[i] /= sqrt(10);
        }
    }

    return modulated_sequence;
}
// Tuple that separates data into real and imaginary parts
tuple<vector<double>, vector<double>> separate_real_imaginary(const vector<complex<double>>& data) {
    cout << "Separating real and imaginary parts" << endl;
    // Get size of data
    int size = data.size();
    // Create arrays to store real and imaginary parts
    vector<double> real_part(size);
    vector<double> imaginary_part(size);
    // Separate data
    for (int i = 0; i < size; i++) {
        real_part[i] = data[i].real();
        imaginary_part[i] = data[i].imag();
    }
    return make_tuple(real_part, imaginary_part);
} 
// Add noise
// vector<complex<double>> add_noise(const vector<complex<double>>& data, double snr) {

// }

