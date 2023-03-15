#include "../include/INSPECTED_fibonacci.hpp"

extern "C" int main() {
  return Fibonacci(Fibonacci(4));
}

extern "C" uint64_t Fibonacci(const uint64_t number) {
  if ((number == 1) || (number == 2)) {
    return 1;
  }

  return Fibonacci(number - 1) + Fibonacci(number - 2);
}


