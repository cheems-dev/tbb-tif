/*
g++ -o blackscholes_tbb blackscholes_tbb.cpp -ltbb -std=c++11
./blackscholes_tbb 10
 */
#include <iostream>
#include <cmath>
#include <tbb/parallel_for.h>
#include <vector>

const double RISK_FREE = 0.02;
const double VOLATILITY = 0.30;

double blackScholesFormula(double S, double K, double T)
{
  double d1 = (log(S / K) + (RISK_FREE + 0.5 * VOLATILITY * VOLATILITY) * T) / (VOLATILITY * sqrt(T));
  double d2 = d1 - VOLATILITY * sqrt(T);
  double callPrice = S * 0.5 * (1 + erf(d1 / sqrt(2))) - K * exp(-RISK_FREE * T) * 0.5 * (1 + erf(d2 / sqrt(2)));
  return callPrice;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <num_prices>" << std::endl;
    return 1;
  }

  int num_prices = std::stoi(argv[1]);

  std::vector<double> prices(num_prices, 50.0); // Precios iniciales (por ejemplo, 50)
  std::vector<double> results(num_prices);

  tbb::parallel_for(0, num_prices, [&](int i)
                    {
                      results[i] = blackScholesFormula(prices[i], 100, 1); // K=100, T=1
                    });

  // Output results
  for (int i = 0; i < num_prices; i++)
  {
    std::cout << "Option price " << i << ": " << results[i] << std::endl;
  }

  return 0;
}
