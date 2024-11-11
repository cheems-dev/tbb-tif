/*
Compilacion
g++ -o nqueens_pthread nqueens_pthread.cpp -lpthread -std=c++11
./nqueens_pthread 4 8
*/
#include <iostream>
#include <vector>
#include <pthread.h>
#include <cmath>

struct ThreadData
{
  int start;
  int end;
  int n;
  int *solutions;
};

bool isSafe(const std::vector<int> &board, int row, int col)
{
  for (int i = 0; i < row; i++)
  {
    if (board[i] == col || abs(board[i] - col) == row - i)
    {
      return false;
    }
  }
  return true;
}

void solveNQueens(int row, std::vector<int> &board, int n, int &solution_count)
{
  if (row == n)
  {
    solution_count++;
    return;
  }
  for (int col = 0; col < n; col++)
  {
    if (isSafe(board, row, col))
    {
      board[row] = col;
      solveNQueens(row + 1, board, n, solution_count);
    }
  }
}

void *threadSolve(void *args)
{
  ThreadData *data = static_cast<ThreadData *>(args);
  int solution_count = 0;
  for (int i = data->start; i < data->end; i++)
  {
    std::vector<int> board(data->n, -1);
    board[0] = i;
    solveNQueens(1, board, data->n, solution_count);
  }
  *data->solutions = solution_count;
  return nullptr;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " <num_threads> <n>" << std::endl;
    return 1;
  }

  int num_threads = std::stoi(argv[1]);
  int n = std::stoi(argv[2]);

  int total_solutions = 0;
  std::vector<pthread_t> threads(num_threads);
  std::vector<ThreadData> threadData(num_threads);
  std::vector<int> solutions(num_threads, 0);

  int chunk_size = n / num_threads;

  for (int i = 0; i < num_threads; i++)
  {
    threadData[i].start = i * chunk_size;
    threadData[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk_size;
    threadData[i].n = n;
    threadData[i].solutions = &solutions[i];
    pthread_create(&threads[i], nullptr, threadSolve, &threadData[i]);
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], nullptr);
    total_solutions += solutions[i];
  }

  std::cout << "Total solutions for " << n << "-Queens: " << total_solutions << std::endl;

  return 0;
}
