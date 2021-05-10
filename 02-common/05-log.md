## 代码总览

log.h和log_test.cc

一些定义打印语句的宏
xxxxxx

## 功能/知识

### 1. 宏定义`MODULE_NAME`
- 调用`cyber/binary.h`的`GetName()`. `cyber/binary.h`中维护了一个`name(string)`, 通过`GetName()`得到它. 

### 2. 宏定义`ADEBUG_MODULE`
- `VLOG(4)`在`glog`中定义的. 字面阅读的话应该是定义了一个警告的等级. 

### 3. 宏定义`RETURN_IF_NULL`
- `#if !defined`等价于`#ifndef`吗? 
- 宏中`#`的用法，阻止宏展开, 如: 
    ```c++
    #include <iostream>
    #include <thread>
    #include <mutex>
    #define COUT_A(a) std::cout<<#a<<": "<<a<<std::endl;
    int main()
    {
    std::string Im_string="Im a.";
    COUT_A(Im_string)
    }
    ```

#### d) 宏定义`ADEBUG_MODULE`
`<<`连接的字符串。估计是用到一些文本的打印过程中。

`VLOG(4)`在`glog`中定义的。字面阅读的话应该是定义了一个详细的等级。


