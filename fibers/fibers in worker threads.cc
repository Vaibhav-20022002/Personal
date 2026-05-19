#include <boost/fiber/all.hpp>
#include <iostream>
#include <mutex>
#include <thread>

/// \note boost::fibers::mutex is fiber-aware and, by default, can synchronize
/// fibers across threads. \note If Boost.Fiber is built with
/// BOOST_FIBERS_NO_ATOMICS, then all fibers touching the same
///       synchronization object must stay on the same thread.
boost::fibers::mutex mtx;

/// \note boost::fibers::condition_variable_any can work with any lock type
/// (like std::unique_lock<std::mutex>).
boost::fibers::condition_variable_any cv;
bool stop{false};

void worker(int threadID) {
  /// Install a scheduler for THIS thread. Every OS thread needs its own
  /// scheduler.
  boost::fibers::use_scheduling_algorithm<boost::fibers::algo::round_robin>();

  /// unique_lock is REQUIRED for cv.wait() because the CV needs to
  /// unlock/relock the mutex internally while the fiber is suspended.
  std::unique_lock<boost::fibers::mutex> lock(mtx);
  cv.wait(lock, [&] { return stop; });

  std::cout << "Worker " << threadID << " is shutting down.\n";
}

int main(void) {
  using namespace boost::fibers;

  std::thread thread1(worker, 1);

  /// Do other work here

  {
    /// lock_guard is sufficient here as we don't need manual unlock or CV
    /// waiting. It's more lightweight and clearly signals a simple critical
    /// section.
    std::lock_guard<boost::fibers::mutex> lock(mtx);
    stop = true;
  }

  cv.notify_one();
  thread1.join();

  return EXIT_SUCCESS;
}