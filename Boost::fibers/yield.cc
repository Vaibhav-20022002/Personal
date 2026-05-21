#include <boost/fiber/all.hpp>
#include <iostream>

int main() {

  boost::fibers::fiber f1([]() {
    std::cout << "Fiber 1 : Doing Ist task...\n";
    boost::this_fiber::yield();
    std::cout << "Fiber 1 : Doing IInd task...\n";
  });

  boost::fibers::fiber f2([]() {
    std::cout << "Fiber 2 : Doing Ist task...\n";
    boost::this_fiber::yield();
    std::cout << "Fiber 2 : Doing IInd task...\n";
  });

  f1.join();
  f2.join();

  return EXIT_SUCCESS;
}