#include "data_manager.hpp"

void write_to_csv(const vector<valarray<double>>& data, const vector<string>& column_names, string CSV_FILENAME = "results.csv") {
    string csv_file_path = RESULTS_FOLDER_PATH.generic_string() + "/" + CSV_FILENAME;
    ofstream output_file(csv_file_path);

    if (!output_file.is_open()) {
        cerr << "Error: Unable to open CSV file." << endl;
        return;
    }

    // Write headers
    for (const string& column_name : column_names) {
        output_file << column_name << ",";
    }
    output_file << endl;

    // Write data
    size_t num_rows = data[0].size();
    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < data.size(); ++j) {
            output_file << data[j][i] << ",";
        }
        output_file << endl;
    }

    output_file.close();
}

// -- END OF FILE -- //