### x2. `perf_event_cache.h&.cc`

- 链接库
- 详细方法参考: 学习apollo之cyber-event2.html
- 维护了一个类,该类在读取了一些配置量后操作event_queue_?

#### ①.类之中成员变量直接赋值: `PerfEventCache::PerfEventCache()`

- .cc文件中:为什么在头文件中没有声明,这里却可以直接定义?

- `global_conf = GlobalData::Instance()->Config()`在global_data.h&cc中, 返回的是一个proto(perf_conf.proto)对象,下代码global_data.h&cc中是读取proto(config_)的对应的代码, 默认读取路径是`$WorkRoot/conf/cyber.pb.conf`, 

- ```c++
  auto config_path = GetAbsolutePath(WorkRoot(), "conf/cyber.pb.conf");
  if (!GetProtoFromFile(config_path, &config_)) {...}
  ```

- 奇特的是如果打开对应的proto文件,内容是:

- ```protobuf
  # transport_conf {
  #     shm_conf {
  #         # "multicast" "condition"
  #         notifier_type: "condition"
  #         # "posix" "xsi"
  #         shm_type: "xsi"
  #         shm_locator {
  #             ip: "239.255.0.100"
  #             port: 8888
  #         }
  #     }
  #     participant_attr {
  #         lease_duration: 12
  #         announcement_period: 3
  #         domain_id_gain: 200
  #         port_base: 10000
  #     }
  #     communication_mode {
  #         same_proc: INTRA
  #         diff_proc: SHM
  #         diff_host: RTPS
  #     }
  #     resource_limit {
  #         max_history_depth: 1000
  #     }
  # }
  run_mode_conf {
      run_mode: MODE_REALITY
      clock_mode: MODE_CYBER
  }
  scheduler_conf {
      routine_num: 100
      default_proc_num: 16
  }
  ```

- 并没有`if (global_conf.has_perf_conf())`中需要的`perf_conf`字段, 难道它加载的是别的文件? 

#### ②`flush()`

- 参考https://blog.csdn.net/caoshangpa/article/details/78920823
- 文件的写过程: 缓冲区->存储, `flush()`的过程是把数据从缓冲区写到存储, 按`kFlushSize`调用`flush()`的好处是降低
  读写的CPU占用?