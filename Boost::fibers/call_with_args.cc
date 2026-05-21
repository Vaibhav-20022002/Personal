#include <boost/fiber/all.hpp>
#include <iostream>

void work(int a, int b) {
  std::cout << "Inside work() with arguments" << a << " " << b << "\n";
  std::cout << "Result: " << a + b << "\n";
}

int main() {
  std::cout << "Inside main()\n";
  boost::fibers::fiber f(work, 10, 20);
  if (f.joinable())
    f.join();
  std::cout << "Back in main()\n";
  return EXIT_SUCCESS;
}