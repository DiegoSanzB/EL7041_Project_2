#include "QAM Demodulation.h"
#include <iostream>
#include <complex>
#include <string>

#include <sequence_generator.hpp>

// QAM demodulation
vector<int> qamdemod_interface(vector<double> I, vector<double> Q, int modulationOrder);

// PSK demodulation
vector<int> pskdemod(vector<complex<double>> data, int modulationOrder);


// General demodulation, passing the modulation Type calls the right function
vector<int> demod(vector<complex<double>> data, int modulationOrder, string modulationType);
