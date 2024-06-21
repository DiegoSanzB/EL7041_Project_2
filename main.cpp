#include <data_manager.hpp>
#include <sequence_generator.hpp>

using namespace std;
const vector<int> mod_complexity = {4, 16};
const vector<string> mod_complexity_str = {"QPSK", "16QAM"};
int main() {
    for (int i = 0; i < mod_complexity.size(); i++) {
        // Generate sequence
        vector<int> seq = generate_sequence_bins(mod_complexity[i]);
        // Modulate sequence
        vector<complex<double>> modulated_seq = modulate_sequence(seq, mod_complexity[i]);
        // Separate real and imaginary parts
        vector<double> real, imaginary;
        tie(real, imaginary) = separate_real_imaginary(modulated_seq);
        vector<vector<double>> data = {real, imaginary};
        vector<string> column_names = {"re", "im"};
        // Save data to csv
        write_to_csv(data, column_names, mod_complexity_str[i] + ".csv");
        // Add noise
        
    }
    return 0;
}

// -- END OF FILE -- // 