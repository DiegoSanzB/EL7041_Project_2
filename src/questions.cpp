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

CArray cubic_interpolation(CArray pilots, Complex pilot_symbol, size_t n_interp, size_t out_size) {
    CArray interpolation(out_size), temp_interp;
    for (size_t i = 0; i < pilots.size() - 1; i++) {
        // get channel estimates
        Complex csi0 = pilots[i]/pilot_symbol, csi1 = pilots[i+1]/pilot_symbol;
        // compute interpolation
        temp_interp = interpolateComplexPoints(csi0, csi1, n_interp + 2);
        // assign interpolation out vector
        for (size_t j = 0; j < temp_interp.size() - 2; j++) {
            interpolation[i*n_interp + j] = temp_interp[j + 1];
        }
    }
    return interpolation;
}

CArray fft_interpolation(CArray pilots, Complex pilot_symbol, size_t n_interp) {
    CArray csi, interpolation;
    for (auto value: pilots) {
        csi.push_back(value/pilot_symbol);
    }
    interpolation = fftInterpolateComplex(csi, n_interp);
    
    return interpolation;
}

void constellations(std::mt19937& gen){
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
        // Add pilot
        modulated_seq = add_pilot_symbols(modulated_seq, MOD_COMPLEXITY[i], PILOT_PLOT);
        // Save data
        separate_and_save_complex(modulated_seq, MOD_COMPLEXITY_STR[i] + ".csv", "clean", true);

        /*
        -----------Iterate by SNR only--------------
        */ 
        for (int j = 0; j < SNR_PLOT.size(); j++){
            // Add noise
            vector<complex<double>> noisy_seq = add_noise(modulated_seq, MOD_COMPLEXITY[i], SNR_PLOT[j], gen);
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
            CArray H_rayleigh = generate_rayleigh_mpth(modulated_seq.size(), gen);
            vector<complex<double>> r_data = apply_channel(modulated_seq, H_rayleigh);
            vector<complex<double>> noisy_r_data = add_noise(r_data, MOD_COMPLEXITY[i], SNR_PLOT[j], gen);
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
            CArray H_cubic = cubic_interpolation(noise_r_pilots, pilot_symbol, PILOT_PLOT, noise_r_syms.size());
            CArray H_cubic_eq = equalizate_channel(noise_r_syms, H_cubic);
            // Save equalized data
            separate_and_save_complex(H_cubic_eq, MOD_COMPLEXITY_STR[i] + "_eq_rayleigh_cubic.csv", to_string(SNR_PLOT[j]), bool_cubic);
            bool_cubic = false;
            // FFT channel estimation
            CArray H_fft = fft_interpolation(noise_r_pilots, pilot_symbol, PILOT_PLOT);
            CArray H_fft_eq = equalizate_channel(noise_r_syms, H_fft);
            // Save equalized data
            separate_and_save_complex(H_fft_eq, MOD_COMPLEXITY_STR[i] + "_eq_rayleigh_fft.csv", to_string(SNR_PLOT[j]), bool_fft);
            bool_fft = false;
        }

        // Reset bool flags
        bool_plot = true, bool_perfect = true, bool_fft = true, bool_cubic = true;

        // Finite paths (Doppler)
        for (int j = 0; j < SNR_PLOT.size(); j++){
            CArray H_doppler = generate_doppler_mpth(modulated_seq.size(), PATH_PLOT, SPEED_PLOT, CARRIER_FREQ_PLOT, gen);
            vector<complex<double>> d_data = apply_channel(modulated_seq, H_doppler);
            vector<complex<double>> noisy_d_data = add_noise(d_data, MOD_COMPLEXITY[i], SNR_PLOT[j], gen);
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
            CArray H_cubic = cubic_interpolation(noise_d_pilots, pilot_symbol, PILOT_PLOT, noise_d_syms.size());
            CArray H_cubic_eq = equalizate_channel(noise_d_syms, H_cubic);
            // Save equalized data
            separate_and_save_complex(H_cubic_eq, MOD_COMPLEXITY_STR[i] + "_eq_doppler_cubic.csv", to_string(SNR_PLOT[j]), bool_cubic);
            bool_cubic = false;
            // FFT channel estimation
            CArray H_fft = fft_interpolation(noise_d_pilots, pilot_symbol, PILOT_PLOT);
            CArray H_fft_eq = equalizate_channel(noise_d_syms, H_fft);
            // Save equalized data
            separate_and_save_complex(H_fft_eq, MOD_COMPLEXITY_STR[i] + "_eq_doppler_fft.csv", to_string(SNR_PLOT[j]), bool_cubic);
            bool_fft = false;
        }
        // Reset bool flags
        bool_plot = true, bool_perfect = true, bool_fft = true, bool_cubic = true;
    }
}

