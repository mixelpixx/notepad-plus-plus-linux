#include <iostream>
#include <vector>
#include <string>

// Sample C++ code to test syntax highlighting
class Calculator {
private:
    std::vector<double> history;
    
public:
    Calculator() : history() {}
    
    double add(double a, double b) {
        double result = a + b;
        history.push_back(result);
        return result;
    }
    
    double multiply(double a, double b) {
        double result = a * b;
        history.push_back(result);
        return result;
    }
    
    void printHistory() const {
        std::cout << "Calculation history:" << std::endl;
        for (size_t i = 0; i < history.size(); ++i) {
            std::cout << "  " << i + 1 << ": " << history[i] << std::endl;
        }
    }
};

int main() {
    Calculator calc;
    
    // Test calculations
    double sum = calc.add(5.5, 3.2);
    double product = calc.multiply(4.0, 2.5);
    
    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Product: " << product << std::endl;
    
    calc.printHistory();
    
    return 0;
}

/* 
 * Multi-line comment
 * This file demonstrates:
 * - Classes and methods
 * - STL containers
 * - Control structures
 * - String literals
 * - Numbers and operators
 */