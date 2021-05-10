### data3. `data_dispatcher.h`

- 详细方法参考: 学习apollo之cyber-data4.html
- 维护了一个类: DataDispatcher, 

#### ①.`using BufferVector = std::vector<std::weak_ptr<CacheBuffer<std::shared_ptr<T>>>>`
 - 要说的是`std::weak_ptr`, 它在`bool DataDispatcher<T>::Dispatch(const uint64_t channel_id, const std::shared_ptr<T>& msg)`也出现了`lock()`方法. 
 - `std::weak_ptr`是智能指针的一种
 - 多线程智能指针必须加锁: https://www.cnblogs.com/wang1994/p/10765974.html
 - 单纯用`std::shared_ptr`可能造成内存释放失败:
 ```c++
 #include <iostream>
#include <memory> // for std::shared_ptr
#include <string>
class Person{
public:
	std::string m_name;
	std::shared_ptr<Person> m_partner; 
	Person(const std::string &name): m_name(name){ 
		std::cout << m_name << " created"<<std::endl;
	}
	~Person(){
		std::cout << m_name << " destroyed"<<std::endl;
	}
};
int main(){
	auto lucy = std::make_shared<Person>("Lucy"); // create a Person named "Lucy"
	auto ricky = std::make_shared<Person>("Ricky"); // create a Person named "Ricky"
    lucy->m_partner = ricky;
    ricky->m_partner = lucy;
    std::cout << lucy.use_count() << std::endl;
    std::cout << ricky.use_count() << std::endl;
	return 0;
}
 ```
 用`std::weak_ptr`可能修复这种BUG. 
 ```c++
  #include <iostream>
#include <memory> // for std::shared_ptr
#include <string>
class Person{
public:
	std::string m_name;
	std::weak_ptr<Person> m_partner;  //这里修改
	Person(const std::string &name): m_name(name){ 
		std::cout << m_name << " created"<<std::endl;
	}
	~Person(){
		std::cout << m_name << " destroyed"<<std::endl;
	}
};
int main(){
	auto lucy = std::make_shared<Person>("Lucy"); // create a Person named "Lucy"
	auto ricky = std::make_shared<Person>("Ricky"); // create a Person named "Ricky"
    lucy->m_partner = ricky;
    ricky->m_partner = lucy;
    std::cout << lucy.use_count() << std::endl;
    std::cout << ricky.use_count() << std::endl;
	return 0;
}
 ```
 - 另,智能指针怎样判断先析构那个对象呢? 注意到下两个例子的输出是不一样的. 程序如何判断这种不同呢? 
 ```c++
 #include <iostream>
#include <memory> // for std::shared_ptr
#include <string>
class Person{
public:
	std::string m_name;
	std::shared_ptr<Person> m_partner; 
	Person(const std::string &name): m_name(name){ 
		std::cout << m_name << " created"<<std::endl;
	}
	~Person(){
		std::cout << m_name << " destroyed"<<std::endl;
	}
};
int main(){
	auto lucy = std::make_shared<Person>("Lucy"); // create a Person named "Lucy"
	auto ricky = std::make_shared<Person>("Ricky"); // create a Person named "Ricky"
    lucy->m_partner = ricky;
    // ricky->m_partner = lucy;
	return 0;
}
 ```

 ```c++
 #include <iostream>
#include <memory> // for std::shared_ptr
#include <string>
class Person{
public:
	std::string m_name;
	std::shared_ptr<Person> m_partner; 
	Person(const std::string &name): m_name(name){ 
		std::cout << m_name << " created"<<std::endl;
	}
	~Person(){
		std::cout << m_name << " destroyed"<<std::endl;
	}
};
int main(){
	auto lucy = std::make_shared<Person>("Lucy"); // create a Person named "Lucy"
	auto ricky = std::make_shared<Person>("Ricky"); // create a Person named "Ricky"
    // lucy->m_partner = ricky;
    ricky->m_partner = lucy;
	return 0;
}
 ```
 - ?

