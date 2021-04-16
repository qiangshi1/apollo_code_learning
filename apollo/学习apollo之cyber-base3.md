### 14. `bounded_queue_test.cc`

`xx_test.cc`先不关注.


### 15. `concurrent_object_pool.h`

#### a) `BUILD`的控制：

```
cc_library(
    name = "concurrent_object_pool",
    hdrs = ["concurrent_object_pool.h"],
    deps = [
        "//cyber/base:for_each",
    ],
)
```

它是一个常规链接库.

#### b) 类`CCObjectPool`:

- `std::enable_shared_from_this`,c11新特性,两个智能指针指向同一个实体,https://blog.csdn.net/caoshangpa/article/details/79392878 写的蛮好的.
- 这个函数也不是很懂,先往下看吧.

#### c) 两个`using`
- 重命名,常规操作.
- 

#### d) 类构造函数`ObjectPool::ObjectPool()`

- `explicit`用在类构造函数之中,取消参数类型隐式变换,用于规范化代码.
- `template <typename... Args>`变参模板,如:
```
#include <iostream>
#include <string>
template<typename T>
T adder(T v) { //一定要有一个基本单元.也可能不一定.
  return v;
}
template<typename T, typename... Args>
T adder(T first, Args... args) {
  return first + adder(args...);
}
int main()
{
    std::cout<<adder(1,2,3,4)<<std::endl; //10
    std::cout<<adder('a','b','c','d')<<std::endl; //乱码
    std::cout<<adder(std::string("a"),std::string("b"),std::string("c"),std::string("d"))<<std::endl; //abcd
}
```

#### e) 类构造函数`ObjectPool::ObjectPool()`

- d)的重载.
- C11的右值引用.https://blog.csdn.net/with_dream/article/details/85137039 但感觉还是云里雾里的.
- `FOR_EACH`里边的东西还是云里雾里的.


#### f) 类析构函数`ObjectPool::~ObjectPool()`

- 虚函数.

- `std::free()`

#### g) 类函数`ObjectPool::GetObject()`

虚函数.

#### h) 类函数`ObjectPool::ReleaseObject()`

虚函数.

#### i) 类函数`ObjectPool::GetObject()`
- `this->shared_from_this()`?指向this的当前智能指针.
- 哈?

### 16. `object_pool_test.cc`
暂不关注.


### 17. `reentrant_rw_lock.h`

- reentrant 可再入的.
- 它的结构和`atomic_rw_lock`好像啊.


#### a) `BUILD`的控制：

```
cc_library(
    name = "reentrant_rw_lock",
    hdrs = ["reentrant_rw_lock.h"],
)
```

#### b) 友元类声明：

```
friend class ReadLockGuard<ReentrantRWLock>;
friend class WriteLockGuard<ReentrantRWLock>;
```

#### c) `static const`量
```
static const int32_t RW_LOCK_FREE = 0;
static const int32_t WRITE_EXCLUSIVE = -1;
static const uint32_t MAX_RETRY_TIMES = 5;
static const std::thread::id null_thread;
```

#### d) 类构造函数`ReentrantRWLock::ReentrantRWLock()`

#### e) 类构造函数`ReentrantRWLock::ReentrantRWLock()`
d)的重载


#### f) 类函数`ReentrantRWLock::ReadLock()`
d)的重载

### 18. `rw_lock_guard.h`

#### a) `BUILD`的控制：

```
cc_library(
    name = "rw_lock_guard",
    hdrs = ["rw_lock_guard.h"],
)
```
常规链接库.


#### b) 类`ReadLockGuard`

- 它是构造时加锁,析构时解锁.一行代码实现两行的功能;
- 取消拷贝初始化.


#### c) 类`WriteLockGuard`

- 和b)极其类似.













