#include <iostream>
#include <fstream>
#include <vector>
#include "libalglib/interpolation.h"
#include "libalglib/ap.h"
#include "libalglib/fasttransforms.h"

// Function to write data to a file
void writeDataToFile(const std::string& filename, const std::vector<double>& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (size_t i = 0; i < data.size(); ++i) {
            file << i << " " << data[i] << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

int main() {
    // Original data size
    const int N = 8;
    std::vector<double> original_data(N);

    // Fill original data with a simple example signal
    for (int i = 0; i < N; ++i) {
        original_data[i] = i;
    }

    // Convert original data to alglib::complex_1d_array
    alglib::complex_1d_array original_data_complex;
    original_data_complex.setlength(N);
    for (int i = 0; i < N; ++i) {
        original_data_complex[i] = alglib::complex(original_data[i], 0.0);
    }

    // Perform FFT
    alglib::fftc1d(original_data_complex);

    // Zero-padding to interpolate (increasing the frequency resolution)
    const int M = 16; // Interpolated size (must be >= N)
    alglib::complex_1d_array padded_fft_result;
    padded_fft_result.setlength(M);

    // Copy original FFT results and zero-pad the rest
    int halfN = N / 2;
    for (int i = 0; i < halfN; ++i) {
        padded_fft_result[i] = original_data_complex[i];
    }
    for (int i = halfN; i < M - halfN; ++i) {
        padded_fft_result[i] = alglib::complex(0.0, 0.0);
    }
    for (int i = M - halfN; i < M; ++i) {
        padded_fft_result[i] = original_data_complex[i - (M - N)];
    }

    // Perform inverse FFT
    alglib::fftc1dinv(padded_fft_result);

    // Normalize the output of the inverse FFT
    std::vector<double> interpolated_data(M);
    for (int i = 0; i < M; ++i) {
        interpolated_data[i] = padded_fft_result[i].x / N;
    }

    // Output interpolated data to a file
    writeDataToFile("interpolated_data.txt", interpolated_data);

    // Plotting using gnuplot
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    fprintf(gnuplotPipe, "set title 'Interpolated Data'\n");
    fprintf(gnuplotPipe, "plot 'interpolated_data.txt' with lines\n");
    fclose(gnuplotPipe);

    return 0;
}
