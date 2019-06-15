// A x B = C

#include <iostream>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <thread>
#include <iomanip>

void parseArguments (int argc, char** argv, int& number_threads, bool& print_time, bool& print_matrix);
void* compute (int from, int to, int n, int m, int p, int q, std::vector <int>& A, std::vector <int>& B, std::vector <int>& C);


class MatrixMultiplication {
  public:
    MatrixMultiplication (int number_threads, bool print_time, bool print_matrix):
      number_threads(number_threads), print_time(print_time), print_matrix(print_matrix) {}

    ~MatrixMultiplication () {
      A.clear();
      B.clear();
      C.clear();
    }

    // Leemos A y lo guardamos en un array unidimensional
    // A[m * i + j] = a[i][j]
    void readA () {
      std::cin >> n >> m;
      int n_elements = n * m;
      A.resize(n_elements);
      for (int i = 0; i < n_elements; i++) std::cin >> A[i];
    }

    // Leemos B y lo guardamos en un array unidimensional como su transpuesta
    // B[j * p + i] = b[i][j]
    void readB () {
      std::cin >> p >> q;
      int n_elements = p * q;
      B.resize(n_elements);
      for (int j = 0; j < p; j++) {
        for (int i = 0; i < q; i++) {
          std::cin >> B[i * p + j];
        }
      }
    }

    void computeC () {
      if (m != p) {
        std::cerr << "No se pueden multiplicar las matrices\n";
        exit (-1);
      }
      n_elements = n * q;
      C.resize(n_elements, 0);
      gettimeofday(&start, 0);
      std::vector <std::thread> arr_thread;
      createThreads(arr_thread);
      for (std::thread& t: arr_thread) t.join();
      gettimeofday(&finish, 0);
      multiplicationTime = (finish.tv_sec - start.tv_sec) * 1000000;
      multiplicationTime = multiplicationTime + (finish.tv_usec - start.tv_usec);
    }

    void printResults () {
      if (print_time) {
        std::cout << std::fixed << std::setprecision(6) <<
          multiplicationTime / 1000000.0 << '\n';
      }
      if (print_matrix) {
        printMatrix(C, n, q);
      }
    }

  private:
    // Command arguments
    int number_threads = -1;
    bool print_time = false;
    bool print_matrix = true;
    // Matrices
    int n, m;
    std::vector <int> A; // n x m
    int p, q;
    std::vector <int> B; // p x q
    int n_elements;
    std::vector <int> C; // n x q si [m = p]
    // Chronometer
    struct timeval start;
    struct timeval finish;
    long long multiplicationTime;

    void printMatrix (std::vector <int>& mat, int n, int m) {
      std::cout << n << ' ' << m << '\n';
      for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
          std::cout << mat[i * m + j] << ' ';
        }
        puts("");
      }
    }

    void createThreads (std::vector <std::thread>& arr_thread) {
      int gap = n_elements / number_threads;
      if (gap == 0) gap = 1;
      int from = 0;
      int t;
      for (t = 0; t < number_threads; t++) {
        int to = from + gap;
        if (to >= n_elements) to = n_elements;
        if (t == number_threads - 1) to = n_elements;
        arr_thread.push_back(std::thread(compute, from, to, 
                                         n, m, p, q,
                                         std::ref(A), std::ref(B), std::ref(C)));
        from = to;
      }
    }
};


int main (int argc, char** argv) {
  int number_threads = -1;
  bool print_time = false;
  bool print_matrix = true;
  parseArguments(argc, argv, number_threads, print_time, print_matrix);
  MatrixMultiplication* multiplication = new MatrixMultiplication(number_threads, 
                                                                  print_time,
                                                                  print_matrix);
  multiplication -> readA();
  multiplication -> readB();
  multiplication -> computeC();
  multiplication -> printResults();
  delete multiplication;
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
        if (optopt == 'n') {
          std::cerr << "Debes indicar el número de hilos\n";
        } else {
          std::cerr << "Opción desconocida\n";
        }
        exit(-1);
    }
  }
  if (take_all_threads) {
    number_threads = std::thread::hardware_concurrency();
  }
  if (number_threads <= 0) {
    std::cerr << "Numero de hilos invalido\n";
    exit (-1);
  }
}

// Calculamos C[from:to)
void* compute (int from, int to, int n, int m, int p, int q, std::vector <int>& A, std::vector <int>& B, std::vector <int>& C) {
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

