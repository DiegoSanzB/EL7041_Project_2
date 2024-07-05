#include <questions.hpp>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Create constellations for scenario
    constellations(gen);

    // Run BER scenarios
    vector<thread> threads;
    mutex gen_mutex;

    // Launch two threads with different values of i
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([gen, i]() mutable {
            compute_ber_data(gen, i);
        });
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

// -- END OF FILE -- // 