#include <boost/fiber/all.hpp>
#include <iostream>

boost::fibers::mutex mtx;
std::string shared_data;

boost::fibers::condition_variable cv;
bool state_ready{false};

void writer() {
  std::lock_guard<boost::fibers::mutex> lock(mtx);
  shared_data = "Hello from writer";
}

void reader() {
  std::lock_guard<boost::fibers::mutex> lock(mtx);
  std::cout << "Shared Data: " << shared_data << "\n";
}

void waiter() {
  std::unique_lock<boost::fibers::mutex> lock(mtx);
  cv.wait(lock, [] { return state_ready; });
  std::cout << "State_ready : " << state_ready << '\n';
}

void notifier() {
  {
    std::lock_guard<boost::fibers::mutex> lock(mtx);
    state_ready = true;
  }
  cv.notify_one();
}

int main() {
  using namespace boost::fibers;

  /// MUTEX :
  fiber f1(writer);
  fiber f2(reader);

  f1.join();
  f2.join();

  /// CONDITIONAL VARIABLES :
  fiber f3(waiter);
  fiber f4(notifier);

  f3.join();
  f4.join();

  return EXIT_SUCCESS;
}