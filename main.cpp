#include <data_manager.hpp>
#include <sequence_generator.hpp>

using namespace std;

int main() {
    const int complexity = 4;
    vector<int> seq = generate_sequence_bins(complexity);
    for (int i = 0; i < seq.size(); i++) {
        cout << bitset<2>(seq[i]) << " ";
        // cout << seq[i] << " ";
    }
    cout << endl;
    vector<complex<double>> modulated_seq = modulate_sequence(seq, complexity);
    for (int i = 0; i < modulated_seq.size(); i++) {
        cout << modulated_seq[i] << " ";
    }
    cout << endl;
    // Separate real and imaginary parts
    vector<double> real, imaginary;
    tie(real, imaginary) = separate_real_imaginary(modulated_seq);
    // Write to CSV
    vector<vector<double>> data = {real, imaginary};
    vector<string> column_names = {"Real", "Imaginary"};
    write_to_csv(data, column_names, "test.csv");
}

// -- END OF FILE -- // 