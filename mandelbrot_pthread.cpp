/*
g++ -o mandelbrot_pthread mandelbrot_pthread.cpp -lpthread -std=c++11
./mandelbrot_pthread 4 800 800 mandelbrot_output.txt
 */
#include <iostream>
#include <vector>
#include <complex>
#include <pthread.h>

const int MAX_ITER = 1000;

struct ThreadData
{
  int start;
  int end;
  int width;
  int height;
  std::vector<std::vector<int>> &image;
  double min_x;
  double max_x;
  double min_y;
  double max_y;
};

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

void *computeMandelbrot(void *args)
{
  ThreadData *data = static_cast<ThreadData *>(args);
  for (int y = data->start; y < data->end; ++y)
  {
    for (int x = 0; x < data->width; ++x)
    {
      double real = data->min_x + (data->max_x - data->min_x) * x / data->width;
      double imag = data->min_y + (data->max_y - data->min_y) * y / data->height;
      std::complex<double> c(real, imag);
      data->image[y][x] = mandelbrot(c);
    }
  }
  return nullptr;
}

int main(int argc, char *argv[])
{
  if (argc < 5)
  {
    std::cerr << "Usage: " << argv[0] << " <num_threads> <width> <height> <output_file>" << std::endl;
    return 1;
  }

  int num_threads = std::stoi(argv[1]);
  int width = std::stoi(argv[2]);
  int height = std::stoi(argv[3]);
  std::string output_file = argv[4];

  std::vector<std::vector<int>> image(height, std::vector<int>(width, 0));

  std::vector<pthread_t> threads(num_threads);
  std::vector<ThreadData> threadData(num_threads);

  int chunk_size = height / num_threads;
  double min_x = -2.0, max_x = 1.0, min_y = -1.5, max_y = 1.5;

  for (int i = 0; i < num_threads; ++i)
  {
    threadData[i].start = i * chunk_size;
    threadData[i].end = (i == num_threads - 1) ? height : (i + 1) * chunk_size;
    threadData[i].width = width;
    threadData[i].height = height;
    threadData[i].image = image;
    threadData[i].min_x = min_x;
    threadData[i].max_x = max_x;
    threadData[i].min_y = min_y;
    threadData[i].max_y = max_y;
    pthread_create(&threads[i], nullptr, computeMandelbrot, &threadData[i]);
  }

  for (int i = 0; i < num_threads; ++i)
  {
    pthread_join(threads[i], nullptr);
  }

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
