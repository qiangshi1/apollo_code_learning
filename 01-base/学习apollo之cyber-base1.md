# `cyber`

## 一: `base`

### 10. `atomic_hash_map.h`

#### a) `BUILD`

```
cc_library(
    name = "atomic_hash_map",
    hdrs = ["atomic_hash_map.h"],
)
```

#### b) 模板类`AtomicHashMap`

- 它主要是维护了一个链表吧？
- 它的模板花样好多，用`std::enable_if`强制一个模板类型的demo如下：
    ```
    #include <atomic>
    #include <cstdint>
    #include <type_traits>
    #include <utility>
    template <typename K,typename std::enable_if<std::is_integral<K>::value,int>::type = 0>
    class A_CLASS {
        public:
        int a_int;
    };
    int main(int argc, const char* argv[]) {
      A_CLASS<int> a_class; //可以通过编译
    //   A_CLASS<double> a_class; //无法通过编译
      return 0;
    }
    ```
- 另，这个编译可以通过，但我还是不知道它的意义何在：
    ```
    #include <atomic>
    #include <cstdint>
    #include <type_traits>
    #include <utility>
    template <typename K,int = 0>
    class A_CLASS {
        public:
        int a_int;
    };
    int main(int argc, const char* argv[]) {
      A_CLASS<int> a_class; 
      return 0;
    }
    ```
    好像知道了。
    
- 另：
    ```
    #include <atomic>
    #include <cstdint>
    #include <type_traits>
    #include <utility>
    // template <typename K,typename std::enable_if<128,int>::type = 0>  //A_CLASS<int> a_class; 可以通过编译
    // template <typename K,int I,typename std::enable_if<128,int>::type = 0>  //A_CLASS<int,0> a_class; 可以通过编译
    // template <typename K,int I=8,typename std::enable_if<128,int>::type = 0>  //A_CLASS<int> a_class;A_CLASS<int,0> a_class;均可以通过编译
    template <typename K,int I=8,typename std::enable_if<I,int>::type = 0>  //A_CLASS<int> a_class;A_CLASS<int,1> a_class;可以通过编译.A_CLASS<int,0> a_class;无法通过编译
    class A_CLASS {
        public:
        int a_int;
    };
    int main(int argc, const char* argv[]) {
      A_CLASS<int> a_class; 
      // A_CLASS<int,0> a_class; 
      // A_CLASS<int,1> a_class; 
      return 0;
    }
    ```
    
#### c) 构造函数`AtomicHashMap::AtomicHashMap`

常规构造。

#### d) = delete `AtomicHashMap(const AtomicHashMap &other) = delete;AtomicHashMap &operator=(const AtomicHashMap &other) = delete;`

禁止拷贝初始化。

#### e) 函数`Has`

和`mode_num_`按位与之后判断`key`是否存在。

#### f) 函数`Get`

和`mode_num_`按位与之后找`key`。

#### g) 函数`Get`

f)的重载。

它这个可以识别常量和变量的输入，不知道是什么机制，如：
```
apollo::cyber::base::AtomicHashMap<int, int> map;
int j=5;
map.Set(i*256, j);  //  和下边调用的函数不一样
map.Set(i*256, 5);  //  和上边调用的函数不一样

```


操纵的对象是一个`Hash`表（`table_`），首先索引到当前`index`：`index = key & mode_num_;`，这个没什么好说的，一行代码的事，然后操纵`table_[index]`.
`Set`是插入操作。对于`table_[index]`，它是有序的，按序查找是否有`key`，如果有，逻辑我还不太懂；如果没有，在当前的`key`，和下一个`key`之间插入，以保证有序性，另如果当前是最后一个，把下一个置成`nullptr`，这样也可以保证有序性。

总之，它应该是一个标准的`hash`表， 加上了一些线程保护的机制。

#### h) 函数`Set`

获取`key`。

#### i) 函数`Set`

h)的重载。

#### j) 函数`Set`

h)的重载。

e)~h)的一些小知识：
- `**` 指针的指针 
- `&&` 右值引用
- `std::forward<V>` 完美转发，https://blog.csdn.net/coolwriter/article/details/80970718

#### k) 结构体`Entry`

一个链表的子单元吧，原子保护多线程。

#### l) 构造函数`Entry::Entry`
常规。

