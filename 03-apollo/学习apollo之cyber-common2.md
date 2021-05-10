# `cyber`

## 一: `common`

### 4. `macros.h`

#### a) `BUILD`

`BUILD`中的控制方法：

```
cc_library(
    name = "macros",
    hdrs = ["macros.h"],
    deps = [
        "//cyber/base:macros",
    ],
)
```

依赖base下变的macros，生成链接库。


#### b) 宏函数`DEFINE_TYPE_TRAIT`

在base/macros下声明。

它展开应该是这个样子的:

```
template <typename T>                                   
struct HasShutdown {                                           
template <typename Class>                             
static constexpr bool Test(decltype(&Class::Shutdown)*) { 
  return true;                                        
}                                                     
template <typename>                                   
static constexpr bool Test(...) {                     
  return false;                                        
}                                                     
                                                      
static constexpr bool value = Test<T>(nullptr);       
};                                                      
                                                      
template <typename T>                                   
constexpr bool HasShutdown<T>::value;
```


需要说明的是：

`constexpr`常量修饰符，作用有二,1告诉编译器做优化；2初始化后限制修改。

讲`decltype`的，很有意思，https://blog.csdn.net/helloworld19970916/article/details/82935374

类的静态成员的使用要注意初始化，否则编译不通过。

```
#include <iostream>
#include <string>
#include <typeinfo>
class ClassWithShutdown {
    public:
        static int foo_;
        static void set_foo(int val) { 
        foo_ = val;
    }
};
//编译通过
int ClassWithShutdown::foo_ = 0;
int main(int argc, char* argv[]) {
  ClassWithShutdown::set_foo(1);
}
```

```
#include <iostream>
#include <string>
#include <typeinfo>
class ClassWithShutdown {
    public:
        static int foo_;
        static void set_foo(int val) { 
        foo_ = val;
    }
};
//编译不通过
// int ClassWithShutdown::foo_ = 0;
int main(int argc, char* argv[]) {
  ClassWithShutdown::set_foo(1);
}
```

