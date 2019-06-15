// A x B = C

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int n, m, p, q;
int* A = nullptr; // n x m
int* B = nullptr; // p x q
int* C = nullptr; // n x q if [m = p]

struct Parameters {
  int from, to;
};

void printMatrix (int* mat, int n, int m);
void readA ();
void readB ();
void dotProduct (int* x, int* y, int pos);
void* compute (void* parameters);

int main (int argc, char* argv[]) {
  if (argc == 1) {
    std::cerr << "Debes ingresar como argumento el número de hilos\n";
    return (-1);
  }
  int number_threads = atoi(argv[1]);
  if (number_threads <= 0) {
    std::cerr << "Numero de hilos invalido\n";
    return (-1);
  }
  int print_time = 1;
  if (argc >= 3) print_time = atoi(argv[2]);
  int print_matrix = 1;
  if (argc >= 4) print_matrix = atoi(argv[3]);
  readA();
  readB();
  if (m != p) {
    std::cerr << "No se pueden multiplicar las matrices\n";
    return (-1);
  }
  int n_elements = n * q;
  C = new int[n_elements];
  int gap = n_elements / number_threads;
  if (gap == 0) gap = 1;
  int from = 0;
  int t;
  // Consideremos solo el tiempo de ejecutar la multiplicacion y no los tiempos
  // de lectura
  struct timeval start;
  struct timeval finish;
  long long multiplicationTime;
  gettimeofday(&start, 0);
  pthread_t thread[number_threads];
  struct Parameters parameters[number_threads];
  for (t = 0; t < number_threads; t++) {
    int to = from + gap;
    if (to >= n_elements) to = n_elements;
    if (t == number_threads - 1) to = n_elements;
    parameters[t].from = from;
    parameters[t].to = to;
    int rc = pthread_create(&thread[t], NULL, compute, (void*)&parameters[t]);
    if (rc) {
      std::cerr << "ERROR código " << rc;
      exit(-1);
    }
    from = to;
  }
  for (t = 0; t < number_threads; t++) {
    int rc = pthread_join(thread[t], NULL);
    if (rc) {
      std::cerr << "ERROR código " << rc;
      exit(-1);
    }
  }
  gettimeofday(&finish, 0);
  multiplicationTime = (finish.tv_sec - start.tv_sec) * 1000000;
  multiplicationTime = multiplicationTime + (finish.tv_usec - start.tv_usec);
  if (print_time) printf("%.6f\n", multiplicationTime / 1000000.0);
  if (print_matrix) printMatrix(C, n, q);
  delete A;
  delete B;
  delete C;
  return (0);
}

void printMatrix (int* mat, int n, int m) {
  std::cout << n << ' ' << m << '\n';
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      std::cout << mat[i * m + j] << ' ';
    }
    puts("");
  }
}

// Leemos A y lo guardamos en un array unidimensional
// A[m * i + j] = a[i][j]
void readA () {
  std::cin >> n >> m;
  int n_elements = n * m;
  A = new int[n_elements];
  for (int i = 0; i < n_elements; i++) std::cin >> A[i];
}

// Leemos B y lo guardamos en un array unidimensional como su transpuesta
// B[j * p + i] = b[i][j]
void readB () {
  std::cin >> p >> q;
  int n_elements = p * q;
  B = new int[n_elements];
  for (int j = 0; j < p; j++) {
    for (int i = 0; i < q; i++) {
      std::cin >> B[i * p + j];
    }
  }
}

// Calculalos C[from:to)
void* compute (void* parameters) {
  struct Parameters* this_parameters = (struct Parameters*) parameters;
  int from = this_parameters -> from;
  int to = this_parameters -> to;
  if (n * q <= from) {
    pthread_exit(NULL);
  }
  int row = (from / q) * m;
  int col = (from % q) * p;
  for (int pos = from; pos < to; pos++) {
    int* x = A + row;
    int* y = B + col;
    C[pos] = 0;
    for (int k = 0; k < m; k++) C[pos] += x[k] * y[k];
    col += p;
    if (col == p * q) {
      col = 0;
      row += m;
    }
  }
  pthread_exit(NULL);
}

