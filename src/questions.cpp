#include<questions.hpp>


// Specific for this project
// Separates a complex data structure into a flat structure and saves it to a CSV file
void separate_and_save_complex(const vector<complex<double>>& data, string CSV_FILENAME, string identifier, bool create_new_file) {
    // Separate real and imaginary parts
    vector<double> real, imaginary;
    tie(real, imaginary) = separate_real_imaginary(data);
    vector<vector<double>> data2save = {real, imaginary};
    vector<string> column_names = {identifier + "_re", identifier + "_im"};
    if (create_new_file) {
        write_to_csv(data2save, column_names, CSV_FILENAME);
    } else {
        add_to_csv(data2save, column_names, CSV_FILENAME);
    }
}


void constellations(){
    // Iterate by modulation complexity
    // Create bool flags
    bool bool_plot = true, bool_perfect = true, bool_fft = true, bool_cubic = true;
    for (int i = 0; i < MOD_COMPLEXITY.size(); i++){
        // Get pilot
        complex<double> pilot_symbol = (MOD_STR[i] == "PSK") ? QPSK.at(PILOT) : QAM16.at(PILOT);
        // Generate data
        vector<int> seq = generate_sequence_bins(MOD_COMPLEXITY[i], M);
        // Modulate sequence
        vector<complex<double>> modulated_seq = modulate_sequence(seq, MOD_COMPLEXITY[i]);    // <- 
        // Save data
        separate_and_save_complex(modulated_seq, MOD_COMPLEXITY_STR[i] + ".csv", "clean", true);

        /*
        -----------Iterate by SNR only--------------
        */ 
        for (int j = 0; j < SNR_PLOT.size(); j++){
            // Add noise
            vector<complex<double>> noisy_seq = add_noise(modulated_seq, MOD_COMPLEXITY[i], SNR_PLOT[j]);
            // Save data
            separate_and_save_complex(noisy_seq, MOD_COMPLEXITY_STR[i] + "_snr.csv", to_string(SNR_PLOT[j]), bool_plot);
            bool_plot = false;
        }
        // Reset bool flags
        bool_plot = true;
        /*
        ----------Iterate multipath sims-------------
        */
       // Infinite paths
        for (int j = 0; j < SNR_PLOT.size(); j++){
            CArray H_rayleigh = generate_rayleigh_mpth(modulated_seq.size());
            vector<complex<double>> r_data = apply_channel(modulated_seq, H_rayleigh);
            vector<complex<double>> noisy_r_data = add_noise(r_data, MOD_COMPLEXITY[i], SNR_PLOT[j]);
            // Save distorted data
            separate_and_save_complex(noisy_r_data, MOD_COMPLEXITY_STR[i] + "_rayleigh.csv", to_string(SNR_PLOT[j]), bool_plot);
            bool_plot = false;
            // Separate pilots
            CArray noise_r_syms, noise_r_pilots, r_syms, r_pilots;
            tie(noise_r_syms, noise_r_pilots) = remove_pilot_symbols(noisy_r_data, PILOT_PLOT); // channel to estimate
            // Equalize
            // Perfect channel estimation
            CArray H_syms, H_pilots;
            tie(H_syms, H_pilots) = remove_pilot_symbols(H_rayleigh, PILOT_PLOT); // Reference
            CArray H_eq_r = equalizate_channel(noise_r_syms, H_syms);
            // Save equalized data
            separate_and_save_complex(H_eq_r, MOD_COMPLEXITY_STR[i] + "_eq_rayleigh_perfect.csv", to_string(SNR_PLOT[j]), bool_perfect);
            bool_perfect = false;
            // Cubic spline channel estimation
            // TODO
            // FFT channel estimation
            // TODO
        }

        // Reset bool flags
        bool_plot = true, bool_perfect = true, bool_fft = true, bool_cubic = true;


        // Finite paths (Doppler)
        for (int j = 0; j < SNR_PLOT.size(); j++){
            CArray H_doppler = generate_doppler_mpth(modulated_seq.size(), PATH_PLOT, SPEED_PLOT, CARRIER_FREQ_PLOT);
           vector<complex<double>> d_data = apply_channel(modulated_seq, H_doppler);
            vector<complex<double>> noisy_d_data = add_noise(d_data, MOD_COMPLEXITY[i], SNR_PLOT[j]);
            // Save distorted data
            separate_and_save_complex(noisy_d_data, MOD_COMPLEXITY_STR[i] + "_doppler.csv", to_string(SNR_PLOT[j]), bool_plot);
            bool_plot = false;
            // Separate pilots
            CArray noise_d_syms, noise_d_pilots, d_syms, d_pilots;
            tie(noise_d_syms, noise_d_pilots) = remove_pilot_symbols(noisy_d_data, PILOT_PLOT); // channel to estimate
            // Equalize
            // Perfect channel estimation
            CArray H_syms, H_pilots;
            tie(H_syms, H_pilots) = remove_pilot_symbols(H_doppler, PILOT_PLOT); // Reference
            CArray H_eq_d = equalizate_channel(noise_d_syms, H_syms);
            // Save equalized data
            separate_and_save_complex(H_eq_d, MOD_COMPLEXITY_STR[i] + "_eq_doppler_perfect.csv", to_string(SNR_PLOT[j]), bool_perfect);
            bool_perfect = false;
            // Cubic spline channel estimation
            // TODO
            // FFT channel estimation
            // TODO
        }
        // Reset bool flags
        bool_plot = true, bool_perfect = true, bool_fft = true, bool_cubic = true;
    }
}
