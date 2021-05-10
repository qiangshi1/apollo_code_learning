## 代码总览

提供了一些枚举变量和静态常量. 用于一些标识符? 

## 功能/知识


### 1. 枚举变量`Relation`

- enum可以指定底层类型, 如下. 注意这个`std::uint8_t`, 是强制底层类型.
  ```c++
  enum Relation : std::uint8_t {
    NO_RELATION = 0,
    DIFF_HOST,  // different host
    DIFF_PROC,  // same host, but different process
    SAME_PROC,  // same process
  };
  ```
- 又如: 
  ```c++
  #include <iostream>
  #include <string>
  #include <typeinfo>
  enum class Relation1 : std::uint8_t {
    NO_RELATION = 0,
    DIFF_HOST, 
    DIFF_PROC,  
    SAME_PROC,  
  };
  enum class Relation2{
    NO_RELATION = 0,
    DIFF_HOST,  
    DIFF_PROC,  
    SAME_PROC,  
  };
  int main(int argc, char* argv[]) {
    std::cout << typeid(std::underlying_type<Relation1>::type).name() << std::endl;
    std::cout << typeid(std::underlying_type<Relation2>::type).name() << std::endl;
  }
  ```
  它会输出: 
  ```shell
  h
  i
  ```
  `h`应该是`usigned int`,`i`应该是`int`. 可能是编译器简写了.
- 这个博客讲枚举变量的值得作用域的, 讲得不错. 注意`enum class`和`enum struct`.
https://blog.csdn.net/datase/article/details/82773937




