#include "../header/petcare.h"
#include <stdexcept>

using namespace Coruh::petcare;

double petcare::add(double a, double b) {
    return a + b;
}

double petcare::subtract(double a, double b) {
    return a - b;
}

double petcare::multiply(double a, double b) {
    return a * b;
}

double petcare::divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero is not allowed.");
    }
    return a / b;
}