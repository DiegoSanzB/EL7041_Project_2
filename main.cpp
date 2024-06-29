#include <questions.hpp>

using namespace std;

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    constellations(gen);
    return 0;
}

// -- END OF FILE -- // 