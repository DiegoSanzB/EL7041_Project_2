#include <iostream>
#include <vector>
#include <complex>
#include <libalglib/fasttransforms.h>
#include <libalglib/interpolation.h>
#include <algorithm>
#include <fstream>
using namespace std;

void writeDataToFile(const std::string& filename, const std::vector<double>& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& value : data) {
            file << value << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

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
    vector<complex<double>> interpolated_values(intersize - n_interp);
    for (int i = 0; i < intersize - n_interp; ++i) {
        interpolated_values[i] = complex<double>(interpolated[i].x * factor, interpolated[i].y * factor);
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

int main() {
    complex<double> a (1.0, 0.0);
    complex<double> b (4.0, 0.0);
    cout << a << b << endl;
    vector<complex<double>> test_vector = {a, b, a, b};
    // Cubic
    // vector<complex<double>> interpolated_values = interpolateComplexPoints(a, b, 10);
    // FFT
    vector<complex<double>> interpolated_values = fftInterpolateComplex(test_vector, 10);
    vector<double> reales(interpolated_values.size());
    for (int i = 0; i < interpolated_values.size(); ++i) {
        cout << interpolated_values[i].real() << endl;
        reales[i] = interpolated_values[i].real();
    }
    // Plot real values
    writeDataToFile("interpolated_data.txt", reales);

    // Plot using gnuplot (example assumes gnuplot is installed)
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    fprintf(gnuplotPipe, "set title 'Interpolated FFT Data'\n");
    fprintf(gnuplotPipe, "plot 'interpolated_data.txt' with lines\n");
    fclose(gnuplotPipe);

    return 0;

}
