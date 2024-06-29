#include <iostream>
#include <vector>
#include <complex>
#include <fasttransforms.h>
#include <interpolation.h>
#include <algorithm>
#include <fstream>
using namespace std;

vector<complex<double>> interpolateComplexPoints(const complex<double>& point1, const complex<double>& point2, int n_interp);
vector<complex<double>> fftInterpolateComplex(const complex<double>& point1, const complex<double>& point2, int n_interp);