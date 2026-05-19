#include <boost/fiber/all.hpp>
#include <boost/fiber/future/future.hpp>
#include <boost/fiber/future/packaged_task.hpp>
#include <boost/fiber/future/promise.hpp>
#include <iostream>

/// \brief This style is cleanest when a one-shot result is needed to appear
/// later.
/// \details A `promise<>` stores either a value or an exception into the
/// associated `future<>`, while `packaged_task<>` wraps a callable so its
/// return value is computed asynchronously and delivered via the promise’s
/// future.
/// \note `fibers::async()` is a convenience wrapper around this idea.

/// \attention USAGE :
/// # When one owns the async boundary.
/// # When one wants a result value later.
/// # When one wants exception transport.
/// # When one wants to launch callable work onto a fiber and collect its
/// result.

/// \attention NON-USAGE :
/// ! Don't use for APIs that report progress repeatedly or call back multiple
/// ! times.

/// \note Exceptions thrown by the callable are stored in the future.

int compute_score(std::vector<int> data) {
  if (data.empty()) {
    throw std::runtime_error("Empty Input");
  }

  /// Simulate work that cooperatively yields :
  int sum = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    sum += data[i];

    /// Yield occasionally so other fibers can run
    if (i % 2 == 0) {
      boost::this_fiber::yield();
    }
  }
  return sum;
}

int main(void) {
  using namespace boost::fibers;

  /// 1. Create a future :
  future<int> ft;

  /// 2. Create a packaged_task that wraps/holds the callable/function :
  packaged_task<int(std::vector<int>)> task(compute_score);

  /// 3. Get the future from the task. This is the result handler.
  ft = task.get_future();

  std::vector<int> myData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  /// 4. Launch the work in a separate fiber means the callable runs
  /// asynchronously in a fiber :
  fiber(std::move(task), std::move(myData)).detach();

  /// 5. Get the result :
  /// ft.get() will cooperatively block the main fiber until the result is
  /// ready.
  try {
    int score = ft.get();
    std::cout << "Computed score: " << score << '\n';
  } catch (std::exception const &e) {
    std::cout << "Exception in fiber: " << e.what() << '\n';
  }

  return EXIT_SUCCESS;
}