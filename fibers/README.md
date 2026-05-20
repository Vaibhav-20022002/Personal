# Boost.Fiber Tutorial Guide

This repository contains educational implementations and references for learning `boost::fiber`, a high-performance cooperative user-space threading library in C++.

Fibers provide the scalability and lightweight execution of asynchronous programming (coroutines) combined with the synchronous programming model of standard OS threads.

## Core Motive and Purpose of Boost.Fiber

Asynchronous programming with callbacks, event loops, or promises is performant but introduces call stack fragmentation, complex error propagation, and cognitive overhead. Standard OS threads (`std::thread`) are easy to use but have high memory overhead (minimum 1MB stack allocations) and incur kernel transitions during context switches, limiting concurrent thread count.

Boost.Fiber provides:
* **Stackful Execution:** Each fiber has its own call stack, allowing it to yield execution from deep within nested function calls (unlike stackless coroutines, which can only suspend at the top level).
* **Cooperative Scheduling:** Fibers yield control voluntarily. This minimizes thread preemption, simplifies synchronization, and achieves fast context switches in user space.
* **Synchronous-Looking Async Code:** Code remains sequential and readable while executing asynchronously and efficiently under the hood.

## Compilation and Setup

### Compiler Alignment
Due to stack-alignment and ABI mismatches between some standard libraries (specifically libc++ and Boost), compile using libstdc++ and link the boost_fiber, boost_context, and pthread libraries.

### Command Example
To compile the C++ files in this repository:

```bash
clang++ -stdlib=libstdc++ -std=c++17 <filename>.cc -o <output_name> -lboost_fiber -lboost_context -pthread
```

Using g++:

```bash
g++ -std=c++17 <filename>.cc -o <output_name> -lboost_fiber -lboost_context -pthread
```

## Index and File Mapping

The index below outlines all topics in the Boost.Fiber tutorial, indicating which concepts are implemented in this repository and which are theoretical topics.

