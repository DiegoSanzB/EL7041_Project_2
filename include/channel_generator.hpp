#ifndef CHANNEL_GENERATOR
#define CHANNEL_GENERATOR

#include <complex>
#include <cmath>
#include <math.h>
#include <random>
#include <vector>

#include <sequence_generator.hpp>

using namespace std;

#ifndef COMPLEX
#define COMPLEX

typedef complex<double> Complex;
typedef vector<Complex> CArray;

#endif

CArray generate_doppler_mpth(int size, int paths, double speed, double carrier_freq);
CArray generate_rayleigh_mpth(int size);

#endif