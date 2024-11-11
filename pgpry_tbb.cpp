/*
g++ -o pgpry_tbb pgpry_tbb.cpp -ltbb -std=c++11
./pgpry_tbb
*/

#include <iostream>
#include <tbb/pipeline.h>
#include <tbb/parallel_pipeline.h>

const int NUM_ITEMS = 10; // Número de datos a procesar en el pipeline

int main()
{
  tbb::parallel_pipeline(
      NUM_ITEMS,
      tbb::make_filter<void, int>(tbb::filter::serial, [](tbb::flow_control &fc) -> int
                                  {
            static int item = 0;
            if (item < NUM_ITEMS) {
                std::cout << "Stage 1 produced: " << item << std::endl;
                return item++;
            } else {
                fc.stop();
                return 0;
            } }) &

          tbb::make_filter<int, int>(tbb::filter::parallel, [](int item) -> int
                                     {
            item *= 2; // Procesa el dato
            std::cout << "Stage 2 processed: " << item << std::endl;
            return item; }) &

          tbb::make_filter<int, void>(tbb::filter::parallel, [](int item)
                                      {
            item += 10; // Procesa el dato final
            std::cout << "Stage 3 processed: " << item << std::endl; }));

  return 0;
}
