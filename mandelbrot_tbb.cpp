/*
g++ -o mandelbrot_tbb mandelbrot_tbb.cpp -ltbb -std=c++11
./mandelbrot_tbb 800 800 mandelbrot_output.txt
*/
#include <iostream>
#include <vector>
#include <complex>
#include <tbb/parallel_for.h>
#include <fstream>

const int MAX_ITER = 1000;

int mandelbrot(const std::complex<double> &c)
{
  std::complex<double> z = 0;
  int iter = 0;
  while (std::abs(z) <= 2.0 && iter < MAX_ITER)
  {
    z = z * z + c;
    iter++;
  }
  return iter;
}

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " <width> <height> <output_file>" << std::endl;
    return 1;
  }

  int width = std::stoi(argv[1]);
  int height = std::stoi(argv[2]);
  std::string output_file = argv[3];

  std::vector<std::vector<int>> image(height, std::vector<int>(width, 0));

  double min_x = -2.0, max_x = 1.0, min_y = -1.5, max_y = 1.5;

  tbb::parallel_for(0, height, [&](int y)
                    {
        for (int x = 0; x < width; ++x) {
            double real = min_x + (max_x - min_x) * x / width;
            double imag = min_y + (max_y - min_y) * y / height;
            std::complex<double> c(real, imag);
            image[y][x] = mandelbrot(c);
        } });

  // Output results to the file
  std::ofstream out(output_file);
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      out << image[y][x] << " ";
    }
    out << "\n";
  }
  out.close();

  return 0;
}
