### 19. `thread_pool.h`

线程池?

#### a) `BUILD`的控制：

```
cc_library(
    name = "thread_pool",
    hdrs = ["thread_pool.h"],
)
```
常规链接库.

#### b) 类`ThreadPool`

该头文件只有一个类.

#### b1) 类成员`ThreadPool::workers_`

线程vector。

#### b2) 类构造函数`ThreadPool::`task_queue_`

涉及到了`BoundedQueue`，但`BoundedQueue`还是不理解。

#### b3) 类构造函数`ThreadPool::stop_`

原子。

#### c) 类构造函数`ThreadPool::ThreadPool()`

- `std::vector`的`reserve()`.
`resize()`：改变的是`size()`与`capacity()`的大小,所以用完`resize()`之后一般用`at()`.
`reserve()`：改变的只是`capacity()`的大小，`size()`和内容不会改变,所以用完`reserve()`之后一般用`push_back()`或者`emplace_back()`.

- lambda表达式,的捕获`this`的规则. 所以`this`到底要不要显式地指明?我看thread_pool.h并没有显式地指明.
```
#include<iostream>
using namespace std;

struct Foo1 {
  int x = 1;

  void f(int x) {
    auto f = [x, this] { cout << x << endl; };
    f();
  }
};
struct Foo2 {
  int x = 1;

  void f(int x) {
    auto f = [this] { cout << this->x << endl; };
    // auto f = [this] { cout << x << endl; }; //编译报错
    f();
  }
};
struct Foo3 {
  int x = 1;

  void f(int x) {
    auto f = [x] { cout << x << endl; };
    f();
  }
};

int main(){
    Foo1 foo1;
    foo1.f(2); //2
    Foo2 foo2;
    foo2.f(2); //1
    Foo3 foo3;
    foo3.f(2); //2
}
```

- `std::thread`+`std::vector`+`push_back()`+`emplace_back()`的一些神奇现象:,,,,感觉`emplace_back()`比`push_back()`更支持隐式变换.

```c++
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <vector>
void f1()
{
    std::cout << "I'm f1."<<std::endl;
}
int main()
{
    std::thread a_t(f1);
    a_t.join();
    std::vector<std::thread> t_s;
    // t_s.push_back(f1); //编译不通过
    t_s.push_back(std::thread(f1)); //编译通过
    t_s.at(0).join();
    // t_s.push_back([]{std::cout<<"666"<<std::endl;}); //编译不通过
    t_s.push_back(std::thread([]{std::cout<<"I'm lambda."<<std::endl;})); //编译通过
    t_s.at(1).join();
    t_s.emplace_back(f1); //编译通过
    t_s.at(2).join();
    t_s.emplace_back(std::thread(f1)); //编译通过
    t_s.at(3).join();
    t_s.emplace_back(std::thread([]{std::cout<<"I'm lambda."<<std::endl;})); //编译通过
    t_s.at(4).join();
    t_s.emplace_back([]{std::cout<<"I'm lambda."<<std::endl;}); //编译通过
    t_s.at(5).join();
}
```

#### d) 类函数`ThreadPool::Enqueue()`

- 这个函数还是不懂
- 尾置类型
- `std::result_of`推断出函数的返回值类型,cpp reference里的例子挺好的:

```c++
// result_of example
#include <iostream>
#include <type_traits>
int fn(int) {return int();}                            // function
typedef int(&fn_ref)(int);                             // function reference
typedef int(*fn_ptr)(int);                             // function pointer
struct fn_class { int operator()(int i){return i;} };  // function-like class
int main() {
  typedef std::result_of<decltype(fn)&(int)>::type A;  // int
  typedef std::result_of<fn_ref(int)>::type B;         // int
  typedef std::result_of<fn_ptr(int)>::type C;         // int
  typedef std::result_of<fn_class(int)>::type D;       // int
  std::cout << std::boolalpha;
  std::cout << "typedefs of int:" << std::endl;
  std::cout << "A: " << std::is_same<int,A>::value << std::endl; // true
  std::cout << "B: " << std::is_same<int,B>::value << std::endl; // true
  std::cout << "C: " << std::is_same<int,C>::value << std::endl; // true
  std::cout << "D: " << std::is_same<int,D>::value << std::endl; // true
  return 0;
}
```

- `std::make_shared`智能指针的创建方法。cpp reference的例子：

```
#include <iostream>
#include <memory>
int main () {
  std::shared_ptr<int> foo = std::make_shared<int> (10);
  // same as:
  std::shared_ptr<int> foo2 (new int(10));
  auto bar = std::make_shared<int> (20);
  auto baz = std::make_shared<std::pair<int,int>> (30,40);
  std::cout << "*foo: " << *foo << '\n';
  std::cout << "*bar: " << *bar << '\n';
  std::cout << "*baz: " << baz->first << ' ' << baz->second << '\n';
  return 0;
}
```



