/*
Hola
*/

// Add standard libraries
#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>
#include <complex>
#include <unordered_map>
#include <random>
using namespace std;

#ifndef SEQUENCE_GENERATOR
#define SEQUENCE_GENERATOR
// Define constants
// const int MAX_SEQUENCE_LENGTH = 1e5;
const int MAX_SEQUENCE_LENGTH = 1e5;
const int PILOT = 3;
const double PI = 3.141592653589793238460;

// Map with modulations for each symbol
const unordered_map<int, complex<double>> QPSK = {
    {0, complex<double>(-1, -1)}, // 00
    {1, complex<double>(-1, 1)}, // 01
    {2, complex<double>(1, -1)}, // 10
    {3, complex<double>(1, 1)} // 11
};

const unordered_map<int, complex<double>> QAM16 = {
    {0, complex<double>(-3, -3)}, // 0000
    {1, complex<double>(-3, -1)}, // 0001
    {2, complex<double>(-3, 3)}, // 0010
    {3, complex<double>(-3, 1)}, // 0011
    {4, complex<double>(-1, -3)}, // 0100
    {5, complex<double>(-1, -1)}, // 0101
    {6, complex<double>(-1, 3)}, // 0110
    {7, complex<double>(-1, 1)}, // 0111
    {8, complex<double>(3, -3)}, // 1000
    {9, complex<double>(3, -1)}, // 1001
    {10, complex<double>(3, 3)}, // 1010
    {11, complex<double>(3, 1)}, // 1011
    {12, complex<double>(1, -3)}, // 1100
    {13, complex<double>(1, -1)}, // 1101
    {14, complex<double>(1, 3)}, // 1110
    {15, complex<double>(1, 1)} // 1111
};

// Functions
vector<int> generate_sequence_bins(int mod_complexity, int n = MAX_SEQUENCE_LENGTH);
vector<complex<double>> modulate_sequence(const vector<int>& sequence, int mod_complexity);
tuple<vector<double>, vector<double>> separate_real_imaginary(const vector<complex<double>>& data);
vector<complex<double>> add_noise(const vector<complex<double>>& data, int mod_complexity, double snr, std::mt19937& gen);
vector<complex<double>> add_pilot_symbols(const vector<complex<double>>& data, int mod_complexity, int pilot_spacing, int pilot_num = PILOT);
vector<complex<double>> apply_channel(const vector<complex<double>>& data, vector<complex<double>>& H);
vector<complex<double>> equalizate_channel(const vector<complex<double>>& data, vector<complex<double>>& H);
tuple<vector<complex<double>>, vector<complex<double>>> remove_pilot_symbols(const vector<complex<double>>& data, int pilot_spacing);
// vector<complex<double>> add_doppler_mpth(const vector<complex<double>>& data, int mod_complexity, int paths, double speed, double carrier_freq);
// vector<complex<double>> add_rayleigh_mpth(const vector<complex<double>>& data, int mod_complexity);
#endif