#include <boost/fiber/all.hpp>
#include <boost/fiber/unbuffered_channel.hpp>
#include <iostream>

/// \brief Unbuffered channel has ZERO capacity.
/// \note It acts as a rendezvous point: `push()` blocks until a `pop()` is
/// called,
///       and `pop()` blocks until a `push()` is called.
/// \note This ensures synchronous handoff between fibers.

void producer(boost::fibers::unbuffered_channel<int> &chan) {
  for (int i = 0; i < 10; ++i) {
    chan.push(i);
    std::cout << "Pushed " << i << '\n';
  }
  chan.close();
}

void consumer(boost::fibers::unbuffered_channel<int> &chan) {
  int val;
  while (chan.pop(val) == boost::fibers::channel_op_status::success) {
    std::cout << "Popped " << val << '\n';
  }
  std::cout << "Consumer channel closed\n";
}

int main(void) {
  using namespace boost::fibers;

  unbuffered_channel<int> chan;

  fiber prod(producer, std::ref(chan));
  fiber cons(consumer, std::ref(chan));

  prod.join();
  cons.join();

  return EXIT_SUCCESS;
}