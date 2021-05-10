## 代码总览

environment.h和environment_test.cc

读取一些环境变量. 

## 功能/知识

### 1. 方法`GetEnv()`
- `GetEnv()`会查找环境变量`${var_name}`. 若`${var_name}`非空, 返回`${var_name}`; 若`${var_name}`为空, 返回`default_value`.`default_value`可以作为输入传入, 默认为`""`. 
- `std::getenv()`返回操作系统环境变量. 一个demo: 
    ```c++
    #include <iostream>
    #include <cstdlib>
    int main()
    {
        if(const char* env_p = std::getenv("PATH"))
            std::cout << "Your PATH is: " << env_p << '\n';
    }
    ```
    我的系统返回: 
    ```shell
    Your PATH is: /opt/ros/kinetic/bin:/home/shiqiang/bin:/home/shiqiang/.local/bin:/home/shiqiang/bin:/home/shiqiang/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/home/shiqiang/bin:/home/shiqiang/bin
    ```
### 2. 方法`WorkRoot()`
- 获取环境变量`${CYBER_PATH}`. 若`${CYBER_PATH}`非空, 返回`${CYBER_PATH}`; 若`${CYBER_PATH}`为空,返回"/apollo/cyber". 
-

