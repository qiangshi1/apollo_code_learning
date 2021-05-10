## 代码总览

提供了两个方法, 一个是字符串转哈希, 另一个是返回枚举变量的值(int).

## 功能/知识

### 1. 方法`Hash()`

- 用到了`std::hash`, 哈希运算应该是生成一个数字签名. 
- `std::size_t`是`unsigned long`. 
- 为什么会有这种表达方法? `hash<string>{}(key);`
注意里边的大括号, 删除又会报错.


### 2. 方法`ToInt()`

- 这个方法用到了C++11的新特性: 尾置类型. 它相当于: 
    ```C++
    template <typename Enum>
    typename std::underlying_type<Enum>::type ToInt(Enum const value) {
        return static_cast<typename std::underlying_type<Enum>::type>(value);
    }
    ```
- `std::underlying_type`用于获取枚举变量的基础类型, 它的标准用法是`underlying_type<a_enum>::value`
- 所以这个方法的功能是返回当前枚举变量的值，如：
    ```c++
    enum Week {Sun, Mon, Tue, Wed, Thu, Fri, Sat}; // 定义枚举类型week
    Week today=Week::Fri;
    std::cout << "today: " << apollo::cyber::common::ToInt(today) << std::endl; //会输出: today: 5
    ```
- 为什么它要用尾置类型呢? 为什么它要把简单问题复杂化呢? 