/*
g++ -o pgpry_pthread pgpry_pthread.cpp -lpthread -std=c++11
./pgpry_pthread
*/

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <condition_variable>
#include <mutex>

const int NUM_ITEMS = 10; // Número de datos a procesar en el pipeline

struct Buffer
{
  std::queue<int> data;
  std::mutex mtx;
  std::condition_variable cv;

  void push(int item)
  {
    std::unique_lock<std::mutex> lock(mtx);
    data.push(item);
    cv.notify_one();
  }

  int pop()
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]
            { return !data.empty(); });
    int item = data.front();
    data.pop();
    return item;
  }
};

Buffer buffer1, buffer2;

void *stage1(void *args)
{
  for (int i = 0; i < NUM_ITEMS; ++i)
  {
    int item = i; // Genera el dato
    std::cout << "Stage 1 produced: " << item << std::endl;
    buffer1.push(item);
  }
  return nullptr;
}

void *stage2(void *args)
{
  for (int i = 0; i < NUM_ITEMS; ++i)
  {
    int item = buffer1.pop();
    item *= 2; // Procesa el dato
    std::cout << "Stage 2 processed: " << item << std::endl;
    buffer2.push(item);
  }
  return nullptr;
}

void *stage3(void *args)
{
  for (int i = 0; i < NUM_ITEMS; ++i)
  {
    int item = buffer2.pop();
    item += 10; // Procesa el dato final
    std::cout << "Stage 3 processed: " << item << std::endl;
  }
  return nullptr;
}

int main()
{
  pthread_t t1, t2, t3;

  pthread_create(&t1, nullptr, stage1, nullptr);
  pthread_create(&t2, nullptr, stage2, nullptr);
  pthread_create(&t3, nullptr, stage3, nullptr);

  pthread_join(t1, nullptr);
  pthread_join(t2, nullptr);
  pthread_join(t3, nullptr);

  return 0;
}
