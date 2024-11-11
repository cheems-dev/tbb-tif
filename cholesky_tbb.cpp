/*
g++ -o cholesky_tbb cholesky_tbb.cpp -ltbb -std=c++11
./cholesky_tbb 4
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <tbb/parallel_for.h>

bool choleskyDecomposition(std::vector<std::vector<double>> &matrix, int n)
{
  for (int i = 0; i < n; ++i)
  {
    double sum = matrix[i][i];

    for (int k = 0; k < i; ++k)
    {
      sum -= matrix[i][k] * matrix[i][k];
    }

    if (sum <= 0)
    {
      std::cerr << "Matrix is not positive definite" << std::endl;
      return false;
    }

    matrix[i][i] = std::sqrt(sum);

    tbb::parallel_for(i + 1, n, [&](int j)
                      {
            double sum = matrix[j][i];
            for (int k = 0; k < i; ++k) {
                sum -= matrix[j][k] * matrix[i][k];
            }
            matrix[j][i] = sum / matrix[i][i]; });
  }
  return true;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <matrix_size>" << std::endl;
    return 1;
  }

  int n = std::stoi(argv[1]);

  std::vector<std::vector<double>> matrix(n, std::vector<double>(n, 1.0));
  for (int i = 0; i < n; ++i)
    matrix[i][i] += n; // Ensure positive definiteness

  if (!choleskyDecomposition(matrix, n))
  {
    return 1;
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
