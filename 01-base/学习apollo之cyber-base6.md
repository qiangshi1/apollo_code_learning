### 20. thread_safe_queue.h`

线程池?

#### a) `BUILD`的控制：

```
cc_library(
    name = "thread_safe_queue",
    hdrs = ["thread_safe_queue.h"],
)
```

它是常规链接库.

#### b) 类`ThreadSafeQueue`

该头文件只有一个类.

#### b1) 类成员`ThreadSafeQueue::break_all_wait_`

- 是不是多线程临界资源都加上`volatile`比较好？

#### b2) 类成员`ThreadSafeQueue::mutex_

- 线程锁

#### b3) 类成员`ThreadSafeQueue::queue_

- 队列

#### b4) 类成员`ThreadSafeQueue::cv_

- `std::condition_variable`

#### c) `delete`

取消拷贝赋值。



#### d) `ThreadSafeQueue::Enqueue()`

- 线程安全地：向queue_对尾加入一个元素
- 随机唤醒一个线程？(`cv_.notify_one()`)
- `std::condition_variable`:线程同步用的
  - `std::condition_variable`的`notify_all()`和`wait()`

```c++
// 用std::condition_variable唤醒所有线程的一个例子, 参考 https://murphypei.github.io/blog/2019/04/cpp-concurrent-3.html
#include <condition_variable>  // std::condition_variable
#include <iostream>            // std::cout
#include <mutex>               // std::mutex, std::unique_lock
#include <string>              // std::string
#include <thread>              // std::thread
std::mutex mtx;                // 全局互斥锁.
std::condition_variable cv;    // 全局条件变量.
void do_print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  cv.wait(lck);  // 当前线程被阻塞.
  // 线程被唤醒, 继续往下执行.
  std::cout << "thread " << id << '\n';
}
void go() {
  std::unique_lock<std::mutex> lck(mtx);
  cv.notify_all();  // 唤醒所有线程.
}
int main() {
  int thread_num = 100;
  std::thread threads[thread_num];
  // 开辟10个线程
  for (int i = 0; i < thread_num; ++i) threads[i] = std::thread(do_print_id, i);
  //输入go,会看到10个线程执行.
  std::cout << "please input go to wake up all threads" << std::endl;
  std::string input_str;
  while (1) {
    std::cin >> input_str;
    if (input_str != "go") {
      std::cout << "error input. please do again!" << std::endl;
    } else {
      break;
    }
  }
  go();
  for (auto& th : threads) th.join();
  return 0;
}
```

但是上例之中若程序执行过快可能会有问题（可能），如果注释掉cin：

```c++
// 该例之中并不是所有线程都会执行，这可能是线程执行过快所致？
#include <condition_variable>  // std::condition_variable
#include <iostream>            // std::cout
#include <mutex>               // std::mutex, std::unique_lock
#include <string>              // std::string
#include <thread>              // std::thread
std::mutex mtx;                // 全局互斥锁.
std::condition_variable cv;    // 全局条件变量.
void do_print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  cv.wait(lck);  // 当前线程被阻塞.
  // 线程被唤醒, 继续往下执行.
  std::cout << "thread " << id << '\n';
}
void go() {
  std::unique_lock<std::mutex> lck(mtx);
  cv.notify_all();  // 唤醒所有线程.
}
int main() {
  int thread_num = 100;
  std::thread threads[thread_num];
  // 开辟10个线程
  for (int i = 0; i < thread_num; ++i) threads[i] = std::thread(do_print_id, i);
//   //输入go,会看到10个线程执行.
//   std::cout << "please input go to wake up all threads" << std::endl;
//   std::string input_str;
//   while (1) {
//     std::cin >> input_str;
//     if (input_str != "go") {
//       std::cout << "error input. please do again!" << std::endl;
//     } else {
//       break;
//     }
//   }
  go();
  for (auto& th : threads) th.join();
  return 0;
}
```

```c++
// 但加上一个全局标志位之后又不会又该问题了，好奇怪。
#include <condition_variable>  // std::condition_variable
#include <iostream>            // std::cout
#include <mutex>               // std::mutex, std::unique_lock
#include <string>              // std::string
#include <thread>              // std::thread
std::mutex mtx;                // 全局互斥锁.
std::condition_variable cv;    // 全局条件变量.
bool ready = false;   // 全局标志位
void do_print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  // 如果标志位不为true，则等待
  while(!ready)
  {
    cv.wait(lck);  // 当前线程被阻塞.
  }
  // 线程被唤醒, 继续往下执行.
  std::cout << "thread " << id << '\n';
}
void go() {
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;
  cv.notify_all();  // 唤醒所有线程.
}
int main() {
  int thread_num = 100;
  std::thread threads[thread_num];
  // 开辟10个线程
  for (int i = 0; i < thread_num; ++i) threads[i] = std::thread(do_print_id, i);
  //输入go,会看到10个线程执行.
//   std::cout << "please input go to wake up all threads" << std::endl;
//   std::string input_str;
//   while (1) {
//     std::cin >> input_str;
//     if (input_str != "go") {
//       std::cout << "error input. please do again!" << std::endl;
//     } else {
//       break;
//     }
//   }
  go();
  for (auto& th : threads) th.join();
  return 0;
}
```
- `std::condition_variable`的`notify_one()` 说的是随机唤起一个线程，但是下例的测试显示随机唤起一个到多个线程。

```c++
// 测试 `notify_one()`
#include <condition_variable>  // std::condition_variable
#include <iostream>            // std::cout
#include <mutex>               // std::mutex, std::unique_lock
#include <string>              // std::string
#include <thread>              // std::thread
std::mutex mtx;                // 全局互斥锁.
std::condition_variable cv;    // 全局条件变量.
bool ready = false;   // 全局标志位
void do_print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  // 如果标志位不为true，则等待
  while(!ready)
  {
    cv.wait(lck);  // 当前线程被阻塞.
  }
  // 线程被唤醒, 继续往下执行.
  std::cout << "thread " << id << '\n';
}
void go() {
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;
  cv.notify_one();  // 随机唤醒一个等待线程.
}
int main() {
  int thread_num = 100;
  std::thread threads[thread_num];
  // 开辟10个线程
  for (int i = 0; i < thread_num; ++i) threads[i] = std::thread(do_print_id, i);
  //输入go,会看到10个线程执行.
//   std::cout << "please input go to wake up all threads" << std::endl;
//   std::string input_str;
//   while (1) {
//     std::cin >> input_str;
//     if (input_str != "go") {
//       std::cout << "error input. please do again!" << std::endl;
//     } else {
//       break;
//     }
//   }
  go();
  for (auto& th : threads) th.join();
  return 0;
}
```

#### e) `ThreadSafeQueue::Dequeue()`

- 线程安全地从队列中取出第一个元素？并在队列中删除该元素？
- `std::move()`: 左值变为右值, 等同于`static_cast<T&&>(lvalue);`但为什么会出现在这里？

#### f) `ThreadSafeQueue::WaitDequeue()`

- 若`break_all_wait_==true`且`queue_`不为空，执行`Dequeue()`，否则线程等待。
- `wait()`有两种用法，一种是：void wait (unique_lock<mutex>& lck);上边已经介绍；另一种是`template <class Predicate> void wait (unique_lock<mutex>& lck, Predicate pred);`，类似于：`while (!pred()) wait(lck);`，相当于加了条件判断语句。 https://blog.csdn.net/zzhongcy/article/details/85248597

#### g) `ThreadSafeQueue::Empty()`

- 判断queue_是否为空。

#### g) `ThreadSafeQueue::BreakAllWait()`

- 唤醒所有线程。