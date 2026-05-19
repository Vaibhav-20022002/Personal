#include <boost/fiber/all.hpp>
#include <boost/fiber/buffered_channel.hpp>
#include <iostream>

/// \brief Buffered channel gives queue with capacity. (default capacity 0)
/// \note The capacity must be a power of 2 (e.g., 2, 4, 8, 16...).
/// \note Effective capacity is actually (capacity - 1) due to internal
/// ring-buffer logic
///       that requires one empty slot to distinguish between empty and full
///       states.
/// \attention `pop()` suspends the fiber when the channel is empty until a
/// value is pushed or channel is closed. \attention `push()` stores a value
/// into the channel and if the channel is full it blocks the pushing fiber
/// until space becomes available. \attention `close()` deactivates the channel
/// and wakes waiting fibers appropriately.

void producer(boost::fibers::buffered_channel<int> &chan) {
  for (int i = 0; i < 7; ++i) {
    chan.push(i);
    std::cout << "Pushed " << i << '\n';
  }
  chan.close();
}

void consumer(boost::fibers::buffered_channel<int> &chan) {
  int val;
  while (chan.pop(val) == boost::fibers::channel_op_status::success) {
    std::cout << "Popped " << val << '\n';
  }
  std::cout << "Consumer channel closed\n";
}

int main(void) {
  using namespace boost::fibers;

  /// \note Capacity must be a power of 2 (e.g., 2, 4, 8, 16...)
  buffered_channel<int> chan(4);

  fiber prod(producer, std::ref(chan));
  fiber cons(consumer, std::ref(chan));

  prod.join();
  cons.join();

  return EXIT_SUCCESS;
}