#include <iostream>
#include <stdlib.h>
#include <time.h>

const int MIN_VAL = -100;
const int MAX_VAL =  100;

int getRandomNumber (int from, int to) {
  return from + (rand() % (to - from + 1));
}

void printRandomMatrix (int n, int m) {
  std::cout << n << ' ' << m << '\n';
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      std::cout << getRandomNumber(MIN_VAL, MAX_VAL) << ' ';
    }
    std::cout << '\n';
  }
}

int main (int argc, char* argv[]) {
  srand(time(0));
  if (argc == 1) {
    puts("Debes ingresar como argumento la dimensión de las matrices");
    return (-1);
  }
  int N = atoi(argv[1]);
  if (N <= 0) {
    puts("La dimension debe ser un entero positivo");
    return (-1);
  }
  printRandomMatrix(N, N);
  printRandomMatrix(N, N);
  return (0);
}
