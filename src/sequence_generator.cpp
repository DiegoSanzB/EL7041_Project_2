#include "sequence_generator.hpp"

// Generate a random sequence of numbers 
vector<int> generate_sequence_bins(int mod_complexity, int n) {
    // cout << "Generating sequence of complexity " << mod_complexity << endl;
    // Get bit per symbol
    int bps = log2(mod_complexity);
    // determine max value for each simbol
    int max = pow(2, bps) - 1;
    // And how many to create
    int size = n/bps;
    cout << "Sequence of size " << size << " with max value " << max << " Which in binary looks like " << bitset<4>(max) << endl;
    // Create sequence and add random numbers
    vector<int> sequence(size);
    for (int i = 0; i < size; i++) {
        sequence[i] = rand() % (max + 1);
    }
    return sequence;
}
// Modulate data into complex numbers
vector<complex<double>> modulate_sequence(const vector<int>& sequence, int mod_complexity) {
    // cout << "Modulating sequence" << endl;
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

// Add pilot symbols (including first position)
vector<complex<double>> add_pilot_symbols(const vector<complex<double>>& data, int mod_complexity, int pilot_spacing, int pilot_num) {
    // cout << "Adding pilot symbols" << endl;
    // Modulate pilot symbol
    complex<double> pilot_symbol;
    if (mod_complexity == 4) {
        pilot_symbol = QPSK.at(PILOT)/sqrt(2);
    } else if (mod_complexity == 16) {
        pilot_symbol = QAM16.at(PILOT)/sqrt(10);
    } else {
        cerr << "Error: Modulation complexity not supported." << endl;
        return {};
    }
    // pilot_symbol = (1, 1);
    // Get size of data
    int size = data.size();
    // Calculate the number of pilots to insert
    int num_pilots = ceil(static_cast<double>(size) / pilot_spacing) + 1;
    // Create array to store data with pilot symbols
    vector<complex<double>> data_with_pilots(size + num_pilots);
    // Add pilot symbols
    int pilot_counter = 0;
    for (int i = 0; i < size + num_pilots; i++) {
        if (i % (pilot_spacing + 1) == 0) {
            data_with_pilots[i] = pilot_symbol;
            pilot_counter++;
        } else {
            data_with_pilots[i] = data[i - pilot_counter];
        }
    }
    return data_with_pilots;
}

// Tuple that separates data into real and imaginary parts
tuple<vector<double>, vector<double>> separate_real_imaginary(const vector<complex<double>>& data) {
    // cout << "Separating real and imaginary parts" << endl;
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
vector<complex<double>> add_noise(const vector<complex<double>>& data, int mod_complexity, double snr, std::mt19937& gen) {
    // cout << "Adding noise" << endl;
    // Get N_0 from SNR
    double N_0 = 1 / (pow(10, snr / 10)); // *log2(mod_complexity));
    // Get size of data
    int size = data.size();
    // // Calculate actual symbol energy
    // double Es = 0;
    // Create array to store noisy data
    vector<complex<double>> noisy_data(size);
    // Create gaussian generator
    normal_distribution<double> distribution(0.0, 1.0);
    // Add noise
    double noise_real, noise_imag;
    for (int i = 0; i < size; i++)
    {
        // Generate noise
        noise_real = distribution(gen) * (N_0 * 0.5);
        noise_imag = distribution(gen) * (N_0 * 0.5);
        // Add noise
        noisy_data[i] = data[i] + complex<double>(noise_real, noise_imag);
    }
    cout << snr << " " << noise_real << " "<< noise_imag << endl;
    return noisy_data;
}

// Add multipath
vector<complex<double>> apply_channel(const vector<complex<double>>& data, vector<complex<double>>& H) {
    // Apply multipath
    size_t size = data.size();

    vector<complex<double>> multipath_data(size);
    
    for (size_t i = 0; i < size; i++) {
        multipath_data[i] = data[i] * H[i];
    }

    return multipath_data;
}

vector<complex<double>> equalizate_channel(const vector<complex<double>>& data, vector<complex<double>>& H) {
    // Equalizate channel
    size_t size = data.size();
    vector<complex<double>> equalizated_data(size);

    if (size != H.size()) {
        cout << "[EQ] Sizes don't match!";
        return equalizated_data;
    }

    for (size_t i = 0; i < size; i++) {
        equalizated_data[i] = data[i] / H[i];
    }
    return equalizated_data;
}

// Remove pilots
tuple<vector<complex<double>>, vector<complex<double>>> remove_pilot_symbols(const vector<complex<double>>& data, int pilot_spacing) {
    // cout << "Removing pilot symbols" << endl;
    // Get size of data
    int size = data.size();
    // Create array to store data without pilot symbols, (size unknown)
    vector<complex<double>> data_without_pilots;
    vector<complex<double>> pilots;
    // Remove pilot symbols
    for (int i = 0; i < size; i++) {
        if (i % (pilot_spacing + 1) != 0) {
            data_without_pilots.push_back(data[i]);
        }
        else {
            pilots.push_back(data[i]);
        }
    }
    return make_tuple(data_without_pilots, pilots);
}
// MOVI ESTO A channel_generator PARA MANEJAR POR SEPARADO EL H

// Option 1 - Reflections and Doppler
// vector<complex<double>> add_doppler_mpth(const vector<complex<double>>& data, int mod_complexity, int paths, double speed, double carrier_freq) {
//     // cout << "Adding multipath" << endl;
//     // Get size of data
//     int size = data.size();

//     // Create array to store multipath data
//     vector<complex<double>> multipath_data(size);

//     // Create gaussian generator
//     default_random_engine generator;
//     // normal_distribution<double> distribution(0, 1);
//     uniform_real_distribution<> dis_angle(0, 2 * PI);

//     // Get lambda
//     double lambda = 3e8 / carrier_freq; 

//     // Get f_max (With speed in m/s)
//     double max_doppler = (speed / 3.6) / lambda;

//     // t vector
//     vector<double> t(size);
//     for (int i = 0; i < size; i++) {
//         t[i] = static_cast<double>(i) / size;
//     }

//     // Normalized power
//     double an = 1 / sqrt(paths);

//     // Random phase
//     vector<double> thetan(paths);
//     for (int i = 0; i < paths; i++) {
//         thetan[i] = dis_angle(generator);
//     }

//     // Doppler
//     vector<double> fDn(paths);
//     for (int i = 0; i < paths; i++) {
//         fDn[i] = max_doppler * cos(2 * PI * dis_angle(generator));
//     }

//     // Initialize multipath channel
//     vector<complex<double>> H(size, complex<double>(0.0, 0.0));

//     // Create multipath
//     for (int n = 0; n < paths; n++) {
//         for (int i = 0; i < size; i++) {
//             H[i] += an * exp(complex<double>(0, thetan[n] - 2 * PI * fDn[n] * t[i]));
//         }
//     }
    
//     // Apply multipath
//     for (int i = 0; i < size; i++) {
//         multipath_data[i] = data[i] * H[i];
//     }

//     return multipath_data;

// }

// Option 2 - Rayleigh
// vector<complex<double>> add_rayleigh_mpth(const vector<complex<double>>& data, int mod_complexity) {
//     // cout << "Adding Rayleigh multipath" << endl;
//     // Get size of data
//     int size = data.size();

//     // Create array to store multipath data
//     vector<complex<double>> multipath_data(size);

//     // Create gaussian generator
//     default_random_engine generator;
//     normal_distribution<double> distribution(0.0, 1.0);

//     // Initialize multipath channel
//     vector<complex<double>> H(size, complex<double>(0.0, 0.0));

//     // Create multipath
//     for (int i = 0; i < size; i++) {
//         H[i] = complex<double>(distribution(generator), distribution(generator)) * sqrt(0.5);
//     }
    
//     // Apply multipath
//     for (int i = 0; i < size; i++) {
//         multipath_data[i] = data[i] * H[i];
//     }
//     return multipath_data;
    
// }


// -- END OF FILE -- //