### 24. `utility/class_factory.h & utility/class_factory.cc`

无界限队列。

#### a) `BUILD`的控制：

```cmake
cc_library(
    name = "class_loader",
    srcs = [
        "class_loader.cc",
        "utility/class_factory.cc",
        "utility/class_loader_utility.cc",
    ],
    hdrs = [
        "class_loader.h",
        "class_loader_register_macro.h",
        "utility/class_factory.h",
        "utility/class_loader_utility.h",
    ],
    deps = [
        "//cyber:init",
        "//cyber/common:log",
        "@poco//:PocoFoundation",
    ],
)
```

这个build控制多个文件，文件展开树：

![文件树](图片/文件树.png)

删除重复文件之后的展开树：

![剪枝文件树](图片/剪枝文件树.png)

这下知道该先看哪个文件了（`class_factory.h`&`class_factory.cc`->`class_loader_utility.h`&`class_loader_utility.cc`->`class_loader_register_macro.h`->class_loader.h`&`class_loader.cc`）。

这几个文件涉及到头文件相互包含，看了一篇博客https://blog.csdn.net/hazir/article/details/38600419，写的比较好，现复现一下他的工作：(好像看错了，它并没有头文件相互包含，只是有一些头文件重复包含了，不过这里学习下也可以的)

- C++是允许头文件相互包含的先看一个小例子：
  - 代码:

```c++
// header1.h
#ifndef _HEADER_1_H__
#define _HEADER_1_H__
#include"header2.h"
#endif
```

```c++
// header2.h
#ifndef _HEADER_2_H__
#define _HEADER_2_H__
#include"header1.h"
#endif
```

```c++
// source1.cc
#include"header1.h"
int main(int argc, char **argv){
    return 0;
}
```

- 编译：

```shell
g++ source1.cc
```

编译会成功的。

但若没有`#ifndef`，编译会报错：

  - 代码:

```c++
// header1.h
#include"header2.h"
```

```c++
// header2.h
#include"header1.h"
```

```c++
// source1.cc
#include"header1.h"
int main(int argc, char **argv){
    return 0;
}
```

- 编译：

```shell
g++ source1.cc
```

会报错：

```shell
In file included from header2.h:2:0,
                 from header1.h:2,
                 from header2.h:2,
                 from header1.h:2,
                 from header2.h:2,
                 from header1.h:2,
                 ......
                 from header2.h:2,
                 from header1.h:2,
                 from header2.h:2,
                 from header1.h:2,
                 from source1.cc:2:
header1.h:2:20: error: #include nested too deeply
```

所以还是要加上`#ifndef`防止递归无穷包含。

再看一个例子：

```c++
// header1.h
#ifndef _HEADER_1_H__
#define _HEADER_1_H__
#include"header2.h"
#define CONST_FOR_HEADER_2  1 //a macro used in header2.h
bool func(Header2Class* CA){ //defined in header2.h
    return true;
}
#endif
```

```c++
// header2.h
#ifndef _HEADER_2_H__
#define _HEADER_2_H__
#include"header1.h"
class Header2Class{ // used in header1.h
  int mVar;
  void setMem(){ mVar = CONST_FOR_HEADER_2; };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
};
#endif
```

```c++
// source1.cc
#include"header1.h"
int main(int argc, char **argv){
    return 0;
}
```

```c++
// source2.cc
#include"header2.h"
int main(int argc, char **argv){
    return 0;
}
```

编译：

```shell
g++ source1.cc -o app1 & g++ source2.cc -o app2
```

报错：

```shell
[1] 9207
In file included from header2.h:3:0,
                 from source2.cc:2:
header1.h:6:11: error: ‘Header2Class’ was not declared in this scope
 bool func(Header2Class* CA){ //defined in header2.h
           ^
header1.h:6:25: error: ‘CA’ was not declared in this scope
 bool func(Header2Class* CA){ //defined in header2.h
                         ^
header1.h:6:28: error: expected ‘,’ or ‘;’ before ‘{’ token
 bool func(Header2Class* CA){ //defined in header2.h
                            ^
In file included from header1.h:3:0,
                 from source1.cc:1:
header2.h: In member function ‘void Header2Class::setMem()’:
header2.h:7:25: error: ‘CONST_FOR_HEADER_2’ was not declared in this scope
   void setMem(){ mVar = CONST_FOR_HEADER_2 };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
                         ^
In file included from source2.cc:2:0:
header2.h: In member function ‘void Header2Class::setMem()’:
header2.h:7:44: error: expected ‘;’ before ‘}’ token
   void setMem(){ mVar = CONST_FOR_HEADER_2 };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
                                            ^
[1]+  Exit 1                  g++ source1.cc -o app1
```

