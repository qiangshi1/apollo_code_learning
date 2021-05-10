### 21. `unbounded_queue.h`

无界限队列。

#### a) `BUILD`的控制：

```
cc_library(
    name = "unbounded_queue",
    hdrs = ["unbounded_queue.h"],
)
```

常规链接库.

#### b) 类`UnboundedQueue`

- 该头文件只有一个类.

#### b1) 类成员`UnboundedQueue::head_`

- 

#### b2) 类成员`UnboundedQueue::tail_`

- 

#### b3) 类成员`UnboundedQueue::tail_`

- 

#### c) 类构造函数`UnboundedQueue::UnboundedQueue()`

- 调用`Reset()`

#### d) `delete`

- 删除拷贝赋值。

#### e) 类析构函数`UnboundedQueue::~UnboundedQueue()`

- 调用`Destroy()`

#### f) 类函数`UnboundedQueue::Clear()`

- 调用`Destroy()`和`Reset()`

#### g) 类函数`UnboundedQueue::Enqueue()`

- 
- 如果忽略多线程的话：

```
while (true) {
  if (tail_.compare_exchange_strong(old_tail, node)) {
    old_tail->next = node;
    old_tail->release();
    size_.fetch_add(1);
    break;
  }
}
可以写成：
tail_=node;
old_tail->next = node;
old_tail->release();
size_.fetch_add(1);
```

所以代码的含义是向队列末尾增加一个元素，也就是tail_后边的一位。同时队列长度加1. 

- 原子类型的`fetch_add()`: 原子类型的自增？

#### h) 类函数`UnboundedQueue::Dequeue()`

- 线程安全地从队列中取出首个元素

#### i) 类函数`UnboundedQueue::Size()`

- 返回队列长度. 

#### i1) 类函数`UnboundedQueue::Empty()`

- 判断队列是否为空. 

#### k) 类内类声明`UnboundedQueue::Node`

- 链表的最小单元`Node`
- 为什么构造时`ref_count`是2，析构时ref_count是` ref_count.fetch_sub(1)`?

#### l) 类函数`UnboundedQueue::Reset()`

- 构造出一个有一个Node的链表

#### m) 类函数`UnboundedQueue::Destroy()`

- 删除链表的所有节点

### 22. `unbounded_queue_test.cc`

- `unbounded_queue.h`的test.

#### a) 类函数`UnboundedQueue::Reset()`

- 构造出一个有一个Node的链表

#### b) 类函数`UnboundedQueue::Destroy()`

- 删除链表的所有节点

### 23. `wait_strategy.h`

- 该头文件有1个父类和5个子类. 

#### a) `BUILD`的控制：

```
cc_library(
    name = "wait_strategy",
    hdrs = ["wait_strategy.h"],
)
```

常规链接库.

#### b)类 `WaitStrategy`

- 有4个虚函数: `NotifyOne()` `BreakAllWait()` `EmptyWait()` `~WaitStrategy`
- 关于虚函数这篇博客https://blog.csdn.net/weixin_43329614/article/details/89103574强调了虚函数与派生类的函数形参和函数修饰符必须完全一样，否则虚函数的继承不起作用。另外，我按照这篇博客改了改，发现只有指针和引用可以实现父类用子类资源（函数和成员）的情况：

```C++
#include<iostream>
#include<string>
using namespace std;
class Base
{
public:
    string str="Base";
	virtual void func()
	{
		cout <<str<< ":Base!" << endl;
	}
};
class Derived :public Base
{
public:
    string str2="Derived";
	void func()
	{
		cout <<str2<< ":Derived!" << endl;
	}
};
Base base;
Derived derived;
Base base2=static_cast<Base>(derived);
Base* base3=&derived;
Base base4=derived;
Base& base5=derived;
Base base6=derived;
Base* base7=static_cast<Base*>(&derived);
int main()
{
	base.func();
	derived.func();
	base2.func();
	base3->func();
	base4.func();
	base5.func();
	base6.func();
	base7->func();
    std::cout<<sizeof(base)<<std::endl;
    std::cout<<sizeof(derived)<<std::endl;
    std::cout<<sizeof(base2)<<std::endl;
    std::cout<<sizeof(base5)<<std::endl;
	return 0;
}
```

#### c)类 `BlockWaitStrategy`

- 继承自类`WaitStrategy`
- 该类打包了`std::mutex mutex_`和`condition_variable cv_`，应该是线程同步功能的进一步封装。

#### d)类构造函数 `BlockWaitStrategy::BlockWaitStrategy()`

- 

#### d1)类成员`BlockWaitStrategy::mutex_`

- 线程锁

#### d2)类成员`BlockWaitStrategy::cv_

- 线程同步:`std::condition_variable`

#### e)类函数 `BlockWaitStrategy::NotifyOne()`

- `override`的作用是给编译器提供报错信息，在成员函数声明或定义中， override 确保该函数为虚函数并覆写来自基类的虚函数。
- 随机唤醒一个线程。

#### f)类函数 `BlockWaitStrategy::EmptyWait()`

- 线程等待区

- 

#### g)类函数 `BlockWaitStrategy::BreakAllWait()`

- 唤醒所有线程

#### h)类 `SleepWaitStrategy`

- 继承自类`WaitStrategy`

#### h1)类成员 `SleepWaitStrategy::sleep_time_us_`

- `uint64_t`

#### i)类构造函数 `SleepWaitStrategy::SleepWaitStrategy()`

#### j)类构造函数 `SleepWaitStrategy::SleepWaitStrategy()`

- i)的重载。
- 给`sleep_time_us_`赋值

#### k)类函数 `SleepWaitStrategy::EmptyWait()`

- 等待`sleep_time_us_`时间

#### l)类函数 `SleepWaitStrategy::SetSleepTimeMicroSeconds()`

- 设置`sleep_time_us_`

#### m)类 `YieldWaitStrategy`

- 继承自类`WaitStrategy`

#### n)类构造函数 `YieldWaitStrategy::YieldWaitStrategy()`

- 

#### o)类函数 `YieldWaitStrategy::EmptyWait()`

- `std::this_thread::yield()` 让出时间片，减小CPU占用？

#### p)类 BusySpinWaitStrategy

- 继承自类`WaitStrategy`

#### q)类构造函数 `BusySpinWaitStrategy::BusySpinWaitStrategy()`

- 

#### r)类函数 `BusySpinWaitStrategy::EmptyWait()`

- 什么也不做，返回`true`
- 

#### s)类 `TimeoutBlockWaitStrategy`

- 继承自类`WaitStrategy`

#### s1)类成员`TimeoutBlockWaitStrategy::mutex_`

#### s2)类成员`TimeoutBlockWaitStrategy::cv_`

#### s3)类成员`TimeoutBlockWaitStrategy::time_out_`

#### q)类构造函数 `TimeoutBlockWaitStrategy::TimeoutBlockWaitStrategy()`

- 

#### s)类构造函数 `TimeoutBlockWaitStrategy::TimeoutBlockWaitStrategy()`

- q)的重载。
- `std::chrono::milliseconds` C11的时间对象，常用于定时之中。 https://blog.csdn.net/oncealong/article/details/28599655

#### t)类函数 `TimeoutBlockWaitStrategy::NotifyOne()`

- 

#### u)类函数 `TimeoutBlockWaitStrategy::EmptyWait()`

- 

#### v)类函数 `TimeoutBlockWaitStrategy::BreakAllWait()`

- 

#### w)类函数 `TimeoutBlockWaitStrategy::SetTimeout()`

- 