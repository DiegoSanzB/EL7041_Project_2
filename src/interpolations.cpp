#include <interpolations.hpp>
using namespace std;


vector<complex<double>> fftInterpolateComplex(const complex<double>& point1, const complex<double>& point2, int n_interp) {
    // Initialize two input datas
    // alglib::real_1d_array 
    alglib::real_1d_array Re, Im;
    Re.setlength(2);
    Im.setlength(2);
    Re[0] = point1.real(); Re[1] = point2.real();
    Im[0] = point1.imag(); Im[1] = point2.imag();
    

    // Perform FFT on the original data
    alglib::complex_1d_array re;
    alglib::complex_1d_array im;
    fftr1d(Re, re);
    fftr1d(Im, im);

    // Prepare new arrays for zero-padded frequency domain data
    alglib::complex_1d_array Re_freq, Im_freq;
    Re_freq.setlength(n_interp);
    Im_freq.setlength(n_interp);
    for (int i = 0; i < n_interp; ++i) {
        Re_freq[i] = alglib::complex(0, 0);
        Im_freq[i] = alglib::complex(0, 0);
    }

    // Copy the low-frequency components
    int halfOriginal = (Re.length() / 2) + 1; // +1 to include the Nyquist frequency for even lengths
    for (int i = 0; i < halfOriginal; ++i) {
        Re_freq[i] = re[i];
        Im_freq[i] = im[i];
    }

    // Copy the high-frequency components, maintaining symmetry
    for (int i = 1; i < halfOriginal - 1; ++i) { // Skip the first (DC) and last (Nyquist) components
        Re_freq[n_interp - i] = re[re.length() - i];
        Im_freq[n_interp - i] = im[Im.length() - i];
    }

    // Perform inverse FFT on the zero-padded data
    alglib::real_1d_array Re_end, Im_end;
    Re_freq.setlength(n_interp);
    Im_freq.setlength(n_interp);
    alglib::fftr1dinv(Re_freq, Re_end);
    alglib::fftr1dinv(Im_freq, Im_end);
    // printf("%s\n", Re_end.tostring(3).c_str());

    // Convert the results back to vector<complex<double>>
    // Includes scaling
    vector<complex<double>> interpolated_values(n_interp);
    for (int i = 0; i < n_interp; ++i) {
        interpolated_values[i] = complex<double>(Re_end[i]*n_interp/2, Im_end[i]*n_interp/2);
    }

    // Rotate
    rotate(interpolated_values.begin(), interpolated_values.end() - (n_interp) / 2, interpolated_values.end());

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