| Section | Topic | Status | Associated File |
| :--- | :--- | :--- | :--- |
| **0. Orientation** | 1. What `boost::fiber` is <br> 2. Why fibers exist <br> 3. Fiber vs thread vs coroutine vs task <br> 4. When to use fibers, and when not to | [Theory Only] | See Theoretical Foundations below |
| **1. Prerequisites** | 1. C++ background needed <br> 2. What Boost is <br> 3. Installing Boost.Fiber <br> 4. Building a small project with CMake <br> 5. Linking and common setup issues | [Theory Only] | See compilation details above |
| **2. Mental Model** | 1. What a fiber really is <br> 2. Stackful execution explained <br> 3. Cooperative scheduling <br> 4. The fiber lifecycle <br> 5. How fibers run on threads <br> 6. Why yielding matters | [Theory Only] | See Theoretical Foundations below |
| **3. First Programs** | 1. A minimal fiber example <br> 2. Spawning fibers <br> 3. Joining fibers | [Implemented] | [oneFiber.cc](./oneFiber.cc) |
| | 4. Passing arguments into fibers | [Implemented] | [call_with_args.cc](./call_with_args.cc) |
| | 5. Capturing state safely <br> 6. Returning values from fiber work | [Theory Only] | Demonstrated via lambda captures |
| **4. Scheduling** | 1. What a scheduler does <br> 2. The default scheduler model <br> 3. Yielding control <br> 4. Suspended and resumed fibers <br> 5. Ready queues and execution order <br> 6. Cooperative vs preemptive scheduling | [Implemented] | [yield.cc](./yield.cc) |
| **5. Context Switch** | 1. What a context switch means <br> 2. Stack switching concept <br> 3. Cost comparison with threads <br> 4. Why fibers are lightweight <br> 5. Practical performance intuition | [Theory Only] | See Theoretical Foundations below |
| **6. Synchronization** | 1. `mutex` <br> 2. `condition_variable` <br> 6. Differences from thread synchronization | [Implemented] | [synchronization.cc](./synchronization.cc) |
| | 3. `barrier` <br> 4. `latch`-style coordination patterns | [Theory Only] | Not implemented |
| | 5. `future` / `promise` style coordination | [Implemented] | [Futures Promises & Packaged Tasks.cc](./Futures%20Promises%20&%20Packaged%20Tasks.cc) |
| **7. Local Storage** | 1. Fiber-local data <br> 2. Thread-local vs fiber-local <br> 3. Safe state sharing patterns <br> 4. Avoiding accidental global state | [Implemented] | [fiber local storage.cc](./fiber%20local%20storage.cc) |
| **8. Communication** | 1. Message passing concept <br> 2. Shared memory vs passing ownership <br> 3. Producer-consumer patterns | [Implemented] | [unbuffered channel.cc](./unbuffered%20channel.cc) <br> [buffered channel.cc](./buffered%20channel.cc) |
| | 4. Work queues <br> 5. Fan-in and fan-out patterns | [Theory Only] | Can be implemented using channels |
| **9. Scheduler Control**| 1. Explicit yielding | [Implemented] | [yield.cc](./yield.cc) |
| | 2. Blocking behavior and why it matters <br> 4. Multiple fibers per thread <br> 5. Controlling fairness <br> 6. Starvation | [Theory Only] | Not implemented |
| | 3. Running fibers inside worker threads | [Implemented] | [fibers in worker threads.cc](./fibers%20in%20worker%20threads.cc) |
| **10. Composition** | 1. Nested fibers <br> 2. Fiber pipelines <br> 3. Fork/join patterns <br> 4. Event-driven fiber workflows <br> 5. State machines with fibers | [Theory Only] | Not implemented |
| **11. Performance** | 1. When fibers improve throughput <br> 2. When fibers improve latency <br> 3. Overuse risks <br> 4. Stack memory costs <br> 5. Cache behavior <br> 6. Scheduling overhead and tuning | [Theory Only] | Refer to work pool cache locality concepts |
| **12. Safe Design** | 1. Cooperative correctness <br> 2. Blocking APIs and hidden traps <br> 3. Race conditions <br> 4. Deadlocks <br> 5. Reentrancy <br> 6. Exception safety | [Theory Only] | Exception safety shown in [Futures Promises...cc](./Futures%20Promises%20&%20Packaged%20Tasks.cc) |
| **13. Integration** | 1. Mixing `boost::fiber` with `std::thread` | [Implemented] | [fibers in worker threads.cc](./fibers%20in%20worker%20threads.cc) <br> [Work-stealing work pool.cc](./Work-stealing%20work%20pool.cc) |
| | 2. Fiber-aware worker pools | [Implemented] | [Work-stealing work pool.cc](./Work-stealing%20work%20pool.cc) |
| | 3. Integrating fibers with OS I/O <br> 4. Event-loop integration | [Theory Only] | Not implemented |
| | 5. Async interop | [Implemented] | [Callback Wrapping using Fibers.cc](./Callback%20Wrapping%20using%20Fibers.cc) |
| **14. Scheduler Design**| 1. Scheduler anatomy <br> 2. Fairness strategies <br> 4. Custom schedulers <br> 5. Scheduler observability | [Theory Only] | Not implemented |
| | 3. Work stealing | [Implemented] | [Work-stealing work pool.cc](./Work-stealing%20work%20pool.cc) |
| **15. Backpressure** | 1. Channels as message-passing primitives <br> 2. Producer-consumer systems | [Implemented] | [unbuffered channel.cc](./unbuffered%20channel.cc) <br> [buffered channel.cc](./buffered%20channel.cc) |
| | 3. Pipeline architecture <br> 4. Fan-out / fan-in patterns | [Theory Only] | Not implemented |
| | 5. Queueing and flow control (Backpressure) | [Implemented] | [buffered channel.cc](./buffered%20channel.cc) |
| **16. Sync at Scale** | 1. Mutex design in fiber systems <br> 4. Deadlock analysis <br> 5. Liveness and fairness | [Theory Only] | Not implemented |
| | 2. Condition-variable workflows | [Implemented] | [synchronization.cc](./synchronization.cc) |
| | 3. Cross-thread synchronization | [Implemented] | [fibers in worker threads.cc](./fibers%20in%20worker%20threads.cc) <br> [Work-stealing work pool.cc](./Work-stealing%20work%20pool.cc) |
| **17. Propagation** | 1. Fiber-local storage | [Implemented] | [fiber local storage.cc](./fiber%20local%20storage.cc) |
| | 2. Thread-local vs fiber-local <br> 3. Context propagation <br> 4. Per-request runtime data | [Theory Only] | Not implemented |
| **18. Teardown** | 1. Graceful shutdown | [Implemented] | [Work-stealing work pool.cc](./Work-stealing%20work%20pool.cc) |
| | 2. Fiber lifetime rules | [Implemented] | [oneFiber.cc](./oneFiber.cc) <br> [Futures Promises...cc](./Futures%20Promises%20&%20Packaged%20Tasks.cc) |
| | 3. Cancellation models <br> 4. Exception-safe teardown <br> 5. Service termination | [Theory Only] | Not implemented |
| **19. Performance Eng.**| 1. Stack size strategy <br> 2. Yield frequency <br> 3. Cache behavior <br> 4. Contention analysis <br> 5. Throughput vs latency tuning | [Theory Only] | Not implemented |
| **20. Adv. Patterns** | 1. Fiber-per-request server model <br> 2. Fiber pipelines <br> 3. Actor-like designs <br> 4. Simulation runtimes <br> 5. Task decomposition | [Theory Only] | Not implemented |
| **21. Observability** | 1. Tracing fiber execution <br> 2. Hangs and stalls <br> 3. Deadlock diagnosis <br> 4. Failure visibility | [Theory Only] | Not implemented |
| **22. Migration** | 1. Porting thread code blindly <br> 2. Overusing fibers <br> 3. Assuming preemption | [Theory Only] | Not implemented |
| | 4. Migrating callback code | [Implemented] | [Callback Wrapping using Fibers.cc](./Callback%20Wrapping%20using%20Fibers.cc) |
| **23. Deep Projects** | 1. Fiber-based HTTP server core <br> 2. Fiber-driven job queue <br> 3. Multi-stage streaming pipeline <br> 4. Worker pool with backpressure <br> 5. Cooperative scheduler prototype <br> 6. Mini actor runtime on fibers | [Theory Only] | Not implemented |

