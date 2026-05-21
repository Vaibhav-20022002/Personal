#include <boost/fiber/all.hpp>
#include <boost/fiber/fss.hpp>
#include <iostream>

/// \note Fiber Specific Storage (FSS) must outlive the fibers that use them,
/// that's why it is declared global to both fibers.
boost::fibers::fiber_specific_ptr<std::string> fss;

/// \note Mutex is redundant here as we are running two fibers on the same
/// thread (main thread), so there is no preemption. But it is there for the
/// sake of learning good practices.
boost::fibers::mutex mtx;

void worker(const std::string &name) {
  /// Each fiber gets its own separate pointer value :
  fss.reset(new std::string(name));

  {
    std::lock_guard<boost::fibers::mutex> lock(mtx);
    std::cout << "Fiber started with FSS = " << *fss << "\n";
  }

  /// Yield so another fiber can run on the same thread
  boost::this_fiber::yield();

  {
    std::lock_guard<boost::fibers::mutex> lock(mtx);
    std::cout << "Fiber resumed with FSS = " << *fss << '\n';
  }
}
int main(void) {
  using namespace boost::fibers;

  fiber f1(worker, "Fiber-A");
  fiber f2(worker, "Fiber-B");

  f1.join();
  f2.join();

  return EXIT_SUCCESS;
}