`decltype`判断类型的用法，和类、类的成员混用下。
```
#include <iostream>
#include <string>
#include <typeinfo>

class AClass {
 public:
  int a_public_int;
  double a_public_double;
  std::string a_public_string;
  void a_public_function() { std::cout<<"I'm a public function."<<std::endl; }
  static int a_static_public_int;
  static double a_static_public_double;
  static std::string a_static_public_string;
  static void a_static_public_function() { std::cout><<"I'm a static public function."<<std::endl; }
 private:
  int a_private_int;
  double a_private_double;
  std::string a_private_string;
  void a_private_function() { std::cout><<"I'm a private function."<<std::endl; }
  static int a_static_private_int;
  static double a_static_private_double;
  static std::string a_static_private_string;
  static void a_static_private_function() { std::cout><<"I'm a static private function."<<std::endl; }
};
int a_normal_function(){
  return 1;
}
int a_normal_function2(double a_d,double a_d2){
  return 1;
}
// 静态成员必须在定义类的文件中对静态成员变量进行初始化，否则会编译出错。
int AClass::a_static_public_int = 0;
double AClass::a_static_public_double = 0.0;
std::string AClass::a_static_public_string = "0";
int AClass::a_static_private_int = 0;
double AClass::a_static_private_double = 0.0;
std::string AClass::a_static_private_string = "0";
int main(int argc, char* argv[]) {
  std::cout><<typeid(int).name()<<std::endl;  //i
  std::cout><<typeid(int *).name()<<std::endl;  //Pi
  std::cout><<typeid(double).name()<<std::endl;  //d
  std::cout><<typeid(double *).name()<<std::endl;  //Pd
  std::cout><<typeid(std::string).name()<<std::endl;  //NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  std::cout><<typeid(std::string *).name()<<std::endl;  //PNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  std::cout><<typeid(AClass).name()<<std::endl;  //6AClas
  std::cout><<typeid(decltype(&AClass::a_public_int) ).name()<<std::endl;  //M6AClassi
  std::cout><<typeid(decltype(&AClass::a_public_int)*).name()<<std::endl;  //PM6AClassi
  std::cout><<typeid(decltype(&AClass::a_public_double) ).name()<<std::endl;  //M6AClassd
  std::cout><<typeid(decltype(&AClass::a_public_double)*).name()<<std::endl;  //PM6AClassd
  std::cout><<typeid(decltype(&AClass::a_public_string) ).name()<<std::endl;  //M6AClassNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  std::cout><<typeid(decltype(&AClass::a_public_string)*).name()<<std::endl;  //PM6AClassNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  std::cout><<typeid(decltype(&AClass::a_public_function) ).name()<<std::endl;  //M6AClassFvvE
  std::cout><<typeid(decltype(&AClass::a_public_function)*).name()<<std::endl;  //PM6AClassFvvE
  std::cout><<typeid(decltype(&AClass::a_static_public_int) ).name()<<std::endl;  //Pi
  std::cout><<typeid(decltype(&AClass::a_static_public_int)*).name()<<std::endl;  //PPi
  std::cout><<typeid(decltype(&AClass::a_static_public_double) ).name()<<std::endl;  //Pd
  std::cout><<typeid(decltype(&AClass::a_static_public_double)*).name()<<std::endl;  //PPd
  std::cout><<typeid(decltype(&AClass::a_static_public_string) ).name()<<std::endl;  //PNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  std::cout><<typeid(decltype(&AClass::a_static_public_string)*).name()<<std::endl;  //PPNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  std::cout><<typeid(decltype(&AClass::a_static_public_function) ).name()<<std::endl;  //PFvvE
  std::cout><<typeid(decltype(&AClass::a_static_public_function)*).name()<<std::endl;  //PPFvvE
  std::cout><<typeid(decltype(AClass::a_static_public_function)*).name()<<std::endl;  //PFvvE
  // std::cout><<typeid(decltype(&AClass::a_private_int) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_int)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_double) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_double)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_string) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_string)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_function) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_private_function)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_int) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_int)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_double) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_double)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_string) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_string)*).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_function) ).name()<<std::endl;  //编译不通过
  // std::cout><<typeid(decltype(&AClass::a_static_private_function)*).name()<<std::endl;  //编译不通过
  std::cout><<typeid(decltype(a_normal_function)).name()<<std::endl;  //FivE
  std::cout><<typeid(decltype(a_normal_function2)).name()<<std::endl;  //FiddE
}
```

`C++11`中`constexpr`的限制：

``` 
constexpr int func (int x) 
{
    if (x><0)                
        x = -x;
    return x; // 编译不通过 
}
```

```
constexpr int func (int x) { return x < 0 ? -x : x; }   //编译通过
```

此限制已在C++14中接解除。
参考 https://www.qedev.com/dev/120195.html


该宏的用处应该是判断是否有对应的函数(`Shutdown()`),但是底层逻辑还是搞不懂。一个可以参考的测试demo如下：

```
#include <iostream>
#include <string>
#include <typeinfo>
template <typename T>
struct HasShutdown {
template <typename Class>
static constexpr bool Test(decltype(&Class::Shutdown)*) {
    return true;
}
template <typename>
static constexpr bool Test(...) {
    return false;
}
static constexpr bool value = Test<T>(nullptr);
};
template <typename T>
constexpr bool HasShutdown<T>::value;

class ClassWithShutdown {
 public:
  void Shutdown() { set_foo(1);}

  static int foo() { return foo_; }
  static void set_foo(int val) { foo_ = val; }
 private:
  static int foo_;
};

class ClassWithoutShutdown {
 public:
  void Shutdown_invalid() { set_foo(1);}

  static int foo() { return foo_; }
  static void set_foo(int val) { foo_ = val; }

 private:
  static int foo_;
};
int main(int argc, char* argv[]) {
  std::cout<<HasShutdown><ClassWithShutdown>::Test<ClassWithShutdown>(nullptr )<<std::endl;
  std::cout><<HasShutdown<ClassWithShutdown>::value<<std::endl;
  std::cout><<HasShutdown<ClassWithoutShutdown>::Test<ClassWithoutShutdown>(nullptr )<<std::endl;
  std::cout><<HasShutdown<ClassWithoutShutdown>::value<<std::endl;
}
```

