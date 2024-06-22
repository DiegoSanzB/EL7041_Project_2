#include <vector>
#include <iostream>

using namespace std;

// 1d arrays
double compute_BER(vector<int> ground_truth, vector<int> data);

// 2d arrays
vector<double> compute_BER(vector<vector<int>> ground_truth, vector<vector<int>> data);

// -- END OF FILE -- //