先分析第一个问题：`error: ‘Header2Class’ was not declared in this scope`报错是`source2.cc`发出的，cc(cpp,c)文件找头文件的话是直接讲头文件包含进来：

```C++
// source2.cc 找头文件后会拼接成一个新的文件
// 拼接后展开来的新文件

// header1.h
#ifndef _HEADER_1_H__
#define _HEADER_1_H__
// #include"header2.h"
#define CONST_FOR_HEADER_2  1 //a macro used in header2.h
bool func(Header2Class* CA){ //defined in header2.h 		<---这里错误看得很清楚，CA在使用前并没有定义，而是在之后定义
    return true;
}
#endif

// header2.h
#ifndef _HEADER_2_H__
#define _HEADER_2_H__
// #include"header1.h"
class Header2Class{ // used in header1.h
  int mVar;
  void setMem(){ mVar = CONST_FOR_HEADER_2; };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
};
#endif

// source2.cc
#include"header2.h"
int main(int argc, char **argv){
    return 0;
}
```

所以要在header1.h之中声明类`Header2Class`。修改之后的文件为：

```c++
// header1.h
#ifndef _HEADER_1_H__
#define _HEADER_1_H__
#include"header2.h"
#define CONST_FOR_HEADER_2  1 //a macro used in header2.h
class Header2Class;		// <-------------这里新增一行
bool func(Header2Class* CA){ //defined in header2.h
    return true;
}
#endif
```

```c++
// header2.h
#ifndef _HEADER_2_H__
#define _HEADER_2_H__
#include"header1.h"
class Header2Class{ // used in header1.h
  int mVar;
  void setMem(){ mVar = CONST_FOR_HEADER_2; };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
};
#endif
```

```c++
// source1.cc
#include"header1.h"
int main(int argc, char **argv){
    return 0;
}
```

```c++
// source2.cc
#include"header2.h"
int main(int argc, char **argv){
    return 0;
}
```

编译:

```shell
g++ source1.cc -o app1 & g++ source2.cc -o app2
```

过程不报错了，但会另一个问题：

```c++
[1] 10617
In file included from header1.h:3:0,
                 from source1.cc:1:
header2.h: In member function ‘void Header2Class::setMem()’:
header2.h:7:25: error: ‘CONST_FOR_HEADER_2’ was not declared in this scope
   void setMem(){ mVar = CONST_FOR_HEADER_2 };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
                         ^
In file included from source2.cc:2:0:
header2.h: In member function ‘void Header2Class::setMem()’:
header2.h:7:44: error: expected ‘;’ before ‘}’ token
   void setMem(){ mVar = CONST_FOR_HEADER_2 };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
                                            ^
[1]+  Exit 1                  g++ source1.cc -o app1
```

分析第二个问题：`error: ‘CONST_FOR_HEADER_2’ was not declared in this scope`报错是`source1.cc`发出的，编译过程中`source1.cc`会展开成：

```c++
// header2.h
#ifndef _HEADER_2_H__
#define _HEADER_2_H__
// #include"header1.h"

class Header2Class{ // used in header1.h
  int mVar;
  void setMem(){ mVar = CONST_FOR_HEADER_2 };   //macro CONST_FOR_HEADER_2 is defined in head1er.h <---错误也很清楚，CONST_FOR_HEADER_2在使用前并没有定义，而是在之后定义
};
#endif

// header1.h
#ifndef _HEADER_1_H__
#define _HEADER_1_H__
// #include"header2.h"
#define CONST_FOR_HEADER_2  1 //a macro used in header2.h
class Header2Class; 
bool func(Header2Class* CA){ //defined in header2.h
    return true;
}
#endif

// source1.cc
#include"header1.h"
int main(int argc, char **argv){
    return 0;
}
```

所以会报错，改动方法是更改宏定义`#define CONST_FOR_HEADER_2  1`的位置，改动之后代码为：

```c++
// header1.h
#ifndef _HEADER_1_H__
#define _HEADER_1_H__
#include"header2.h"
// #define CONST_FOR_HEADER_2  1 //a macro used in header2.h <--------以前在这里
class Header2Class;
bool func(Header2Class* CA){ //defined in header2.h
    return true;
}
#endif
```

```c++
// header2.h
#ifndef _HEADER_2_H__
#define _HEADER_2_H__
#include"header1.h"

// #define CONST_FOR_HEADER_2  1  <--------移动到这儿
class Header2Class{ // used in header1.h
  int mVar;
  void setMem(){ mVar = CONST_FOR_HEADER_2; };   //macro CONST_FOR_HEADER_2 is defined in head1er.h
};
#endif
```