vector<double> compute_ber_scenario(std::mt19937& gen, int pilot_spacing, int i, int path, int speed, double carrier_freq, int snr) {
    vector<double> out;
    // Get pilot
    complex<double> pilot_symbol = (MOD_STR[i] == "PSK") ? QPSK.at(PILOT) : QAM16.at(PILOT);
    // Generate data
    vector<int> seq = generate_sequence_bins(MOD_COMPLEXITY[i], M);
    // Modulate sequence
    vector<complex<double>> modulated_seq = modulate_sequence(seq, MOD_COMPLEXITY[i]);
    // Add pilot
    modulated_seq = add_pilot_symbols(modulated_seq, MOD_COMPLEXITY[i], pilot_spacing);

    // RAYLEIGH
    CArray H_rayleigh = generate_rayleigh_mpth(modulated_seq.size(), gen);
    vector<complex<double>> r_data = apply_channel(modulated_seq, H_rayleigh);
    vector<complex<double>> noisy_r_data = add_noise(r_data, MOD_COMPLEXITY[i], snr, gen);
    // Separate pilots
    CArray noise_r_syms, noise_r_pilots, r_syms, r_pilots;
    tie(noise_r_syms, noise_r_pilots) = remove_pilot_symbols(noisy_r_data, pilot_spacing); // channel to estimate
    // Perfect channel estimation
    CArray H_syms, H_pilots;
    tie(H_syms, H_pilots) = remove_pilot_symbols(H_rayleigh, pilot_spacing); // Reference
    CArray syms_perf_r = equalizate_channel(noise_r_syms, H_syms);
    vector<int> seq_perf_r = demod(syms_perf_r, MOD_COMPLEXITY[i], MOD_STR[i]);
    out.push_back(compute_BER(seq, seq_perf_r, MOD_COMPLEXITY[i]));
    // Cubic spline channel estimation
    CArray H_cubic = cubic_interpolation(noise_r_pilots, pilot_symbol, pilot_spacing, noise_r_syms.size());
    CArray syms_cubic_r = equalizate_channel(noise_r_syms, H_cubic);
    vector<int> seq_cubic_r = demod(syms_cubic_r, MOD_COMPLEXITY[i], MOD_STR[i]);
    out.push_back(compute_BER(seq, seq_cubic_r, MOD_COMPLEXITY[i]));
    // FFT channel estimation
    CArray H_fft = fft_interpolation(noise_r_pilots, pilot_symbol, pilot_spacing);
    CArray syms_fft_r = equalizate_channel(noise_r_syms, H_fft);
    vector<int> seq_fft_r = demod(syms_fft_r, MOD_COMPLEXITY[i], MOD_STR[i]);
    out.push_back(compute_BER(seq, seq_fft_r, MOD_COMPLEXITY[i]));

    // DOPPLER
    CArray H_doppler = generate_doppler_mpth(modulated_seq.size(), path, speed, carrier_freq, gen);
    vector<complex<double>> d_data = apply_channel(modulated_seq, H_doppler);
    vector<complex<double>> noisy_d_data = add_noise(d_data, MOD_COMPLEXITY[i], snr, gen);
    // Separate pilots
    CArray noise_d_syms, noise_d_pilots, d_syms, d_pilots;
    tie(noise_d_syms, noise_d_pilots) = remove_pilot_symbols(noisy_d_data, pilot_spacing); // channel to estimate
    // Perfect channel estimation
    tie(H_syms, H_pilots) = remove_pilot_symbols(H_doppler, pilot_spacing); // Reference
    CArray syms_perf_d = equalizate_channel(noise_d_syms, H_syms);
    vector<int> seq_perf_d = demod(syms_perf_d, MOD_COMPLEXITY[i], MOD_STR[i]);
    out.push_back(compute_BER(seq, seq_perf_d, MOD_COMPLEXITY[i]));
    // Cubic spline channel estimation
    H_cubic = cubic_interpolation(noise_d_pilots, pilot_symbol, pilot_spacing, noise_d_syms.size());
    CArray syms_cubic_d = equalizate_channel(noise_d_syms, H_cubic);
    vector<int> seq_cubic_d = demod(syms_cubic_d, MOD_COMPLEXITY[i], MOD_STR[i]);
    out.push_back(compute_BER(seq, seq_cubic_d, MOD_COMPLEXITY[i]));
    // FFT channel estimation
    H_fft = fft_interpolation(noise_d_pilots, pilot_symbol, pilot_spacing);
    CArray syms_fft_d = equalizate_channel(noise_d_syms, H_fft);
    vector<int> seq_fft_d = demod(syms_fft_d, MOD_COMPLEXITY[i], MOD_STR[i]);
    out.push_back(compute_BER(seq, seq_fft_d, MOD_COMPLEXITY[i]));
    
    return out;
}

// i -> mod_complexity
void compute_ber_data(std::mt19937& gen, int i) {
    bool ber_flag = false;
    string ber_file = "BER_" + MOD_COMPLEXITY_STR[i] + ".csv";
    vector<string> column_names = {
        "BER_R_perfect",
        "BER_R_cubic",
        "BER_R_fft",
        "BER_D_perfect",
        "BER_D_cubic",
        "BER_D_fft",
        "snr",
        "pilot_spacing",
        "path",
        "speed",
        "carrier_freq",
        "mod_complexity"
    };

    // for (int i = 0; i < MOD_COMPLEXITY.size(); i++){ // esto corre en threads separados y se guarda en dos archivos
    for (int pilot_spacing: PILOT_SPACING) {
    for (int path: PATHS) {
    for (int speed: SPEED) {
    for (double carrier_freq: CARRIER_FREQ) {
    for (int snr: SNRs) {
    vector<double> out_j, out = {0, 0, 0, 0, 0, 0};
        for (int run = 0; run < NUMBER_OF_RUNS; run++) {
            out_j = compute_ber_scenario(gen, pilot_spacing, i, path, speed, carrier_freq, snr);
            for (int j = 0; j < 6; j++) {
                out[j] += out_j[j];
            }
        }
        for (int j = 0; j < 6; j++) {
            out[j] /= NUMBER_OF_RUNS;
        }
        vector<double> row = out;
        row.push_back((double) snr);
        row.push_back((double) pilot_spacing);
        row.push_back((double) path);
        row.push_back((double) speed);
        row.push_back(carrier_freq);
        row.push_back((double) MOD_COMPLEXITY[i]);
        
        if (ber_flag) {
            append_to_csv(row, ber_file);
        } else {
            vector<vector<double>> row_formatted;
            for (auto value: row) {
                row_formatted.push_back({value});
            }
            write_to_csv(row_formatted, column_names, ber_file);
            ber_flag = true;
        }
    }
    }
    }
    }
    }
    // }
}
