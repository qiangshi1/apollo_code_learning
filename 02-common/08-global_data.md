## 代码总览

global_data.h和global_data.cc. 

维护了一个类: `GlobalData`. 

也是获取一些环境变量. 

## 功能/知识
### 1. 类方法`GlobalData::InitHostInfo()`
- 初始化一些东西吧. 
- 赋值`host_ip_`, 如果环境变量`CYBER_IP`有值, 把`CYBER_IP`赋给`host_ip_`; 如果果环境变量`CYBER_IP`无值, 当前设备IP赋给`host_ip_`. 只支持IPv4? 
- 测试`gethostname`的一个`demo`: 
    ```c++
    #include <unistd.h>
    #include <iostream>
    #include <string>
    int main() {
      char host_name[1024];
      gethostname(host_name, sizeof(host_name)); //返回hostname，我的是carbon
      std::cout<<host_name<<std::endl;
      return 0;
    }
    ```
- `unistd.h`为`Linux/Unix`系统中内置头文件, 包含了许多系统服务的函数原型, 例如`read`函数 `write`函数和`getpid`函数等. 
其作用相当于`windows`操作系统的`windows.h`, 是操作系统为用户提供的统一API接口, 方便调用系统提供的一些服务.     
    
- 
-

### 2. 方法`program_path()`
- 返回当前程序的绝对路径. 
- `/proc/self/exe`是一个符号链接, 代表当前程序的绝对路径. 用`readlink`读取`/proc/self/exe`可以获取当前程序的绝对路径. 一个demo: 
    ```c++
    #include <unistd.h>
    #include <iostream>
    int main(int argc, const char* argv[]) {
      char path[1025];
      auto len = readlink("/proc/self/exe", path, sizeof(path) - 1); 
      if (len == -1) {
        return 0;
      }
      path[len] = '\0';
      std::cout << path << std::endl; // /home/shiqiang/apollo/with_apollo/temp/a.out
      return 0;
    }
    ```
    会打印: 
    ```shell
    shiqiang@carbon:~/apollo/with_apollo/temp$ ./a.out 
    /home/shiqiang/apollo/with_apollo/temp/a.out
    ```
-