## Recommended Study Order

The recommended sequence for reading and understanding the code files is:

```
[1. oneFiber.cc] (Basic Spawn & Join)
       │
       ▼
[2. call_with_args.cc] (Parameter Passing)
       │
       ▼
[3. yield.cc] (Cooperative Yielding)
       │
       ▼
[4. synchronization.cc] (Mutexes & CVs)
       │
       ▼
[5. Futures & Promises.cc] (Asynchronous Returns)
       │
       ▼
[6. fiber local storage.cc] (FSS and State Scope)
       │
       ▼
[7. Channels (Unbuffered/Buffered)]
       │
       ▼
[8. fibers in worker threads.cc] (Multi-threaded Integration)
       │
       ▼
[9. Work-stealing work pool.cc] (Work Stealing Pools)
       │
       ▼
[10. Callback Wrapping.cc] (Callback Adaptation)
```

### 1. Basic Lifecycle
* **[oneFiber.cc](./oneFiber.cc)**: Spawning a single fiber and calling `join()`. Illustrates fiber execution on the primary thread.
* **[call_with_args.cc](./call_with_args.cc)**: Passing parameters into the fiber's entry function.

### 2. Cooperative Scheduling
* **[yield.cc](./yield.cc)**: Spawning multiple fibers on one thread and yielding execution between them using `boost::this_fiber::yield()`.

### 3. Synchronization and State
* **[synchronization.cc](./synchronization.cc)**: Using fiber-aware mutexes and condition variables. Standard threading primitives (`std::mutex`) block the OS thread; `boost::fibers::mutex` blocks only the calling fiber.
* **[Futures Promises & Packaged Tasks.cc](./Futures%20Promises%20&%20Packaged%20Tasks.cc)**: Retrieving results and exceptions from detached fibers via promises and futures.
* **[fiber local storage.cc](./fiber%20local%20storage.cc)**: Managing fiber-scoped state using `fiber_specific_ptr`.

### 4. Channels
* **[unbuffered channel.cc](./unbuffered%20channel.cc)**: Synchronous message passing with zero-capacity rendezvous points.
* **[buffered channel.cc](./buffered%20channel.cc)**: Queue-based message passing. Provides backpressure control by blocking `push()` calls when the channel is full.

### 5. Multi-threading and Integration
* **[fibers in worker threads.cc](./fibers%20in%20worker%20threads.cc)**: Running fibers on background threads and initializing the round-robin scheduler via `boost::fibers::use_scheduling_algorithm<algo::round_robin>()`.
* **[Work-stealing work pool.cc](./Work-stealing%20work%20pool.cc)**: Advanced thread pool design using `boost::fibers::algo::work_stealing`. Idle threads steal fibers from active threads to balance CPU utilization.
* **[Callback Wrapping using Fibers.cc](./Callback%20Wrapping%20using%20Fibers.cc)**: Adapting legacy callback-based interfaces to synchronous fiber code via promise-future bridges without blocking OS worker threads.

## Theoretical Foundations

The following concepts from the index are not represented in the codebase and must be referenced theoretically:

### 1. Fiber vs Thread vs Coroutine vs Task
* **Thread (OS level):** Scheduled by the kernel. Preemptive, large footprint (1MB+ stack), and high context-switching overhead.
* **Coroutine (Stackless, e.g., C++20 `std::coroutine`):** No dedicated stack. Suspension points are determined at compile time and can only be used at the top-level of the coroutine function.
* **Fiber (Stackful coroutine):** Has an independent stack allocated in user space. Allows suspension from deep nested function calls. Scheduled cooperatively.
* **Task:** Represents a unit of work (e.g. standard future/promise tasks) independent of execution context.

### 2. Blocking APIs and Thread Starvation
Calling blocking system calls (e.g. `std::this_thread::sleep_for()`, blocking socket read, or blocking file I/O) blocks the host OS thread. Since the scheduler runs on this thread, all other fibers assigned to the thread will be starved. Use non-blocking, asynchronous OS interfaces (such as Boost.Asio or `io_uring`) paired with fiber-aware synchronization.

### 3. Custom Schedulers
Boost.Fiber allows customization of scheduling behavior by inheriting from `boost::fibers::algo::algorithm`. 
* **Round Robin:** FIFO queue-based distribution on a single thread.
* **Work Stealing:** Distributed queues across worker threads.
* **Fairness:** Cooperative systems depend on voluntary yielding. A fiber that executes infinite loops without yielding will starve other fibers on that thread.

### 4. Stack Management
Fibers require stack memory allocation. Boost.Fiber supports different allocators:
* `protected_fixedsize_stack`: Appends a guard page to catch stack overflows (safer).
* `pooled_fixedsize_stack`: Reuses previously allocated stacks to reduce heap churn.
For high-concurrency systems, set stack sizes to the minimum necessary (e.g. 16KB or 32KB) and avoid massive stack-allocated arrays.
