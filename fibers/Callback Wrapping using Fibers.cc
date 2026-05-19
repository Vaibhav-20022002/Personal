#include <boost/fiber/all.hpp>
#include <iostream>
#include <thread>

/// \brief This style is for Legacy Callback APIs where the async library wants
/// to call you later.
///
/// \details Basic variants of this style are : return just an error code,
/// return data or an error, return success or exception, or use virtual
/// success()/error() methods.
///
/// \note The commmon trick is to convert the callback completion into a
/// promise/future pair.
///
/// \attention USAGE :
/// # When the API already speaks "callback" style.
///
/// # When the completion may report success or failure (or both).
///
/// # When you want to present a synchronous-looking function to the rest
/// # of your fiber code.
///
/// # When you need to adapt a library that cannot be changed.

struct Response {
  using ptr = std::shared_ptr<Response>;
  virtual ~Response() = default;
  virtual void success(std::string_view data) = 0;
  virtual void error(int err_code) = 0;
};

/// \note The final keyword tells the compiler no other class can inherit from
/// this, allowing virtual function call optimizations.
struct PromiseResponse final : public Response {
  boost::fibers::promise<std::string> p;
  boost::fibers::future<std::string> get_future() { return p.get_future(); }

  void success(std::string_view data) override {
    p.set_value(std::string(data));
  }

  void error(int err_code) override {
    p.set_exception(std::make_exception_ptr(
        std::runtime_error("legacy error code " + std::to_string(err_code))));
  }
};

struct LegacyService {
  void async_fetch(Response::ptr resp) {
    std::thread([resp = std::move(resp)]() mutable {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));

      /// Simulate random success/failure:
      const bool ok = true;
      if (ok) {
        resp->success("Data retrieved");
      } else {
        resp->error(17);
      }
    }).detach();
  }
};

/// \brief This is clean, synchronous looking API for Fiber.
std::string fetch_as_fiber(LegacyService &service) {
  auto resp = std::make_shared<PromiseResponse>();
  auto future = resp->get_future();
  service.async_fetch(resp);

  /// \note This cooperatively suspends the calling fiber.
  /// The underlying OS thread is NOT blocked.
  /// \attention Once the background thread calls set_value/set_execption,
  /// the fiber scheduler is notified, wakes this fiber up, and future.get()
  /// returns the result.
  return future.get();
}

int main() {
  LegacyService service;
  try {
    std::string data = fetch_as_fiber(service);
    std::cout << "Data: " << data << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
