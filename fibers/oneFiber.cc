///\brief This program demonstrates the use of fibers in C++.
///\note The libc++ and boost has ABI/stack-alignment mismatches issues so you
/// must compile with libstdc++. \note Use this command : clang++
///-stdlib=libstdc++ oneFiber.cc -o exe -lboost_fiber -lboost_context -pthread

#include <boost/fiber/all.hpp>
#include <iostream>

int main(void) {
  std::cout << "Inside Main()\n";

  boost::fibers::fiber f([]() { std::cout << "Inside Fiber\n"; });

  f.join();

  std::cout << "Back in main() again\n";

  return EXIT_SUCCESS;
}