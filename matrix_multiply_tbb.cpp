/*
g++ -o matrix_multiply_tbb matrix_multiply_tbb.cpp -ltbb -std=c++11
./matrix_multiply_tbb 4
 */
#include <iostream>
#include <vector>
#include <tbb/parallel_for.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <matrix_size>" << std::endl;
    return 1;
  }

  int n = std::stoi(argv[1]);

  std::vector<std::vector<int>> A(n, std::vector<int>(n, 1));
  std::vector<std::vector<int>> B(n, std::vector<int>(n, 1));
  std::vector<std::vector<int>> C(n, std::vector<int>(n, 0));

  tbb::parallel_for(0, n, [&](int i)
                    {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        } });

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
