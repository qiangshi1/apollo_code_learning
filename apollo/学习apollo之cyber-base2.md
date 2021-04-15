### 13. `bounded_queue.h`




#### a) `BUILD`的控制：

```
cc_library(
    name = "bounded_queue",
    hdrs = ["bounded_queue.h"],
    deps = [
        "//cyber/base:macros",
        "//cyber/base:wait_strategy",
    ],
)
```

#### b) 类`BoundedQueue`

- 你是干啥的?
- 总之,这个头文件还是不很理解.

#### c) 类成员`BoundedQueue::head_`/`tail_`/`commit_`.

- `alignas`是一个关键字,内存对齐,许多教程说的是跨平台,但我还是不是很理解.但是对于理解代码应该没有干扰.

#### d) 类成员`BoundedQueue::pool_size_`
-

#### e) 类成员`BoundedQueue::pool_`
-

#### f) 类成员`BoundedQueue::wait_strategy_`
- 初始化`pool_[pool_size_]`
- 智能指针,知道在哪里销毁它:
```
#include <iostream>
#include <memory>
struct Task {
    int mId;
    Task(int id ) :mId(id) {
        std::cout << "Task::Constructor" << std::endl;
    }
    ~Task() {
        std::cout << "Task::Destructor" << std::endl;
    }
};
int main()
{
    Task* taskPtr(new Task(23));
    int id = taskPtr->mId;
    std::cout << id << std::endl;
    return 0;
}
```
会输出:
```
Task::Constructor
23
```

```
#include <iostream>
#include <memory>
struct Task {
    int mId;
    Task(int id ) :mId(id) {
        std::cout << "Task::Constructor" << std::endl;
    }
    ~Task() {
        std::cout << "Task::Destructor" << std::endl;
    }
};
int main()
{
    Task* taskPtr(new Task(23));
    int id = taskPtr->mId;
    std::cout << id << std::endl;
    delete taskPtr;
    return 0;
}
```
会输出:
```
Task::Constructor
23
Task::Destructor
```

```
#include <iostream>
#include <memory>
struct Task {
    int mId;
    Task(int id ) :mId(id) {
        std::cout << "Task::Constructor" << std::endl;
    }
    ~Task() {
        std::cout << "Task::Destructor" << std::endl;
    }
};
int main()
{
    std::unique_ptr<Task> taskPtr(new Task(23));
    int id = taskPtr->mId;
    std::cout << id << std::endl;
    return 0;
}
```
会输出:
```
Task::Constructor
23
Task::Destructor
```
- `reset`,智能指针的赋值.

#### g) 类成员`BoundedQueue::break_all_wait_`

- `volatile`是一个关键字,防止编译器优化导致的多线程处理变量出错.

#### h) 构造函数`BoundedQueue::BoundedQueue()`
-

#### d) 两个`delete`

取消赋值和拷贝操作.

#### e) 析构函数`BoundedQueue<T>::~BoundedQueue()`

释放一些东西.

reinterpret_cast指的是类型转换,

用法： TYPE b = reinterpret_cast ( a )

TYPE必须是一个指针、引用、算术类型、函数指针.

详见:
https://blog.csdn.net/m0_45867846/article/details/107082464

#### f) 类函数`BoundedQueue<T>::Init()`

调用重载函数`BoundedQueue<T>::Init()`

#### g) 类函数`BoundedQueue<T>::Init()`

- f)的重载函数.
- `std::calloc`,分配内存,通常和`std::free`搭配使用.
- `new`的几个用法:
    - `int *p = new int(3);`
    - `int *q = new int[3];`
    - `new(p) A(3);`指定内存地址new,又称placement new.,但必须显式包含头文件`new`或者`new.h`,如:

```
#include<stdio.h>
#include<new> //必须显式包含,否则编译不通过
class A
{
   int i;
public:
   A(int _i) :i(_i*_i) {}
   void Say()  { printf("i=%d\n", i); }
};
int main()
{
   char s[sizeof(A)];
   A* p = (A*)s;
   new(p) A(3); 
   p->Say(); // i=9
}
```

#### h) 类函数`BoundedQueue<T>::Enqueue()`

- 字面意思是入队.
- `compare_exchange_weak`的底层逻辑不懂的话可以简单理解成`tail_=new_tail`,同时返回`true`.

```
tail_.compare_exchange_weak(old_tail, new_tail,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed)
```

#### i) 类函数`BoundedQueue<T>::Enqueue()`

-  h)的重载.
-  `cyber_unlikely` 是在`macros.h`的宏定义,它是:`#define cyber_likely(x) (__builtin_expect((x), 1))` 总之是为了编译时生成更高效的代码,虽然我也不知道为什么.参考: https://blog.csdn.net/qq_22660775/article/details/89028258
- `NotifyOne()` `wait_strategy.h`中定义.

#### j) 类函数`BoundedQueue<T>::Dequeue()`

-  字面意思是出队.

#### k) 类函数`BoundedQueue<T>::WaitEnqueue()`

-  等待入队,如果入队,返回;如果没有入队,等待.

#### k2) 类函数`BoundedQueue<T>::WaitEnqueue()`

-  k)的重载.


#### l) 类函数`BoundedQueue<T>::Size()`
-  队列的长度?

#### m) 类函数`BoundedQueue<T>::Empty()`
-  队列是否为空.


#### s) 类函数`BoundedQueue<T>::GetIndex()`
- 取余数,他还标注了比`%`要快,会吗?但我写了两个程序测试了下,感觉差不多哦.
- 

#### n) 类函数`BoundedQueue<T>::SetWaitStrategy()`
-  设置`wait_strategy_`.

#### o) 类函数`BoundedQueue<T>::BreakAllWait()`
-  中断所有的`WaitEnqueue`

#### p) 类函数`BoundedQueue<T>::Head()`
-  

#### q) 类函数`BoundedQueue<T>::Tail()`
-  

#### r) 类函数`BoundedQueue<T>::Commit()`
-  





