#### m) 构造函数`Entry::Entry`
l)的重载，用到了`std::atomic`，关于多线程资源竞争还是不是很理解。有机会再看看吧。

#### n) 构造函数`Entry::Entry`
l)的重载。为什么原子量要用`store`赋值呢？直接等于不好吗？

#### o) 析构函数`Entry::~Entry`

#### p) 成员`K key`，成员`std::atomic<V *> value_ptr`，`std::atomic<Entry *> next`

#### q) 结构体`Bucket`

#### r) 构造函数`Bucket::Bucket`

初始化`head_`，`head_`是`Entry`的实例。

#### s) 析构函数`Bucket::~Bucket`

逐个子单元删除`head_`。

#### t) 类函数`Bucket::Has`

查`head_`中是否含有`key`，`head_`的自单元似乎是以`key`从小到达排列的。

#### u) 类函数`Bucket::Find`

- 查`head_`中是否含有`key`的`Entry`的实例，为什么有两个返回？：`prev_ptr`和`target_ptr`。
- `key`似乎是有向的。`target_ptr`满足`target_ptr->key == key`,`prev_ptr`满足`prev_ptr->next->key == key`，即`prev_ptr->next==target_ptr`。


#### v) 类函数`Bucket::Insert`

`Insert`字面上是插入，但看里边的逻辑好复杂。

#### w) 类函数`Bucket::Insert`

v)的重载。

#### x) 类函数`Bucket::Get`

`Get`函数为什么会返回布尔值？

#### y) 成员变量`table_`，`capacity_`，`mode_num_`

```
Bucket table_[TableSize];
uint64_t capacity_;
uint64_t mode_num_;
```
`TableSize`在模板中声明，默认是128，`capacity_`的值是`TableSize`，`mode_num_`的值是`TableSize-1`，


### 11. `atomic_hash_map_test.cc`

### a) `BUILD`的控制：

```
cc_test(
    name = "atomic_hash_map_test",
    size = "small",
    srcs = ["atomic_hash_map_test.cc"],
    deps = [
        "//cyber/base:atomic_hash_map",
        "@com_google_googletest//:gtest_main",
    ],
)
```

test暂不关注。

### 12. `atomic_rw_lock.h`

原子读写锁。似乎是用原子模拟一个锁,但c++11自带的锁不能满足读写的某种特性,所以它只能自己写一个.

#### a) `BUILD`的控制：

```
cc_library(
    name = "atomic_rw_lock",
    hdrs = ["atomic_rw_lock.h"],
    deps = [
        "//cyber/base:rw_lock_guard",
    ],
)
```

#### b) 类`AtomicRWLock`

- 友元类，我信任你，所以你可以访问我的私处。主要用到了关键字`frient`。一个demo：
    ```
    class CCar
    {
    private:
        int price;
        friend class CDriver;  //声明 CDriver 为友元类
    };
    class CDriver
    {
    public:
        CCar myCar;
        void ModifyCar()  //改装汽车
        {
            myCar.price += 1000;  //因CDriver是CCar的友元类，故此处可以访问其私有成员
        }
    };
    int main()
    {
        return 0;
    }
    ```
    
另:大括号可以用于初始化.
```
int a(5);
std::cout<<a<<std::endl;    // 5
int b{6};
std::cout<<b<<std::endl;    // 6
```
另:`std::this_thread::get_id()`指的是获取当前线程

#### c) 两个友元类声明：

```
friend class ReadLockGuard<AtomicRWLock>;
friend class WriteLockGuard<AtomicRWLock>;
```

还有带模板的写法哈，

#### d) 3个常量

```
static const int32_t RW_LOCK_FREE = 0;
static const int32_t WRITE_EXCLUSIVE = -1;
static const uint32_t MAX_RETRY_TIMES = 5;
```
可以按字面意思理解它们。


#### e) 构造函数`AtomicRWLock::AtomicRWLock`

关键字`explicit`，构造函数中进制隐式类型转换。


#### f) 类函数`AtomicRWLock::ReadLock`

- `std::this_thread::yield();`说是save cpu，但逻辑似乎还是不是很。。。
- 

#### g) 类函数`AtomicRWLock::ReadUnlock`

- 
 
#### h) 类函数`AtomicRWLock::WriteUnlock`

- 








