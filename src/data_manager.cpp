#include "data_manager.hpp"

void write_to_csv(const vector<vector<double>>& data, const vector<string>& column_names, string CSV_FILENAME = "results.csv") {
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

void add_to_csv(const vector<vector<double>>& data, const vector<string>& column_names, string CSV_FILENAME = "results.csv") {
    string csv_file_path = RESULTS_FOLDER_PATH.generic_string() + "/" + CSV_FILENAME;
    string temp_file_path = RESULTS_FOLDER_PATH.generic_string() + "/temp.csv";
    ifstream input_file(csv_file_path);
    ofstream temp_file(temp_file_path);

    // Check if the file opened successfully
    if (!input_file.is_open()) {
        cerr << "Error: Unable to open input CSV file." << endl;
        return;
    }
    // Read and copy existing headers
    string headers;
    getline(input_file, headers);
    temp_file << headers;
    // Write new headers
    for (const string& column_name : column_names) {
        temp_file << column_name << ",";
        // cout << column_name << ",";
    }
    temp_file << endl;


    // Close the original file and open it again for data appending
    input_file.close();
    input_file.open(csv_file_path);

    // Skip the first line (headers)
    string line;
    getline(input_file, line);

    // Write existing data and new data
    size_t num_rows = data[0].size();
    for (size_t i = 0; i < num_rows; ++i) {
        // Write existing data first
        if (getline(input_file, line)) {
            temp_file << line;
        }

        // Write new data
        for (size_t j = 0; j < data.size(); ++j) {
            // temp_file << ",";
            temp_file << data[j][i] << ",";
            // If this is the first row, print
            // if ((i == 0) and (j == 1)) cout << data[j][i] << ",";
        }
        temp_file << endl;
    }

    // Close files
    input_file.close();
    temp_file.close();

    // Replace original file with temporary file
    remove(csv_file_path.c_str());
    rename(temp_file_path.c_str(), csv_file_path.c_str());
}


void append_to_csv(const vector<double>& data, string CSV_FILENAME = "results.csv") {
    string csv_file_path = RESULTS_FOLDER_PATH.generic_string() + "/" + CSV_FILENAME;
    ofstream output_file(csv_file_path, ios::app);

    // Check if the file opened successfully
    if (!output_file.is_open()) {
        cerr << "Error: Unable to open output CSV file." << endl;
        return;
    }
    // output_file << endl;
    // Write new rows
    // size_t num_rows = data[0].size();
    // for (size_t i = 0; i < num_rows; ++i) {
    for (size_t j = 0; j < data.size(); ++j) {
        output_file << data[j] << ",";
    }
    output_file << endl;
    

    // Close file
    output_file.close();
}

// void add_row_to_csv(const vector<vector<double>>& data, string CSV_FILENAME = "results.csv") {
//     string csv_file_path = RESULTS_FOLDER_PATH.generic_string() + "/" + CSV_FILENAME;
//     // string temp_file_path = RESULTS_FOLDER_PATH.generic_string() + "/temp.csv";
//     ifstream input_file(csv_file_path);
//     // ofstream temp_file(temp_file_path);

//     // Check if the file opened successfully
//     if (!input_file.is_open()) {
//         cerr << "Error: Unable to open input CSV file." << endl;
//         return;
//     }
//     // Read and copy existing headers
//     // string headers;
//     // getline(input_file, headers);
//     // temp_file << headers;
//     // // Write new headers
//     // for (const string& column_name : column_names) {
//     //     temp_file << column_name << ",";
//     //     // cout << column_name << ",";
//     // }
//     // temp_file << endl;


//     // Close the original file and open it again for data appending
//     // input_file.close();
//     input_file.open(csv_file_path, ios::app);

//     // Skip the first line (headers)
//     string line;
//     // getline(input_file, line);

//     // Write existing data and new data
//     size_t num_rows = data[0].size();
//     for (size_t i = 0; i < num_rows; ++i) {
//         // Write existing data first
//         // if (getline(input_file, line)) {
//         //     temp_file << line;
//         // }

//         // Write new data
//         for (size_t j = 0; j < data.size(); ++j) {
//             // temp_file << ",";
//             input_file << data[j][i] << ",";
//             // If this is the first row, print
//             // if ((i == 0) and (j == 1)) cout << data[j][i] << ",";
//         }
//         input_file << endl;
//     }

//     // Close files
//     input_file.close();
//     // temp_file.close();

//     // Replace original file with temporary file
//     // remove(csv_file_path.c_str());
//     // rename(temp_file_path.c_str(), csv_file_path.c_str());
// }


void write_to_csv(const vector<vector<int>> &data, const vector<string> &column_names, string CSV_FILENAME) {
    vector<vector<double>> data_double;
    
    for (size_t i = 0; i < data.size(); i++) {
        data_double.push_back({});
        for (size_t j = 0; j < data[0].size(); j++) {
            double d = (double) data[i][j];
            data_double[i].push_back(d);
        }
    }
    write_to_csv(data_double, column_names, CSV_FILENAME);
}


// -- END OF FILE -- //