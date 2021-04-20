# `cyber`

## 一: `common`

`cyber`的地位相当于`ros`的`ros.h`，`common`是`cyber`的一个子包。


### 1. `util.h`

#### a) `BUILD`

`BUILD`中是这样控制它的：

```
cc_library(
    name = "util",
    hdrs = ["util.h"],
)
```

它会生成链接库。

#### b) 内联函数`Hash`

用到了`std::hash`,哈希运算应该是生成一个数字签名，这里就是给出一个字符串的数字签名。

`std::size_t`是`unsigned long`。

为什么会有这种表达方法？`std::hash<std::string>{}(key);`
注意里边的大括号。删除又会报错。


#### c) 模板函数`ToInt`

这个函数用到了C++11的新特性：尾置类型。

它相当于：
```C++
template <typename Enum>
typename std::underlying_type<Enum>::type ToInt(Enum const value) {
  return static_cast<typename std::underlying_type<Enum>::type>(value);
}
```

另外，这个函数的功能是返回当前枚举变量的值，如：

```
enum Week {Sun, Mon, Tue, Wed, Thu, Fri, Sat}; // 定义枚举类型week
Week today=Week::Fri;
std::cout << "today: " << apollo::cyber::common::ToInt(today) << std::endl;
```

会返回：
```
today: 5
```

为什么它要把简单问题复杂化呢？

### 2. `types.h`

#### a) `BUILD`的控制方法：
```
cc_library(
    name = "types",
    hdrs = ["types.h"],
)
```
生成一个链接库。

#### b) 空类`NullType`

里边啥都没有。

#### c) 枚举变量`ReturnCode`

常规枚举变量。

#### d) 枚举变量`Relation`

enum可以指定底层类型：
```
enum Relation : std::uint8_t {
  NO_RELATION = 0,
  DIFF_HOST,  // different host
  DIFF_PROC,  // same host, but different process
  SAME_PROC,  // same process
};
```
注意这个`std::uint8_t`，是强制底层类型，如：

```
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

  system("pause");
}
```
会输出：
```
h
i
sh: 1: pause: not found
```

不用管第三行，`h`应该是`usigned int`，`i`应该是`int`（可能是编译器的问题）。

这个博客讲枚举变量的值得作用域的，讲得不错。注意`enum class`和`enum struct`.
https://blog.csdn.net/datase/article/details/82773937

#### e) 两个char[]常量：`SRV_CHANNEL_REQ_SUFFIX[]`和`SRV_CHANNEL_RES_SUFFIX[]`


### 3. `time_conversion.h`

这个是讲时间转换的。

#### a) 常量`std::vector<std::pair<int32_t, int32_t>> LEAP_SECONDS`

描述UNIX时间和GPS时间的闰秒映射关系。

#### b) 常量`UNIX_GPS_DIFF`

描述UNIX时间和GPS时间常量映射关系。

#### c) 常量`ONE_MILLION`

一百万

#### d) 常量`ONE_BILLION`

一亿

#### e) 模板函数`UnixToGpsSeconds`

UNIX时间（秒）转GPS时间（秒）。

#### f) 函数`UnixToGpsMicroseconds`

UNIX时间（毫秒）转GPS时间（毫秒）。

#### g) 函数`UnixToGpsNanoseconds`

UNIX时间（微秒）转GPS时间（微秒）。

#### h) 

#### i) 模板函数`GpsToUnixSeconds`

GPS时间（秒）转UNIX时间（秒）。


#### j) 函数`GpsToUnixMicroseconds`

GPS时间（毫秒）转UNIX时间（毫秒）。


#### k) 函数`GpsToUnixNanoseconds`

GPS时间（微秒）转UNIX时间（微秒）。

#### l) 函数`GpsToUnixMicroseconds`

无符号类型，j的重载。

#### m) 函数`GpsToUnixNanoseconds`

无符号类型，k的重载。

#### n) 函数`StringToUnixSeconds`

`string`转`UNIX`时间，默认`string`格式`"%Y-%m-%d %H:%M:%S"`，返回类型`uint64_t`。

#### o) 函数`UnixSecondsToString`

`UNIX`转`string`时间，默认`string`格式`"%Y-%m-%d %H:%M:%S"`。



