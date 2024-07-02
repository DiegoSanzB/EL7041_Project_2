#include <interpolations.hpp>
using namespace std;


vector<complex<double>> fftInterpolateComplex(const vector<complex<double>>& original, int n_interp) {
    // Get array size
    int n = original.size();
    // Determine lenght of interpolation array
    int intersize = n + (n) * n_interp;
    // Create ALGLIB array
    alglib::complex_1d_array original_alglib;
    original_alglib.setlength(n);
    // Insert values
    for (int i = 0; i < n; ++i) {
        original_alglib[i] = alglib::complex(original[i].real(), original[i].imag());
    }
    // Apply a fft for data
    alglib::fftc1d(original_alglib);
    // Add zeros
    alglib::complex_1d_array interpolated;
    interpolated.setlength(intersize);
    int half_n = n / 2; // Half of the original size
    // Copy the low-frequency components
    for (int i = 0; i <= half_n; ++i) { // Include the Nyquist frequency
        interpolated[i] = original_alglib[i];
    }
    // High-frequency components are copied to the end, maintaining symmetry
    for (int i = 1; i < half_n; ++i) { // Skip the Nyquist frequency if n is even
        interpolated[intersize - i] = original_alglib[n - i];
    }
    // Apply inverse fft
    alglib::fftc1dinv(interpolated);
    // Denormalize
    double factor = static_cast<double>(intersize) / n;
    // Convert to vector
    vector<complex<double>> interpolated_values(intersize - n_interp - n);
    int idx = 0;
    for (int i = 0; i < intersize - n_interp - n; ++i) {
        // if i is divisible by n_interp, skip the value
        if (i % n_interp == 0) {
            idx++;
        } 
        interpolated_values[i] = complex<double>(interpolated[i + idx].x * factor, interpolated[i + idx].y * factor);
    }
    return interpolated_values;
}

vector<complex<double>> interpolateComplexPoints(const complex<double>& point1, const complex<double>& point2, int n_interp) {
    // Prepare data for ALGLIB
    vector<double> x{0, 1}; // Assuming two points are at 0 and 1 for interpolation
    vector<double> y_real{point1.real(), point2.real()};
    vector<double> y_imag{point1.imag(), point2.imag()};

    // Create ALGLIB's real_1d_array for x, y_real, and y_imag
    alglib::real_1d_array x_alglib, y_real_alglib, y_imag_alglib;
    x_alglib.setcontent(x.size(), x.data());
    y_real_alglib.setcontent(y_real.size(), y_real.data());
    y_imag_alglib.setcontent(y_imag.size(), y_imag.data());

    // Perform interpolation
    alglib::spline1dinterpolant s_real, s_imag;
    alglib::spline1dbuildcubic(x_alglib, y_real_alglib, s_real);
    alglib::spline1dbuildcubic(x_alglib, y_imag_alglib, s_imag);

    // Generate interpolated values
    vector<complex<double>> interpolated_values;
    for (int i = 0; i < n_interp; ++i) {
        double xi = static_cast<double>(i) / (n_interp - 1); // Normalize i to [0, 1]
        double real_interp = alglib::spline1dcalc(s_real, xi);
        double imag_interp = alglib::spline1dcalc(s_imag, xi);
        interpolated_values.push_back(complex<double>(real_interp, imag_interp));
    }

    return interpolated_values;
}