会输出：
```
1
1
0
0
```

#### c) 模板函数`CallShutdown`

`typename`的用法：

1. 在模板定义语法中关键字`class`与`typename`的作用完全一样。
2. `typename    T::const_iterator it(proto.begin());`告诉编译器`T::const_iterator`是类型而不是变量。

参考 https://blog.csdn.net/lyn631579741/article/details/110730145


这个函数的用法应该是如果类型中有Shutdown，调用它。如果没有，编译报错。

一个demo：


```
#include ><iostream>
#include <string>
#include <typeinfo>

template <typename T>
struct HasShutdown {
template <typename Class>
static constexpr bool Test(decltype(&Class::Shutdown)*) {
    return true;
}

template <typename>
static constexpr bool Test(...) {
    return false;
}
static constexpr bool value = Test<T>(nullptr);
};
template <typename T>
constexpr bool HasShutdown<T>::value;

class ClassWithShutdown {
 public:
  void Shutdown() { std::cout<<"Shutdown"<<std::endl; }

  static int foo() { return foo_; }
  static void set_foo(int val) { foo_ = val; }
 private:
  static int foo_;
};

class ClassWithoutShutdown {
 public:
  void Shutdown_invalid() { std::cout><<"Shutdown invalid"<<std::endl; }

  static int foo() { return foo_; }
  static void set_foo(int val) { foo_ = val; }

 private:
  static int foo_;
};

template ><typename ClassWithShutdown>
typename std::enable_if<HasShutdown><ClassWithShutdown>::value>::type CallShutdown(ClassWithShutdown *instance) {
  instance->Shutdown();
}

int main(int argc, char* argv[]) {
  ClassWithShutdown class_with_shutdown;
  CallShutdown(&(class_with_shutdown)); //可以通过编译，运行会打印 Shutdown
  ClassWithoutShutdown class_without_shutdown;
  // CallShutdown(&(class_without_shutdown)); //不会通过编译
}
```

另外；

`typename std::enable_if<bool,T>::type` 的类型是`T`，
`typename std::enable_if<bool,>::type` 的类型是`void`，所以`typename` `std::enable_if<HasShutdown><ClassWithShutdown>::value>::type`的类型是`vold`。

#### d) 模板函数`CallShutdown`

c）的重载。

这个函数的用法应该是如果类型中有`Shutdown`，编译报错。如果没有，把一个类转移成`void`，销毁它？

#### e) 一些宏定义取消

#### f) 宏函数`#define UNUSED(param) (void)param`

转为void类型到底意味着什么？

#### g) 宏函数`#define DISALLOW_COPY_AND_ASSIGN(classname)`

下代码指禁止通过拷贝初始化对象，编译时期检查，两种拷贝方式：a(b),和a=b。
```
classname(const classname &) = delete;
classname &operator=(const classname &) = delete;
```
一个`demo`：

```
class classname1 {
 public:
  int a_num;
  classname1() {}
};

class classname2 {
 public:
  int a_num;
  classname2() {}
  classname2(const classname2 &) = delete;
};

class classname3 {
 public:
  int a_num;
  classname3() {}
  classname3(const classname3 &) = delete;
  classname3 &operator=(const classname3 &) = delete;
};

int main(int argc, char *argv[]) {
  classname1 ca1;
  classname2 ca2;
  classname3 ca3;

  classname1 ca1_copy(ca1);
  // classname2 ca2_copy(ca2); //无法通过编译
  // classname3 ca3_copy(ca3); //无法通过编译

  classname1 ca1_copy2;
  ca1_copy2 = ca1;
  classname2 ca2_copy2;
  ca2_copy2 = ca2;
  classname3 ca3_copy2;
  // ca3_copy2 = ca3; //无法通过编译
}
```

所以这个函数的功能如其名称，禁止拷贝或者拷贝形式的初始化。








