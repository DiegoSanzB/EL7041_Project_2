#ifndef DATA_MANAGER
#define DATA_MANAGER

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>



using namespace std;
const filesystem::path mainFilePath = filesystem::current_path();
const filesystem::path RESULTS_FOLDER_PATH = mainFilePath.parent_path() / "results";

void write_to_csv(const vector<vector<double>> &data, const vector<string> &column_names, string CSV_FILENAME);
void add_to_csv(const vector<vector<double>>& data, const vector<string>& column_names, string CSV_FILENAME);

void append_to_csv(const vector<double>& data, string CSV_FILENAME);

// type overloading (TODO: add_to_csv())
void write_to_csv(const vector<vector<int>> &data, const vector<string> &column_names, string CSV_FILENAME);
void add_to_csv(const vector<vector<int>>& data, const vector<string>& column_names, string CSV_FILENAME);

#endif
// -- END OF FILE -- //