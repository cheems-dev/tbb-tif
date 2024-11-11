/*
g++ -o matrix_multiply_pthread matrix_multiply_pthread.cpp -lpthread -std=c++11
./matrix_multiply_pthread 4 4
 */

#include <iostream>
#include <vector>
#include <pthread.h>

struct ThreadData
{
  int start;
  int end;
  int n;
  const std::vector<std::vector<int>> &A;
  const std::vector<std::vector<int>> &B;
  std::vector<std::vector<int>> &C;
};

void *multiply(void *args)
{
  ThreadData *data = static_cast<ThreadData *>(args);
  int n = data->n;
  for (int i = data->start; i < data->end; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      data->C[i][j] = 0;
      for (int k = 0; k < n; ++k)
      {
        data->C[i][j] += data->A[i][k] * data->B[k][j];
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

  std::vector<std::vector<int>> A(n, std::vector<int>(n, 1));
  std::vector<std::vector<int>> B(n, std::vector<int>(n, 1));
  std::vector<std::vector<int>> C(n, std::vector<int>(n, 0));

  std::vector<pthread_t> threads(num_threads);
  std::vector<ThreadData> threadData(num_threads);

  int chunk_size = n / num_threads;

  for (int i = 0; i < num_threads; ++i)
  {
    threadData[i].start = i * chunk_size;
    threadData[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk_size;
    threadData[i].n = n;
    threadData[i].A = A;
    threadData[i].B = B;
    threadData[i].C = C;
    pthread_create(&threads[i], nullptr, multiply, &threadData[i]);
  }

  for (int i = 0; i < num_threads; ++i)
  {
    pthread_join(threads[i], nullptr);
  }

  // Output the result matrix (optional)
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < n; ++j)
    {
      std::cout << C[i][j] << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
