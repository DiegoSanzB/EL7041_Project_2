#include <fstream>
#include <toml++/toml.h>

#include <BER.hpp>
#include <channel_generator.hpp>
#include <data_manager.hpp>
#include <demodulation.hpp>
#include <interpolations.hpp>
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
    vector<int> snr_plot =  snr; //{-5, 0, 10, 30}; //  
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
        modulated_seq = add_pilot_symbols(modulated_seq, mod_complexity[i], pilot_plot);

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
            CArray H_rayleigh_mtph = generate_rayleigh_mpth(modulated_seq.size());
            vector<complex<double>> rayleigh_data = apply_channel(modulated_seq, H_rayleigh_mtph);
            vector<complex<double>> noisy_rayleigh_data = add_noise(rayleigh_data, mod_complexity[i], snr_plot[j]);

            // Separate real and imaginary parts
            vector<double> rayleigh_real, rayleigh_imaginary;
            tie(rayleigh_real, rayleigh_imaginary) = separate_real_imaginary(noisy_rayleigh_data);
            vector<vector<double>> rayleigh_data_vector = {rayleigh_real, rayleigh_imaginary};

            // Remove pilots
            CArray noisy_rayleigh_symbols, noisy_rayleigh_pilots;
            tie(noisy_rayleigh_symbols, noisy_rayleigh_pilots) = remove_pilot_symbols(noisy_rayleigh_data, pilot_plot);
            // Remove pilots from H
            CArray H_rayleigh_mtph_perfect, H_rayleigh_mtph_pilot;
            tie(H_rayleigh_mtph_perfect, H_rayleigh_mtph_pilot) = remove_pilot_symbols(H_rayleigh_mtph, pilot_plot);

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
            CArray H_doppler_mtph = generate_doppler_mpth(modulated_seq.size(), paths_plot, speed_plot, carrier_freq_plot);
            vector<complex<double>> doppler_data = apply_channel(modulated_seq, H_doppler_mtph);
            vector<complex<double>> noisy_doppler_data = add_noise(doppler_data, mod_complexity[i], snr_plot[j]);

            // Separate real and imaginary parts
            vector<double> doppler_real, doppler_imaginary;
            tie(doppler_real, doppler_imaginary) = separate_real_imaginary(noisy_doppler_data);
            vector<vector<double>> doppler_data_vector = {doppler_real, doppler_imaginary};

            // Remove pilots
            CArray noisy_doppler_symbols, noisy_doppler_pilots;
            tie(noisy_doppler_symbols, noisy_doppler_pilots) = remove_pilot_symbols(noisy_doppler_data, pilot_plot);
            // Remove pilots from H
            CArray H_doppler_mtph_perfect, H_doppler_mtph_pilots;
            tie(H_doppler_mtph_perfect, H_doppler_mtph_pilots) = remove_pilot_symbols(H_doppler_mtph, pilot_plot);

            // Rename columns
            column_names = {
                "re_doppler_noise_" + to_string(snr_plot[j]), 
                "im_doppler_noise_" + to_string(snr_plot[j])
                };
            
            // Save data to csv
            if (!doppler_flag) {
                write_to_csv(doppler_data_vector, column_names, mod_complexity_str[i] + "_doppler_snr.csv");
                doppler_flag = true;
            } 
            else {
                add_to_csv(doppler_data_vector, column_names, mod_complexity_str[i] + "_doppler_snr.csv");
            }            

            // Perfect Channel Estimation
            // y = (x * H) + n -> noisy_doppler_data and noisy_rayleigh_data
            // yh = y / H
            
            // Equalization
            CArray noisy_doppler_data_perfect_csi = equalizate_channel(noisy_doppler_symbols, H_doppler_mtph_perfect);
            CArray noisy_rayleigh_data_perfect_csi = equalizate_channel(noisy_rayleigh_symbols, H_rayleigh_mtph_perfect);
            
            // Demodulation
            vector<int> doppler_seq_received, rayleigh_seq_received;
            doppler_seq_received = demod(noisy_doppler_data_perfect_csi, mod_complexity[i], mod_str[i]);
            rayleigh_seq_received = demod(noisy_rayleigh_data_perfect_csi, mod_complexity[i], mod_str[i]);

            // PILOT SYMBOL
            complex<double> pilot_symbol;

            if (mod_str[i] == "PSK")
            {
                pilot_symbol = QPSK.at(3)/sqrt(2);
            } else {
                pilot_symbol = QAM16.at(3)/sqrt(10);
            }

            // CUBIC INTERPOLATION 
            CArray H_doppler_mtph_cubic(noisy_doppler_symbols.size()), H_temp_interp;
            for (size_t p = 0; p < noisy_doppler_pilots.size()-1; p++) {
                complex<double> csi0 = noisy_doppler_pilots[p]/pilot_symbol, csi1 = noisy_doppler_pilots[p+1]/pilot_symbol;
                H_temp_interp = interpolateComplexPoints(csi0, csi1, pilot_plot+2);

                for (size_t n = 0; n < H_temp_interp.size()-2; n++) {
                    H_doppler_mtph_cubic[p*n + n] = H_temp_interp[n+1];
                }
            }
            CArray H_rayleigh_mtph_cubic(noisy_rayleigh_symbols.size());
            for (size_t p = 0; p < noisy_rayleigh_pilots.size()-1; p++) {
                complex<double> csi0 = noisy_rayleigh_pilots[p]/pilot_symbol, csi1 = noisy_rayleigh_pilots[p+1]/pilot_symbol;
                H_temp_interp = interpolateComplexPoints(csi0, csi1, pilot_plot+2);

                for (size_t n = 0; n < H_temp_interp.size()-2; n++) {
                    H_rayleigh_mtph_cubic[p*n + n] = H_temp_interp[n+1];
                }
            }
            

            // Equalization cubic interpolation
            CArray noisy_doppler_data_cubic = equalizate_channel(noisy_doppler_symbols, H_doppler_mtph_cubic);
            CArray noisy_rayleigh_data_cubic = equalizate_channel(noisy_rayleigh_symbols, H_rayleigh_mtph_cubic);
            
            // Demodulation
            vector<int> doppler_seq_received_cubic, rayleigh_seq_received_cubic;
            doppler_seq_received_cubic = demod(noisy_doppler_data_cubic, mod_complexity[i], mod_str[i]);
            rayleigh_seq_received_cubic = demod(noisy_rayleigh_data_cubic, mod_complexity[i], mod_str[i]);

            // FFT interpolation
            CArray H_doppler_mtph_fft(noisy_doppler_symbols.size());
            for (size_t p = 0; p < noisy_doppler_pilots.size()-1; p++) {
                complex<double> csi0 = noisy_doppler_pilots[p]/pilot_symbol, csi1 = noisy_doppler_pilots[p+1]/pilot_symbol;
                H_temp_interp = fftInterpolateComplex(csi0, csi1, pilot_plot);

                for (size_t n = 0; n < H_temp_interp.size(); n++) {
                    H_doppler_mtph_fft[p*n + n] = H_temp_interp[n];
                }
            }
            CArray H_rayleigh_mtph_fft(noisy_rayleigh_symbols.size());
            for (size_t p = 0; p < noisy_rayleigh_pilots.size()-1; p++) {
                complex<double> csi0 = noisy_rayleigh_pilots[p]/pilot_symbol, csi1 = noisy_rayleigh_pilots[p+1]/pilot_symbol;
                H_temp_interp = fftInterpolateComplex(csi0, csi1, pilot_plot);

                for (size_t n = 0; n < H_temp_interp.size(); n++) {
                    H_rayleigh_mtph_fft[p*n + n] = H_temp_interp[n];
                }
            }

            // Equalization fft interpolation
            CArray noisy_doppler_data_fft = equalizate_channel(noisy_doppler_symbols, H_doppler_mtph_fft);
            CArray noisy_rayleigh_data_fft = equalizate_channel(noisy_rayleigh_symbols, H_rayleigh_mtph_fft);
            
            // Demodulation
            vector<int> doppler_seq_received_fft, rayleigh_seq_received_fft;
            doppler_seq_received_fft = demod(noisy_doppler_data_fft, mod_complexity[i], mod_str[i]);
            rayleigh_seq_received_fft = demod(noisy_rayleigh_data_fft, mod_complexity[i], mod_str[i]);


            // --------------

            CArray modulated_seq_symbols, modulated_seq_pilots;
            tie(modulated_seq_symbols, modulated_seq_pilots) = remove_pilot_symbols(modulated_seq, pilot_plot);
            vector<int> aaa = demod(modulated_seq_symbols, mod_complexity[i], mod_str[i]);
            
            // PERFECT CSI
            ber_doppler_perfect.push_back(compute_BER(aaa, doppler_seq_received, mod_complexity[i]));
            ber_rayleigh_perfect.push_back(compute_BER(aaa, rayleigh_seq_received, mod_complexity[i]));
            // Cubic
            ber_doppler_cubic.push_back(compute_BER(aaa, doppler_seq_received_cubic, mod_complexity[i]));
            ber_rayleigh_cubic.push_back(compute_BER(aaa, rayleigh_seq_received_cubic, mod_complexity[i]));
            // FFT interpolation
            ber_doppler_fft.push_back(compute_BER(aaa, doppler_seq_received_fft, mod_complexity[i]));
            ber_rayleigh_fft.push_back(compute_BER(aaa, rayleigh_seq_received_fft, mod_complexity[i]));


            ber_snr.push_back((double)snr_plot[j]);

            // column_names = {
            //     "SNR",
            //     "BER_perfect",
            //     "BER_cubic"
            // };
            printf("BER_Doppler = %f for SNR = %d, %s\n", compute_BER(aaa, doppler_seq_received, mod_complexity[i]), snr_plot[j], mod_complexity_str[i].c_str());
            printf("BER_Rayleigh = %f for SNR = %d, %s\n", compute_BER(aaa, rayleigh_seq_received, mod_complexity[i]), snr_plot[j], mod_complexity_str[i].c_str());
        }
        // Save data to csv
        column_names = {
            "SNR",
            "BER_perfect",
            "BER_cubic",
            "BER_fft"
        };

        if (!ber_flag) {
            write_to_csv({ber_snr, ber_doppler_perfect, ber_doppler_cubic, ber_doppler_fft}, column_names, mod_complexity_str[i] + "_BER_doppler.csv");
            write_to_csv({ber_snr, ber_rayleigh_perfect, ber_rayleigh_cubic, ber_doppler_fft}, column_names, mod_complexity_str[i] + "_BER_rayleigh.csv");
            ber_flag = true;
        } 
    }
    return 0;
}

// -- END OF FILE -- // 