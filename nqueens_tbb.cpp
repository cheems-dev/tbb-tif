/*
g++ -o nqueens_tbb nqueens_tbb.cpp -ltbb -std=c++11
./nqueens_tbb 8
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <tbb/parallel_for.h>
#include <tbb/atomic.h>

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

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <n>" << std::endl;
    return 1;
  }

  int n = std::stoi(argv[1]);
  tbb::atomic<int> total_solutions = 0;

  tbb::parallel_for(0, n, [&](int i)
                    {
        int solution_count = 0;
        std::vector<int> board(n, -1);
        board[0] = i;
        solveNQueens(1, board, n, solution_count);
        total_solutions += solution_count; });

  std::cout << "Total solutions for " << n << "-Queens: " << total_solutions << std::endl;

  return 0;
}
