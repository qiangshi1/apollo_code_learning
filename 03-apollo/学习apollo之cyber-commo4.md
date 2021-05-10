# `cyber`

## 一: `common`

### 10. `global_data.h`&`global_data.cc`

#### a) `BUILD`
```
cc_library(
    name = "global_data",
    srcs = ["global_data.cc"],
    hdrs = ["global_data.h"],
    data = [
        "//cyber:cyber_conf",
    ],
    deps = [
        "//cyber/base:atomic_hash_map",
        "//cyber/base:atomic_rw_lock",
        "//cyber/common:environment",
        "//cyber/common:file",
        "//cyber/common:macros",
        "//cyber/common:util",
        "//cyber/proto:cyber_conf_cc_proto",
    ],
)
```

#### b) 类`GlobalData`

`global_data.h`&`global_data.cc`含有一个类。

这种`using`的用法可以关注一下：`using ::apollo::cyber::base::AtomicHashMap;`

#### c）类析构函数`GlobalData::~GlobalData`

常规函数。

#### d）类函数`GlobalData::ProcessId`

返回类成员`process_id_`。

#### e）类函数`GlobalData::SetProcessGroup`

设置字符串`process_group_`。

#### f）类函数`GlobalData::ProcessGroup`

返回`process_group_`。

#### g）类函数`GlobalData::ProcessGroup`

返回`process_group_`。

#### h）类函数`GlobalData::SetComponentNums`

设置`component_nums_`。

#### i）类函数`GlobalData::ComponentNums`

返回`component_nums_`。

#### j）类函数`GlobalData::SetSchedName`

设置`sched_name`。

#### k）类函数`GlobalData::SchedName`

返回`sched_name`。

#### l）类函数`GlobalData::HostIp`

返回`host_ip_`。

#### m）类函数`GlobalData::HostName`

返回`host_name_`。

#### n）类函数`GlobalData::EnableSimulationMode`

设置`run_mode_`，`run_mode_ = RunMode::MODE_SIMULATION`。

#### o）类函数`GlobalData::DisableSimulationMode`

设置`run_mode_`，`run_mode_ = RunMode::MODE_REALITY`。

#### p）类函数`GlobalData::IsRealityMode`

设置`run_mode_`，`run_mode_ = RunMode::MODE_REALITY`。

#### q）类函数`GlobalData::IsMockTimeMode`

Is Mock Time Mode：是模拟时间模式

设置`clock_mode_`，`clock_mode_ = ClockMode::MODE_MOCK`。

#### r）类函数`GlobalData::InitHostInfo`

- 初始化一些东西吧。
- 测试`gethostname`的一个`demo`：
    ```
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
- `unistd.h`为`Linux/Unix`系统中内置头文件，包含了许多系统服务的函数原型，例如`read`函数、`write`函数和`getpid`函数等。
其作用相当于`windows`操作系统的`windows.h`，是操作系统为用户提供的统一API接口，方便调用系统提供的一些服务。    
- 赋值`host_ip_`，如果环境变量`CYBER_IP`有值，把`CYBER_IP`赋给`host_ip_`；如果果环境变量`CYBER_IP`无值，当前设备IP赋给`host_ip_`。只支持IPv4(?)。
    
#### s）类函数`GlobalData::InitConfig`

读取`cyber.pb.conf`赋给`config_`。
    
#### t）类函数`GlobalData::Config`

返回`config_`。
    
#### u）类函数`GlobalData::RegisterNode`

- 由`node_name(string)`计算哈希值，更新哈希值与`node_name`的映射表。
- 以哈希值为`id`。
    
#### v）类函数`GlobalData::GetNodeById`

- 由`id`找`node_name`。

#### w）类函数`GlobalData::RegisterChannel`

- 逻辑与u）基本一样，只不过`node_name`换成了`channel`。

#### x）类函数`GlobalData::GetChannelById`

- 由`id`找`channel`。
 
#### y）类函数`GlobalData::RegisterService`

- 逻辑与u）基本一样，只不过`node_name`换成了`service`。

#### z）类函数`GlobalData::GetServiceById`

- 由`id`找`service`。

 
#### aa）类函数`GlobalData::RegisterTaskName`

- 逻辑与u）基本一样，只不过`node_name`换成了`task_name`。

#### ab）类函数`GlobalData::GetTaskNameById`

- 由`id`找`task_name`。
- 
#### ac）函数`program_path`

- 返回当前程序的绝对路径。
- `/proc/self/exe`是一个符号链接，代表当前程序的绝对路径.用`readlink`读取`/proc/self/exe`可以获取当前程序的绝对路径。一个demo：
    ```
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
    
    会打印：
    ```
    shiqiang@carbon:~/apollo/with_apollo/temp$ ./a.out 
    /home/shiqiang/apollo/with_apollo/temp/a.out
    ```
#### ad）类函数`GlobalData::GlobalData`

初始化许多东西。

### 11. `file_test.cc`

### 12. `environment_test.cc`





