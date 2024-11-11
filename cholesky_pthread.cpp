/*
g++ -o cholesky_pthread cholesky_pthread.cpp -lpthread -std=c++11
./cholesky_pthread 4 4
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>

struct ThreadData
{
  int start;
  int end;
  int n;
  std::vector<std::vector<double>> &matrix;
};

void *choleskyDecomposition(void *args)
{
  ThreadData *data = static_cast<ThreadData *>(args);
  int n = data->n;

  for (int i = data->start; i < data->end; ++i)
  {
    for (int j = 0; j <= i; ++j)
    {
      double sum = data->matrix[i][j];

      for (int k = 0; k < j; ++k)
      {
        sum -= data->matrix[i][k] * data->matrix[j][k];
      }

      if (i == j)
      {
        if (sum <= 0)
        {
          std::cerr << "Matrix is not positive definite" << std::endl;
          pthread_exit(nullptr);
        }
        data->matrix[i][j] = std::sqrt(sum);
      }
      else
      {
        data->matrix[i][j] = sum / data->matrix[j][j];
      }
    }
  }

  return nullptr;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " <num_threads> <matrix_size>" << std::endl;
    return 1;
  }

  int num_threads = std::stoi(argv[1]);
  int n = std::stoi(argv[2]);

  std::vector<std::vector<double>> matrix(n, std::vector<double>(n, 1.0));
  for (int i = 0; i < n; ++i)
    matrix[i][i] += n; // Ensure positive definiteness

  std::vector<pthread_t> threads(num_threads);
  std::vector<ThreadData> threadData(num_threads);

  int chunk_size = n / num_threads;

  for (int i = 0; i < num_threads; ++i)
  {
    threadData[i].start = i * chunk_size;
    threadData[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk_size;
    threadData[i].n = n;
    threadData[i].matrix = matrix;
    pthread_create(&threads[i], nullptr, choleskyDecomposition, &threadData[i]);
  }

  for (int i = 0; i < num_threads; ++i)
  {
    pthread_join(threads[i], nullptr);
  }

  // Output the resulting lower triangular matrix
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j <= i; ++j)
    {
      std::cout << matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