- `std::packaged_task`

  - `std::promise`/`std::future`/`std::packaged_task` 这篇文章写得很漂亮：https://zhuanlan.zhihu.com/p/61311187 我们按照它的实现以下：

    ```C++
    // std::promise和std::future搭配使用，用于线程之间的数据传递
    #include <chrono>
    #include <future>  // 引入future所在的头文件
    #include <iostream>
    #include <string>
    #include <thread>
    using namespace std;
    class Food {
     public:
      Food() {}  // 默认构造函数
      // 通过菜名构建Food对象
      Food(string strName) : m_strName(strName) {}
      // 获取菜名
      string GetName() const { return m_strName; }
     private:
      string m_strName;  // 菜名
    };
    // 线程函数
    // 根据菜名创建Food对象，并通过promise对象返回结果数据
    void Cook(const string strName, promise<Food>& prom) {
      //为了突出效果，可以使线程休眠5s
      std::this_thread::sleep_for(std::chrono::seconds(5));
      // 做菜…
      Food food(strName);
      // 将创建完成的food对象放到promise传递出去
      prom.set_value(food);
    }
    int main() {
      // 用于存放结果数据的promise对象
      promise<Food> prom;
      // 获得promise所关联的future对象
      future<Food> fu = prom.get_future();
      // 创建分支线程执行Cook()函数
      // 同时将菜名和用于存放结果数据的promise对象传递给Cook()函数
      // ref()函数用于获取promise对象的引用
      thread t(Cook, "回锅肉", ref(prom));
      // 等待分支线程完成Food对象的创建，一旦完成，立即获取完成的Food对象
	  //可以选择使用wait_for限制主线程等待时间        
      Food food = fu.get(); //等待 prom.set_value(food);执行完成
      // 上菜
      cout << "客官，你点的" << food.GetName() << "来了，请慢用！" << endl;
      t.join();  // 等待分支线程最终完成
      return 0;
    }
    ```

    ```c++
//为了减少码量，C++提供了`packaged_task`
    // std::promise和std::future搭配使用，用于线程之间的数据传递
    #include <chrono>
    #include <future>  // 引入future所在的头文件
    #include <iostream>
    #include <string>
    #include <thread>
    using namespace std;
    class Food {
     public:
      Food() {}  // 默认构造函数
      // 通过菜名构建Food对象
      Food(string strName) : m_strName(strName) {}
      // 获取菜名
      string GetName() const { return m_strName; }
     private:
      string m_strName;  // 菜名
    };
    // 线程函数
    // 根据菜名创建Food对象，并通过promise对象返回结果数据
    Food Cook(const string strName) {
      //为了突出效果，可以使线程休眠5s
      std::this_thread::sleep_for(std::chrono::seconds(5));
      // 做菜…
      Food food(strName);
      return food;
    }
    int main() {
      // 使用线程函数的返回值和参数类型特化packaged_task类模板
      // 利用其构造函数，将线程函数打包成一个packaged_task对象
      packaged_task<Food(string)> cooker(Cook); //传入Cook函数: Food(string)
      // 从packaged_task对象获得与之关联的future对象
      future<Food> fu = cooker.get_future();
      // 创建线程执行packaged_task对象，实际上执行的是Cook()函数
      // 这里也不再需要传递promise对象
      thread t(move(cooker), "回锅肉"); 
      // 同样地获得结果数据
      Food food = fu.get();
      cout << "客官，你点的" << food.GetName() << "来了，请慢用！" << endl;
      t.join();
      return 0;
    }
    ```
    
    ```c++
    // 为了进一步减少码量，C++提供了`async`
    // std::promise和std::future搭配使用，用于线程之间的数据传递
    #include <chrono>
    #include <future>  // 引入future所在的头文件
    #include <iostream>
    #include <string>
    #include <thread>
    using namespace std;
    class Food {
     public:
      Food() {}  // 默认构造函数
      // 通过菜名构建Food对象
      Food(string strName) : m_strName(strName) {}
      // 获取菜名
      string GetName() const { return m_strName; }
     private:
      string m_strName;  // 菜名
    };
    // 线程函数
    // 根据菜名创建Food对象，并通过promise对象返回结果数据
    Food Cook(const string strName) {
      //为了突出效果，可以使线程休眠5s
      std::this_thread::sleep_for(std::chrono::seconds(5));
      // 做菜…
      Food food(strName);
      return food;
    }
    int main() {
      // 将Cook()函数异步（async）执行
        future<Food> fu = async(bind(Cook,"回锅肉"));
        cout<<"客官，你点的"<<fu.get().GetName()<<"来了，请慢用！"<<endl;
      return 0;
    }
    ```
    
    `std::promise`/`std::future`/`std::packaged_task` 都是线程之间数据传递用的.
    
    
  
- `  *auto* task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(*f*), std::forward<Args>(*args*)...));`这句话还是不甚理解。

  

#### c) 类析构函数`ThreadPool::~ThreadPool()`

- 原子的`exchange`，赋值吗
- 为什么析构函数里边有线程的`join()`?















