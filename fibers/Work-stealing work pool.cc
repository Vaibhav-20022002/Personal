#include <boost/fiber/all.hpp>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

/**
 * THREAD-SAFE LOGGING
 * @brief Since we are running on multiple OS threads, we must guard std::cout
 * with a mutex.
 */
std::mutex log_mtx;
#define LOG(expr)                                                              \
  do {                                                                         \
    std::ostringstream ss;                                                     \
    ss << expr;                                                                \
    std::lock_guard<std::mutex> lock(log_mtx);                                 \
    std::cout << ss.str() << '\n';                                             \
  } while (0)

/**
 * EXPLANATION: WORK-STEALING SCHEDULER :
 * @details :
 * 1. Efficiency: Work-stealing is generally more performant than work-sharing.
 *    Each OS thread has its own local queue of ready fibers. This minimizes
 *    lock contention and improves cache locality (data non-locality is
 * avoided).
 *
 * 2. The Mechanism: When an OS thread runs out of fibers in its local queue,
 *    it "steals" a fiber from the queue of another OS thread in the same pool.
 *
 * 3. Thread Pool: All threads participating in the work-stealing pool must
 *    initialize the algorithm with the same total thread count.
 */

/// Synchronization for task completion
boost::fibers::mutex completion_mutex;
boost::fibers::condition_variable_any completion_cv;
std::size_t tasks_remaining{0};

/// Synchronization for worker thread shutdown
boost::fibers::mutex shutdown_mutex;
boost::fibers::condition_variable_any shutdown_cv;
bool shutdown{false};

/**
 * @brief WORKER THREAD ENTRY POINT
 * This function runs in a standard std::thread.
 */
void worker_thread(std::size_t worker_count, std::size_t worker_id) {
  /// Step 1: Initialize the work-stealing scheduler for this OS thread.
  boost::fibers::use_scheduling_algorithm<boost::fibers::algo::work_stealing>(
      worker_count);

  LOG("[Worker " << worker_id << "] Scheduler installed on OS thread: "
                 << std::this_thread::get_id());

  /// Step 2: Keep the thread alive. The main fiber of this thread will block
  /// until the shutdown signal is received. While blocked, the scheduler
  /// will automatically pick up and run (or steal) other fibers.
  {
    std::unique_lock<boost::fibers::mutex> lock(shutdown_mutex);
    shutdown_cv.wait(lock, [&] { return shutdown; });
  }

  LOG("[Worker " << worker_id << "] shutting down.");
}

/**
 * @brief FIBER TASK
 * This function will be executed as a fiber.
 */
void task(std::size_t id) {
  LOG("[Task " << id
               << "] started on OS thread: " << std::this_thread::get_id());

  long long sum = 0;
  /// Uneven workloads make stealing easier to observe in the logs.
  const int phases = 3 + (id % 5);

  for (int phase = 0; phase < phases; ++phase) {
    /// Simulate some work
    for (int i = 0; i < 100000; ++i) {
      sum += (id * 131 + i + phase) % 101;
    }

    /// Yielding gives the scheduler a chance to redistribute fibers if needed.
    if (phase + 1 < phases) {
      boost::this_fiber::yield();
    }
  }

  LOG("[Task " << id << "] finished.");

  /// Notify main thread that this task is done
  {

    std::unique_lock<boost::fibers::mutex> lock(completion_mutex);
    tasks_remaining--;
  }
  completion_cv.notify_one();
}

int main(void) {
  constexpr std::size_t worker_count = 4; // 1 Main Thread + 3 Worker Threads
  constexpr std::size_t task_count = 12;

  std::vector<std::thread> workers;
  workers.reserve(worker_count - 1);

  /// 1. Spawn worker threads.
  /// These threads will immediately start and try to steal work from Main.
  for (std::size_t i = 1; i < worker_count; ++i) {
    workers.emplace_back(worker_thread, worker_count, i);
  }

  /// 2. Initialize the work-stealing scheduler for the Main OS thread.
  boost::fibers::use_scheduling_algorithm<boost::fibers::algo::work_stealing>(
      worker_count);
  LOG("[Main] Scheduler installed on OS thread: "
      << std::this_thread::get_id());

  /// 3. Initialize task counter
  {
    std::lock_guard<boost::fibers::mutex> lock(completion_mutex);
    tasks_remaining = task_count;
  }

  /// 4. Create tasks. These fibers are initially added to the Main thread's
  /// queue. The worker threads will "steal" these tasks from Main's queue.
  std::vector<boost::fibers::fiber> tasks;
  tasks.reserve(task_count);
  for (std::size_t i = 0; i < task_count; ++i) {
    tasks.emplace_back(task, i);
  }

  /// 5. Wait for all tasks to finish.
  {
    std::unique_lock<boost::fibers::mutex> lock(completion_mutex);
    completion_cv.wait(lock, [&] { return tasks_remaining == 0; });
  }
  LOG("[Main] All tasks completed.");

  /// 6. Clean up fiber objects
  for (auto &t : tasks) {
    if (t.joinable())
      t.join();
  }

  /// 7. Signal worker threads to shut down
  {
    std::lock_guard<boost::fibers::mutex> lock(shutdown_mutex);
    shutdown = true;
  }
  shutdown_cv.notify_all();

  /// 8. Wait for OS threads to finish
  for (auto &w : workers) {
    if (w.joinable())
      w.join();
  }

  LOG("[Main] Program exited successfully.");
  return EXIT_SUCCESS;
}