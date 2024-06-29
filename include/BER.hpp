#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 1d arrays
double compute_BER(vector<int> ground_truth, vector<int> data, int modulation_order);

// 2d arrays
vector<double> compute_BER(vector<vector<int>> ground_truth, vector<vector<int>> data, int modulation_order);

// -- END OF FILE -- //