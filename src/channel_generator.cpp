#include <channel_generator.hpp>


// Option 1 - Reflections and Doppler
CArray generate_doppler_mpth(int size, int paths, double speed, double carrier_freq,std::mt19937& gen) {
    // cout << "Generating multipath" << endl;

    // Create array to store multipath data
    CArray multipath_data(size);

    // Create gaussian generator
    // normal_distribution<double> distribution(0, 1);
    uniform_real_distribution<> dis_angle(0, 2 * PI);

    // Get lambda
    double lambda = 3e8 / carrier_freq; 

    // Get f_max (With speed in m/s)
    double max_doppler = (speed / 3.6) / lambda;

    // t vector
    vector<double> t(size);
    for (int i = 0; i < size; i++) {
        t[i] = static_cast<double>(i) / size;
    }

    // Normalized power
    double an = 1 / sqrt(paths);

    // Random phase
    vector<double> thetan(paths);
    for (int i = 0; i < paths; i++) {
        thetan[i] = dis_angle(gen);
    }

    // Doppler
    vector<double> fDn(paths);
    for (int i = 0; i < paths; i++) {
        fDn[i] = max_doppler * cos(2 * PI * dis_angle(gen));
    }

    // Initialize multipath channel
    CArray H(size, complex<double>(0.0, 0.0));

    // Create multipath
    for (int n = 0; n < paths; n++) {
        for (int i = 0; i < size; i++) {
            H[i] += an * exp(complex<double>(0, thetan[n] - 2 * PI * fDn[n] * t[i]));
        }
    }

    return H;

}

// Option 2 - Rayleigh
CArray generate_rayleigh_mpth(int size, std::mt19937& gen) {
    // cout << "Generating Rayleigh multipath" << endl;

    // Create array to store multipath data
    CArray multipath_data(size);

    // Create gaussian generator
    normal_distribution<double> distribution(0.0, 1.0);

    // Initialize multipath channel
    CArray H(size, complex<double>(0.0, 0.0));

    // Create multipath
    for (int i = 0; i < size; i++) {
        H[i] = complex<double>(distribution(gen), distribution(gen)) * sqrt(0.5);
    }
    
    return H;
    
}
