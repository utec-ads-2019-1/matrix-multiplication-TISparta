// A x B = C

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <thread>

int n, m, p, q;
std::vector <int> A; // n x m
std::vector <int> B; // p x q
std::vector <int> C; // n x q if [m = p]

void parseArguments (int argc, char** argv, int& number_threads, bool& print_time, bool& print_matrix);
void readA (std::vector <int>& A);
void readB (std::vector <int>& B);
void createThreads (int n_elements, int n_threads, std::vector <std::thread>& arr_thread);
void dotProduct (int* x, int* y, int pos);
void* compute (int from, int to);
void printMatrix (std::vector <int>& mat, int n, int m);

int main (int argc, char** argv) {
  int number_threads = -1;
  bool print_time = false;
  bool print_matrix = true;
  parseArguments(argc, argv, number_threads, print_time, print_matrix);
  readA(A);
  readB(B);
  if (m != p) {
    std::cerr << "No se pueden multiplicar las matrices\n";
    return (-1);
  }
  int n_elements = n * q;
  C.resize(n_elements, 0);
  struct timeval start;
  struct timeval finish;
  long long multiplicationTime;
  gettimeofday(&start, 0);
  std::vector <std::thread> arr_thread;

  createThreads(n_elements, number_threads,arr_thread);
  for (std::thread& t: arr_thread) t.join();

  gettimeofday(&finish, 0);
  multiplicationTime = (finish.tv_sec - start.tv_sec) * 1000000;
  multiplicationTime = multiplicationTime + (finish.tv_usec - start.tv_usec);
  if (print_time) printf("%.6f\n", multiplicationTime / 1000000.0);
  if (print_matrix) printMatrix(C, n, q);
  A.clear();
  B.clear();
  C.clear();
  return (0);
}

void parseArguments (int argc, char** argv, int& number_threads, bool& print_time, bool& print_matrix) {
  char option;
  bool take_all_threads = false;
  while ((option = getopt (argc, argv, "ahtmn:")) != -1) {
    switch (option) {
      case 't':
        print_time = true;
        break;
      case 'm':
        print_matrix = false;
        break;
      case 'n':
        number_threads = atoi(optarg);
        break;
      case 'a':
        take_all_threads = true;
        break;
      case 'h':
        std::cout << "Options:\n";
        printf("  %-30s %s\n", "-n <numero_hilos>", "indica el número de hilos");
        printf("  %-30s %s\n", "-m", "no imprimir la matrix resultante");
        printf("  %-30s %s\n", "-t", "imprimir el tiempo que demoró la multiplicación");
        printf("  %-30s %s\n", "-a", "ejecutar el programa usando todos los hilos de la máquina");
        exit(0);
      case '?':
        if (optopt == 'c') {
          std::cerr << "Debes indicar el número de hilos\n";
        } else {
          std::cerr << "Opción desconocida\n";
        }
        exit(-1);
    }
  }
  if (take_all_threads) {
    number_threads = std::thread::hardware_concurrency();
    std::cout << number_threads << std::endl;
  }
  if (number_threads <= 0) {
    std::cerr << "Numero de hilos invalido\n";
    exit (-1);
  }
}


// Leemos A y lo guardamos en un array unidimensional
// A[m * i + j] = a[i][j]
void readA (std::vector <int>& A) {
  std::cin >> n >> m;
  int n_elements = n * m;
  A.resize(n_elements);
  for (int i = 0; i < n_elements; i++) std::cin >> A[i];
}

// Leemos B y lo guardamos en un array unidimensional como su transpuesta
// B[j * p + i] = b[i][j]
void readB (std::vector <int>& B) {
  std::cin >> p >> q;
  int n_elements = p * q;
  B.resize(n_elements);
  for (int j = 0; j < p; j++) {
    for (int i = 0; i < q; i++) {
      std::cin >> B[i * p + j];
    }
  }
}

void createThreads (int n_elements, int n_threads, std::vector <std::thread>& arr_thread) {
  int gap = n_elements / n_threads;
  if (gap == 0) gap = 1; 
  int from = 0;
  int t;
  // Consideremos solo el tiempo de ejecutar la multiplicacion y no los tiempos
  // de lectura
  for (t = 0; t < n_threads; t++) {
    int to = from + gap;
    if (to >= n_elements) to = n_elements;
    if (t == n_threads - 1) to = n_elements;
    arr_thread.push_back(std::thread(compute, from, to));
    from = to;
  }
}

// Calculamos C[from:to)
void* compute (int from, int to) {
  if (n * q <= from) {
    pthread_exit(NULL);
  }
  int row = (from / q) * m;
  int col = (from % q) * p;
  for (int pos = from; pos < to; pos++) {
    C[pos] = 0;
    for (int k = 0; k < m; k++) C[pos] += A[row + k] * B[col + k];
    col += p;
    if (col == p * q) {
      col = 0;
      row += m;
    }
  }
  pthread_exit(NULL);
}

void printMatrix (std::vector <int>& mat, int n, int m) {
  std::cout << n << ' ' << m << '\n';
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      std::cout << mat[i * m + j] << ' ';
    }
    puts("");
  }
}
