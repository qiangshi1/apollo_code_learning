### data5. `data_visitor_base.h`

- 详细方法参考: 学习apollo之cyber-data5.html
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