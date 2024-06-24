#include <fstream>
#include <toml++/toml.h>

#include <BER.hpp>
#include <channel_generator.hpp>
#include <data_manager.hpp>
#include <demodulation.hpp>
#include <sequence_generator.hpp>

using namespace std;

int main() {
    // Define constants
    const vector<int> mod_complexity = {4, 16};

    // auto tbl = toml::parse_file("./simulation.toml");
    // toml::table tbl = toml::parse(config);

    // cout << tbl["Simulation"]["mod_complexity"] << endl;

    // return 0;

    const vector<string> mod_complexity_str = {"QPSK", "16QAM"};
    const vector<string> mod_str = {"PSK", "QAM"};

    // Create a vector of snr values
    vector<int> snr(34);
    for (int i = 0; i < snr.size(); i++) {
        snr[i] = i - 2;
    }
    snr[33] = -5;
    // Pilots
    const vector<int> pilot_spacing = {5, 10, 20};
    // Paths, speed, carrier frequency
    const vector<int> paths = {5, 40};
    const vector<int> speed = {30, 120};
    const vector<double> carrier_freq = {7e8, 35e8};
    // Strings
    vector<string> column_names;
    // Plot variables
    vector<int> snr_plot = snr;//{-5, 0, 10, 30};
    const int pilot_plot = 5;
    const double carrier_freq_plot = 7e8;
    const int paths_plot = 5;
    const int speed_plot = 30;

    const int M = 1e5;

    /*
    Iterate by Modulation complexity and SNR
    */
    for (int i = 0; i < mod_complexity.size(); i++) { // Modulation complexity
    /*
    Plot sections Before estimation, comment if not needed
    Hola mr eee e eeee e e e e e e :D
    Donsans esta horrible de largo en vola hay que separarlo por partes xd
    */
        cout << "Modulation complexity: " << mod_complexity[i] << endl;
        bool snr_plot_flag = false, rayleigh_flag = false, doppler_flag = false, ber_flag = false;
        // Generate sequence
        vector<int> seq = generate_sequence_bins(mod_complexity[i], M);

        // Modulate sequence
        vector<complex<double>> modulated_seq = modulate_sequence(seq, mod_complexity[i]);

        // Add pilot
        // modulated_seq = add_pilot_symbols(modulated_seq, mod_complexity[i], pilot_plot);

        // Separate real and imaginary parts
        vector<double> real, imaginary;
        tie(real, imaginary) = separate_real_imaginary(modulated_seq);                        // Clean data
        vector<vector<double>> data = {real, imaginary};                                 
        column_names = {"re", "im"};                                     

        // Save data to csv
        write_to_csv(data, column_names, mod_complexity_str[i] + ".csv");

        // Add noise only
        vector<double> ber_doppler_perfect, ber_rayleigh_perfect, ber_snr;
        vector<double> ber_doppler_cubic, ber_rayleigh_cubic;
        vector<double> ber_doppler_fft, ber_rayleigh_fft;

        for (int j = 0; j < snr_plot.size(); j++) { // SNR
            vector<complex<double>> noisy_data = add_noise(modulated_seq, mod_complexity[i], snr_plot[j]);

            // Separate real and imaginary parts
            vector<double> noisy_real, noisy_imaginary;
            tie(noisy_real, noisy_imaginary) = separate_real_imaginary(noisy_data);
            vector<vector<double>> noisy_data_vector = {noisy_real, noisy_imaginary};

            // Rename columns
            column_names = {
                "re_noise_" + to_string(snr_plot[j]), 
                "im_noise_" + to_string(snr_plot[j])};

            // Save data to csv
            if (!snr_plot_flag) {
                write_to_csv(noisy_data_vector, column_names, mod_complexity_str[i] + "_snr.csv");
                snr_plot_flag = true;
            } 
            else {
                add_to_csv(noisy_data_vector, column_names, mod_complexity_str[i] + "_snr.csv");
            }

            // Add Rayleigh multipath + noise
            CArray H_rayleigh_mtph = generate_rayleigh_mpth(M);
            vector<complex<double>> rayleigh_data = apply_channel(modulated_seq, H_rayleigh_mtph);
            vector<complex<double>> noisy_rayleigh_data = add_noise(rayleigh_data, mod_complexity[i], snr_plot[j]);

            // Separate real and imaginary parts
            vector<double> rayleigh_real, rayleigh_imaginary;
            tie(rayleigh_real, rayleigh_imaginary) = separate_real_imaginary(noisy_rayleigh_data);
            vector<vector<double>> rayleigh_data_vector = {rayleigh_real, rayleigh_imaginary};

            // Rename columns
            column_names = {
                "re_rayleigh_noise_" + to_string(snr_plot[j]), 
                "im_rayleigh_noise_" + to_string(snr_plot[j])};
            
            // Save data to csv
            if (!rayleigh_flag) {
                write_to_csv(rayleigh_data_vector, column_names, mod_complexity_str[i] + "_rayleigh_snr.csv");
                rayleigh_flag = true;
            } 
            else {
                add_to_csv(rayleigh_data_vector, column_names, mod_complexity_str[i] + "_rayleigh_snr.csv");
            }

            // Add Doppler multipath + noise
            CArray H_doppler_mtph = generate_doppler_mpth(M, paths_plot, speed_plot, carrier_freq_plot);
            vector<complex<double>> doppler_data = apply_channel(modulated_seq, H_doppler_mtph);
            vector<complex<double>> noisy_doppler_data = add_noise(doppler_data, mod_complexity[i], snr_plot[j]);

            // Separate real and imaginary parts
            vector<double> doppler_real, doppler_imaginary;
            tie(doppler_real, doppler_imaginary) = separate_real_imaginary(noisy_doppler_data);
            vector<vector<double>> doppler_data_vector = {doppler_real, doppler_imaginary};

            // Rename columns
            column_names = {
                "re_doppler_noise_" + to_string(snr_plot[j]), 
                "im_doppler_noise_" + to_string(snr_plot[j])};
            
            // Save data to csv
            if (!doppler_flag) {
                write_to_csv(doppler_data_vector, column_names, mod_complexity_str[i] + "_doppler_snr.csv");
                doppler_flag = true;
            } 
            else {
                add_to_csv(doppler_data_vector, column_names, mod_complexity_str[i] + "_doppler_snr.csv");
            }

            // SACAR PILOTOS


            // 100.000/20 --interpolacion--> 100.000
            // H_doppler_mtph_estimated = interpolation_cubic(pilotos);

            // Perfect Channel Estimation
            // y = (x * H) + n -> noisy_doppler_data and noisy_rayleigh_data
            // yh = y / H
            
            // Equalization
            CArray noisy_doppler_data_perfect_csi = equalizate_channel(noisy_doppler_data, H_doppler_mtph);
            CArray noisy_rayleigh_data_perfect_csi = equalizate_channel(noisy_rayleigh_data, H_rayleigh_mtph);
            
            // Demodulation
            vector<int> doppler_seq_received, rayleigh_seq_received;
            doppler_seq_received = demod(noisy_doppler_data_perfect_csi, mod_complexity[i], mod_str[i]);
            rayleigh_seq_received = demod(noisy_rayleigh_data_perfect_csi, mod_complexity[i], mod_str[i]);

            // column_names = {
            // "original_sequence",
            // "doppler_sequence",
            // "rayleigh_sequence"
            // };

            // write_to_csv({seq, doppler_seq_received, rayleigh_seq_received}, column_names, mod_complexity_str[i] + "_demodulation.csv");
            
            ber_doppler_perfect.push_back(compute_BER(seq, doppler_seq_received));
            ber_rayleigh_perfect.push_back(compute_BER(seq, rayleigh_seq_received));
            ber_snr.push_back((double)snr_plot[j]);

            column_names = {
                "SNR",
                "BER_perfect"
            };

            

            printf("BER_Doppler = %f for SNR = %d, %s\n", compute_BER(seq, doppler_seq_received), snr_plot[j], mod_complexity_str[i].c_str());
            printf("BER_Rayleigh = %f for SNR = %d, %s\n", compute_BER(seq, rayleigh_seq_received), snr_plot[j], mod_complexity_str[i].c_str());
        }
        // Save data to csv
        column_names = {
            "SNR",
            "BER_perfect"
        };

        if (!ber_flag) {
            write_to_csv({ber_snr, ber_doppler_perfect}, column_names, mod_complexity_str[i] + "_BER_doppler.csv");
            write_to_csv({ber_snr, ber_rayleigh_perfect}, column_names, mod_complexity_str[i] + "_BER_rayleigh.csv");
            ber_flag = true;
        } 
    }
    return 0;
}

// -- END OF FILE -- // 