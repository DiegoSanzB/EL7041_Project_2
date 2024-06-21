#include <data_manager.hpp>
#include <sequence_generator.hpp>

using namespace std;

int main() {
    // Define constants
    const vector<int> mod_complexity = {4, 16};
    const vector<string> mod_complexity_str = {"QPSK", "16QAM"};
    // Create a vector of snr values
    vector<int> snr(34);
    for (int i = 0; i < snr.size(); i++) {
        snr[i] = i - 2;
    }
    vector<int> snr_plot = {-5, 0, 10, 30};
    snr[33] = -5;
    // Execution
    for (int i = 0; i < mod_complexity.size(); i++) {
        bool snr_plot_flag = false;
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
        for (int j = 0; j < snr.size(); j++) {
            vector<complex<double>> noisy_data = add_noise(modulated_seq, mod_complexity[i], snr[j]);
            // Separate real and imaginary parts
            vector<double> noisy_real, noisy_imaginary;
            tie(noisy_real, noisy_imaginary) = separate_real_imaginary(noisy_data);
            vector<vector<double>> noisy_data_vector = {noisy_real, noisy_imaginary};
            // Rename columns
            column_names = {
                "re_noise_" + to_string(snr[j]), 
                "im_noise_" + to_string(snr[j])};
            // Save data to csv if inside snr_plot
            if (find(snr_plot.begin(), snr_plot.end(), snr[j]) != snr_plot.end()) {
                if (!snr_plot_flag) {
                    write_to_csv(noisy_data_vector, column_names, mod_complexity_str[i] + "_snr.csv");
                    snr_plot_flag = true;
                } 
                else {
                    add_to_csv(noisy_data_vector, column_names, mod_complexity_str[i] + "_snr.csv");
                }
            }
            // if (j == 33) {
            //     // Print last snr vector
            //     for (int k = 0; k < noisy_data_vector.size(); k++) {
            //         for (int l = 0; l < noisy_data_vector[k].size(); l++) {
            //             cout << noisy_data_vector[k][l] << " ";
            //         }
            //         cout << endl;
            //     }

            // }
        
        }
        
    }
    return 0;
}

// -- END OF FILE -- // 