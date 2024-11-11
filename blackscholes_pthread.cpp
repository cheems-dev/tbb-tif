/*
g++ -o blackscholes_pthread blackscholes_pthread.cpp -lpthread -std=c++11
./blackscholes_pthread 4 10
 */
#include <iostream>
#include <cmath>
#include <pthread.h>
#include <vector>

struct ThreadData
{
  int start;
  int end;
  double *prices;
  double *results;
};

const double RISK_FREE = 0.02;
const double VOLATILITY = 0.30;

double blackScholesFormula(double S, double K, double T)
{
  double d1 = (log(S / K) + (RISK_FREE + 0.5 * VOLATILITY * VOLATILITY) * T) / (VOLATILITY * sqrt(T));
  double d2 = d1 - VOLATILITY * sqrt(T);
  double callPrice = S * 0.5 * (1 + erf(d1 / sqrt(2))) - K * exp(-RISK_FREE * T) * 0.5 * (1 + erf(d2 / sqrt(2)));
  return callPrice;
}

void *computePrices(void *args)
{
  ThreadData *data = static_cast<ThreadData *>(args);
  for (int i = data->start; i < data->end; i++)
  {
    data->results[i] = blackScholesFormula(data->prices[i], 100, 1); // K=100, T=1
  }
  return nullptr;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " <num_threads> <num_prices>" << std::endl;
    return 1;
  }

  int num_threads = std::stoi(argv[1]);
  int num_prices = std::stoi(argv[2]);

  std::vector<double> prices(num_prices, 50.0); // Precios iniciales (por ejemplo, 50)
  std::vector<double> results(num_prices);

  std::vector<pthread_t> threads(num_threads);
  std::vector<ThreadData> threadData(num_threads);

  int chunk_size = num_prices / num_threads;

  for (int i = 0; i < num_threads; i++)
  {
    threadData[i].start = i * chunk_size;
    threadData[i].end = (i == num_threads - 1) ? num_prices : (i + 1) * chunk_size;
    threadData[i].prices = prices.data();
    threadData[i].results = results.data();
    pthread_create(&threads[i], nullptr, computePrices, &threadData[i]);
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], nullptr);
  }

  // Output results
  for (int i = 0; i < num_prices; i++)
  {
    std::cout << "Option price " << i << ": " << results[i] << std::endl;
  }

  return 0;
}
