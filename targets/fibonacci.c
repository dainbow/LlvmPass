int Fibonacci(const int number);

int main() {
  return Fibonacci(12);
}

int Fibonacci(const int number) {
  if ((number == 1) || (number == 2)) {
    return 1;
  }

  return Fibonacci(number - 1) + Fibonacci(number - 2);
}
