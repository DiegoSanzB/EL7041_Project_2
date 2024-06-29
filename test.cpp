#include <iostream>
#include <vector>
#include <complex>
#include <tuple>

using namespace std;

tuple<vector<complex<double>>, vector<complex<double>>> remove_pilot_symbols(const vector<complex<double>>& data, int pilot_spacing) {
    // Get size of data
    int size = data.size();
    
    // Estimate the sizes of the vectors to reserve space (optional, but improves performance)
    int estimated_pilot_count = size / (pilot_spacing + 1);
    int estimated_data_count = size - estimated_pilot_count;
    
    vector<complex<double>> data_without_pilots;
    data_without_pilots.reserve(estimated_data_count);

    vector<complex<double>> pilots;
    pilots.reserve(estimated_pilot_count);

    // Remove pilot symbols
    for (int i = 0; i < size; i++) {
        if (i % (pilot_spacing + 1) != 0) {
            data_without_pilots.push_back(data[i]);
        } else {
            pilots.push_back(data[i]);
        }
    }
    
    return make_tuple(data_without_pilots, pilots);
}

int main() {
    // Example usage
    vector<complex<double>> data = { {1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}, {4.0, 4.0}, {5.0, 5.0}, {6.0, 6.0} };
    int pilot_spacing = 1; // Example spacing
    
    auto result = remove_pilot_symbols(data, pilot_spacing);
    
    vector<complex<double>> data_without_pilots = get<0>(result);
    vector<complex<double>> pilots = get<1>(result);
    
    // Print the results
    cout << "Data without pilots:" << endl;
    for (const auto& d : data_without_pilots) {
        cout << d << endl;
    }
    
    cout << "Pilots:" << endl;
    for (const auto& p : pilots) {
        cout << p << endl;
    }
    
    return 0;
}