
// questions.hpp
#ifndef QUESTIONS_HPP
#define QUESTIONS_HPP
// Include all
#include <BER.hpp>
#include <channel_generator.hpp>
#include <data_manager.hpp>
#include <demodulation.hpp>
#include <interpolations.hpp>
#include <sequence_generator.hpp>

/*
------------------------------
       Write constants
------------------------------
*/ 
const vector<int> MOD_COMPLEXITY = {4, 16};
const vector<string> MOD_COMPLEXITY_STR = {"QPSK", "16QAM"};
const vector<string> MOD_STR = {"PSK", "QAM"};
// Int vector from -2 to 32
const vector<int> SNRs = []() {
    vector<int> snrs;
    for (int i = -2; i <= 30; i++) {
        snrs.push_back(i);
    }
    return snrs;
}();
const vector<int> PILOT_SPACING = {5, 10, 20};
// Paths, speed, carrier frequency
const vector<int> PATHS = {5, 40};
const vector<int> SPEED = {30, 120};
const vector<double> CARRIER_FREQ = {7e8, 35e8};
// Plot variables
const vector<int> SNR_PLOT = {-5, 0, 10, 30}; 
const int PILOT_PLOT = 5;
const double CARRIER_FREQ_PLOT = 7e8;
const int PATH_PLOT = 5;
const int SPEED_PLOT = 30;
// Bit amount
const int M = 1e5;
const int NUMBER_OF_RUNS = 2;
/*
------------------------------
       
------------------------------
*/ 

void separate_and_save_complex(const vector<complex<double>> &data, string CSV_FILENAME, string identifier, bool create_new_file);
void constellations(std::mt19937& gen);
void compute_ber_data(std::mt19937& gen, int i);

#endif