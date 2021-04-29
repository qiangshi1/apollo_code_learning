### data2. `data_notifier.h`

- 详细方法参考: 学习apollo之cyber-data2.html
- 维护了一个类: DataNotifier, 

#### ①.宏定义`DECLARE_SINGLETON(DataNotifier)`

- 它的展开是:

- ```C++
   public:                                                                  
    static DataNotifier *Instance(bool create_if_needed = true) {              
      static DataNotifier *instance = nullptr;                                 
      if (!instance && create_if_needed) {                                  
        static std::once_flag flag;                                         
        std::call_once(flag, [&] { instance = new (std::nothrow) DataNotifier(); }); 
      }                                                                     
      return instance;                                                      
    }                                                             
    static void CleanUp() {                                                 
      auto instance = Instance(false);                                      
      if (instance != nullptr) {                                            
        CallShutdown(instance);                                            
      }                                                                     
    }                                                                       
   private:                                                                 
    DataNotifier();                                                            
    DataNotifier(const DataNotifier &) = delete;    
    DataNotifier &operator=(const DataNotifier &) = delete;
  ```

- 这是一个单例模式的写法, 它的作用是在一个系统中一个类只能被实例一次.

- 写单例模式的范式是:构造函数私有; 公有静态函数获取指向该实例的指针; 析构函数公有.

