/*
g++ -o cholesky_tbb cholesky_tbb.cpp -ltbb -std=c++11
./cholesky_tbb 4
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <tbb/parallel_pipeline.h>
#include <tbb/flow_control.h>

struct MatrixData {
    std::vector<std::vector<double>>& matrix;
    int n;
};

bool choleskyDecompositionPipeline(MatrixData& data)
{
    for (int i = 0; i < data.n; ++i)
    {
        // Calcula el elemento diagonal
        double sum = data.matrix[i][i];
        for (int k = 0; k < i; ++k)
        {
            sum -= data.matrix[i][k] * data.matrix[i][k];
        }

        if (sum <= 0)
        {
            std::cerr << "Matrix is not positive definite" << std::endl;
            return false;
        }

        data.matrix[i][i] = std::sqrt(sum);

        // Etapa de pipeline para calcular las columnas
        tbb::parallel_pipeline(
            /*num_items=*/data.n - i - 1,
            tbb::make_filter<void, int>(tbb::filter::serial, [&](tbb::flow_control& fc) -> int {
                static int j = i + 1;
                if (j >= data.n) {
                    fc.stop();
                    return 0;
                }
                return j++;
            }) &
            tbb::make_filter<int, void>(tbb::filter::parallel, [&](int j) {
                double sum = data.matrix[j][i];
                for (int k = 0; k < i; ++k) {
                    sum -= data.matrix[j][k] * data.matrix[i][k];
                }
                data.matrix[j][i] = sum / data.matrix[i][i];
            })
        );
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
        matrix[i][i] += n; // Asegura que la matriz sea definida positiva

    MatrixData data{matrix, n};
    if (!choleskyDecompositionPipeline(data))
    {
        return 1;
    }

    // Imprimir la matriz triangular inferior resultante
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