```c++
// source1.cc
#include"header1.h"
int main(int argc, char **argv){
    return 0;
}
```

```c++
// source2.cc
#include"header2.h"
int main(int argc, char **argv){
    return 0;
}
```

可以通过编译！

#### b) 类声明`ClassLoader`：

-  仅仅声明了一个类。

#### c) 类`AbstractClassFactoryBase`

- 很简单的一个类

#### c1) 类成员`AbstractClassFactoryBase::relative_class_loaders_`

- `std::vector<ClassLoader*>`

#### c2) 类成员`AbstractClassFactoryBase::relative_library_path_`

- `std::string`

#### c3) 类成员`AbstractClassFactoryBase::base_class_name_`

- `std::string`

#### c4) 类成员`AbstractClassFactoryBase::class_name_`

- `std::string`

#### d) 类构造函数:`AbstractClassFactoryBase::AbstractClassFactoryBase()`

- 赋值`relative_library_path_` `base_class_name_` `class_name_` `relative_library_path_`赋为空值

#### e) 类析构函数:`AbstractClassFactoryBase::~AbstractClassFactoryBase()`

- 虚函数

#### f) 类函数:`AbstractClassFactoryBase::SetRelativeLibraryPath()`

- 设置相关链接库路径，给`relative_library_path_`赋值

#### g) 类函数:`AbstractClassFactoryBase::AddOwnedClassLoader()`

- 向`relative_class_loaders_`中添加新值，`relative_class_loaders_`是一个类加载器指针(ClassLoader*)的`vector`，`relative_class_loaders_`是一个类成员.

#### h) 类函数:`AbstractClassFactoryBase::RemoveOwnedClassLoader()`

- 在`relative_class_loaders_`删除一个元素，但`relative_class_loaders_`是一个vector，它的删除元素操作是否效率低？或者该方法不会频繁使用？

#### i) 类函数:`AbstractClassFactoryBase::IsOwnedBy()`

- `relative_class_loaders_`是否含有`loader`。

#### j) 类函数:`AbstractClassFactoryBase::IsOwnedByAnybody()`

- 判断`relative_class_loaders_`是否为空。

#### k) 类函数:`AbstractClassFactoryBase::GetRelativeClassLoaders()`

- 返回`relative_class_loaders_`。

#### l) 类函数:`AbstractClassFactoryBase::GetRelativeLibraryPath()`

- 返回`relative_library_path_`。

#### m) 类函数:`AbstractClassFactoryBase::GetBaseClassName()`

- 返回`base_class_name_`。

#### n) 类函数:`AbstractClassFactoryBase::GetClassName()`

- 返回`class_name_`。

#### o) 类:`AbstractClassFactory`

- 继承自`AbstractClassFactoryBase`

#### p) 类构造函数:`AbstractClassFactory::AbstractClassFactory()`

- 集成父类的构造函数。

#### q) 类函数:`AbstractClassFactory::CreateObj()`

- 虚函数。
- `const = 0` `const`和 `=0`，要分开理解，`=0`表示这个成员函数是纯虚函数，也就是它可以没有定义，只有接口，由它的继承类具体定义它的行为，当然，你也可以给它定义缺省的函数体https://www.cnblogs.com/Stephen-Qin/p/10128922.html
- 所以该类只能被继承，而不能创建它的对象。使能创建它的派生类。这里可以自己写一个例子感受下。

#### r) 类函数:`AbstractClassFactory::AbstractClassFactory()`

- 为什么不是public？

#### s) 类函数:`AbstractClassFactory::AbstractClassFactory()`

- 为什么不是public？

- 允许拷贝构造

#### t) 类函数:`AbstractClassFactory::AbstractClassFactory()`

- 为什么不是public？
- 允许拷贝构造

#### u) 类:`ClassFactory`

- 继承自``AbstractClassFactory`

#### v)类构造函数：`ClassFactory::ClassFactory`()

#### w)类函数：`ClassFactory::CreateObj()`

- 

### 25. `class_loader_utility.h`&`class_loader_utility.cc`

- 它的build在24)中已经介绍。

#### a) 类声明`ClassLoader`：

-  仅仅声明了一个类。

#### b) `using`：

-  

#### c) GetClassFactoryMapMap()

- 文件中大量使用了static+return+&(引用)结构：

- ```
  BaseToClassFactoryMapMap& GetClassFactoryMapMap() {
    static BaseToClassFactoryMapMap instance;
    return instance;
  }
  ```

- 它是否相当于：

- ```
  BaseToClassFactoryMapMap instance;
  ```

- ?