来分析下`data_dispatcher_test.cc`
```c++
#include "cyber/data/data_dispatcher.h"
#include <memory>
#include <vector>
#include "gtest/gtest.h"
#include "cyber/common/util.h"

namespace apollo {
namespace cyber {
namespace data {

template <typename T>
using BufferVector = std::vector<std::weak_ptr<CacheBuffer<std::shared_ptr<T>>>>;
// 它是DataDispatcher<T>的buffers_map_(键值对)的值的类型 不知道为什么会在这里
auto channel0 = common::Hash("/channel0");
// std::size_t(unsigned long)
auto channel1 = common::Hash("/channel1");
// std::size_t(unsigned long)
TEST(DataDispatcher, AddBuffer) {
  auto cache_buffer1 = new CacheBuffer<std::shared_ptr<int>>(2);
  /*
  cache_buffer1->{
	T=shared_ptr<int>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=3(2+1), buffer_.size=3(capacity_)
  }
  */
  auto buffer0 = ChannelBuffer<int>(channel0, cache_buffer1);
  /*
  buffer0={
	T=int, channel_id_=channel0,buffer_(BufferType(std::shared_ptr<int>)的智能指针)=cache_buffer1
  }
  */
  auto cache_buffer2 = new CacheBuffer<std::shared_ptr<int>>(2);
  /*
  cache_buffer2->{
	T=shared_ptr<int>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=3(2+1), buffer_.size=3(capacity_)
  }
  */
  auto buffer1 = ChannelBuffer<int>(channel1, cache_buffer2);
  /*
  buffer0={
	T=int, channel_id_=channel1,buffer_(BufferType(std::shared_ptr<int>)的智能指针)=cache_buffer2
  }
  */
  auto dispatcher = DataDispatcher<int>::Instance();
  /* dispatcher是一个单例
	dispatcher->{
	  notifier_(也是一个单例)->{
		  notifies_map_=空
	  }
	  buffers_map_=空
	}
  */
  dispatcher->AddBuffer(buffer0);
  /* dispatcher是一个单例
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={channel0->{cache_buffer1} }
	}
  */
  dispatcher->AddBuffer(buffer1);
  /* dispatcher是一个单例
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={
		channel0->{cache_buffer1}, 
		channel1->{cache_buffer2}
	  }
	}
  */
}

TEST(DataDispatcher, Dispatch) {
  auto cache_buffer = new CacheBuffer<std::shared_ptr<int>>(10);
  /*
  cache_buffer->{
	T=shared_ptr<int>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=11(capacity_)
  }
  */
  auto buffer = ChannelBuffer<int>(channel0, cache_buffer);
  /*
  buffer={
	T=int, channel_id_=channel0,buffer_(BufferType(std::shared_ptr<int>)的智能指针)=cache_buffer
  }
  */
  auto dispatcher = DataDispatcher<int>::Instance();
  /* dispatcher是一个单例, gtest中这种单例模式 不同的TEST()会有干涉吗, 如果把它视作一个全局变量的话会有干涉的, 这里我先假设有干涉
  /* dispatcher是一个单例
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={
		channel0->{cache_buffer1}, 
		channel1->{cache_buffer2}
	  }
	}
  */
  auto msg = std::make_shared<int>(1);
  // 智能指针
  EXPECT_FALSE(dispatcher->Dispatch(channel0, msg));
  /* dispatcher是一个单例, 它有更新, 更新了cache_buffer1, 但dispatcher->notifier_为空, 所以返回false
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={
		channel0->{cache_buffer1}, 
		channel1->{cache_buffer2}
	  }
	}
  */
  /*
  cache_buffer1->{
	T=shared_ptr<int>, head_=0, tail_=1, fusion_callback_=nullptr, capacity_=3(2+1), buffer_.size=3, buffer_={0, msg, 0, ...})
  }
  */
  dispatcher->AddBuffer(buffer);
  /* dispatcher是一个单例, 它有更新,
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={
		channel0->{cache_buffer1, buffer}, 
		channel1->{cache_buffer2}
	  }
	}
  */
  EXPECT_FALSE(dispatcher->Dispatch(channel0, msg));
    /* dispatcher是一个单例, 它有更新, 更新了buffer
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={
		channel0->{cache_buffer1, cache_buffer}, 
		channel1->{cache_buffer2}
	  }
	}
	cache_buffer->{
	  T=shared_ptr<int>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=11(capacity_), buffer_={0, msg, 0, ...}
	}
  */
  auto notifier = std::make_shared<Notifier>();
  // 智能指针
  DataNotifier::Instance()->AddNotifier(channel0, notifier);
  /* DataNotifier::Instance()(和dispatcher->notifier_绑定)->{
	notifies_map_={channel0->{notifier}}
  }
  */
  EXPECT_TRUE(dispatcher->Dispatch(channel0, msg));
  /* 更新dispatcher, 更新了cache_buffer1和cache_buffer
  dispatcher是一个单例, 它有更新, 更新了buffer
	dispatcher->{
	  notifier_(也是一个单例)->{
		notifies_map_=空
	  }
	  buffers_map_={
		channel0->{cache_buffer1, cache_buffer}, 
		channel1->{cache_buffer2}
	  }
	}
	cache_buffer1->{
	  T=shared_ptr<int>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=3(2+1), buffer_.size=3(capacity_), buffer_={0, msg, msg, ...}
	}
	cache_buffer->{
	  T=shared_ptr<int>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=11(capacity_), buffer_={0, msg, msg, ...}
	}
	但dispatcher->notifier_[channel0]有值, 所以返回true
  */
}

}  // namespace data
}  // namespace cyber
}  // namespace apollo
```