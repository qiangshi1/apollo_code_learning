### x1. `perf_event.h`

- 链接库
- 详细方法参考: 学习apollo之cyber-event1.html
- 不论是基类还是派生类,都维护了简单的成员变量/读写方法,似乎维护着一些标志位

#### ①.类之中成员变量直接赋值: `int cr_state_ = 1;`

- 有这种写法,下局一个例子:

```C++
#include<iostream>  
#include<string> 
class AC{
    public:
    std::string a_public_string="a_public_string";
    std::string a_public_string_by_constructor="a_public_string";
    AC(std::string apsbc): a_public_string_by_constructor(apsbc){}
    void printAllString(){
        std::cout<<"a_public_string: "<<a_public_string<<std::endl;
        std::cout<<"a_public_string_by_constructor: "<<a_public_string_by_constructor<<std::endl;
        std::cout<<"a_private_string: "<<a_private_string<<std::endl;
    }
    private:
    std::string a_private_string="a_private_string";
};
int main(void)  
{  
    AC ac("modified by constructor");
    ac.printAllString();
    // 输出: 
    // a_public_string: a_public_string
    // a_public_string_by_constructor: modified by constructor
    // rivate_string: a_private_string
} 
```

- 用C11之前的也会输出正确结果,但编译会有warning.
- 可以看出变量声名出赋值`std::string a_public_string="a_public_string";`会被构造器赋值覆盖掉.

#### ②.`common::GlobalData::GetTaskNameById(cr_id_)`

- 调用的是global_data.h&global_data.cc中的方法, 其中维护了一个task_id_map_(AtomicHashMap<uint64_t, std::string, 256>)(global_data.cc中的一个全局变量),简单理解是一个uint64_t-std::string的键值对, 通过键cr_id找值(std::string). 

#### ③.`common::GlobalData::GetChannelById(channel_id_)`

- 调用的是global_data.h&global_data.cc中的方法, 其中维护了一个channel_id_map_(AtomicHashMap<uint64_t, std::string, 256>)(global_data.cc中的一个全局变量),简单理解是一个uint64_t-std::string的键值对, 通过键channel_id找值(std::string). 

#### ④.`uint64_t channel_id_ = std::numeric_limits<uint64_t>::max()`

- 方法:`std::numeric_limits<uint64_t>::max()`在`<limit>`中,提供基本算数类型的极值信息, https://blog.csdn.net/fengbingchun/article/details/77922558

