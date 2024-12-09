#include "utility.h"
void print_vector(const vector<float>& vec) {
    std::cout << "[";
    for (size_t i = vec.size()-1; i >vec.size()-11; i--) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", "; // Add a comma between elements
        }
    }
    std::cout << "]" << std::endl;
}