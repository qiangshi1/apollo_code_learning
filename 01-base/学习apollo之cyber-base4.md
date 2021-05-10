### 19. `signal.h`

`signal.h`之中维护了三个类:`Slot`/`Connection`/`Signal`,这三个类耦合十分紧密.

#### a) `BUILD`的控制：

```
cc_library(
    name = "signal",
    hdrs = ["signal.h"],
)
```
常规链接库.


#### b) 类`Signal`

- 四个`using`,常规重命名.
- `std::list`是c++标准容器.
- 把类`Slot`/`Connection`声明在了`Signal`前面,为了能在`Signal`中使用`Slot`/`Connection`.

#### b1) 类成员`Signal::slots_`
`slot`智能指针的`std::list`.

#### b2) 类成员`Signal::mutex_`
线程锁.

#### c) 类构造函数`Signal::Signal()`
常规.

#### d) 类析构函数`Signal::~Signal()`
调用`DisconnectAllSlots()`线程安全地清空所有`slots_`(`slots_`是`slot`智能指针的`std::list`(C++标准容器)),`slot`是`Slot`的实例,`slot`维护了一个`std::function`和一个`connected`(布尔值).

#### e) 类运算符`Signal::()`
把`slots_`的`slot`压如`local`,如果`local`不为空,执行每个`slot`中的`function`,线程安全地清除`slots`中所有`connected_==false`的`slot(ClearDisconnectedSlots())`.因为要这样动态地添加删除`slots`,所以用`std::list`而不是`std::vector`?


#### f) 类函数`Signal::Connect()`
线程安全地输入一个`std::function`,用`std::function`初始化一个`slot`,`slot`维护了一个`std::function`和一个`connected`(布尔值).`slot`压入`slots_`,返回一个`Connection`,`Connection`维护了一个`Slot`和`Signal`指针.

#### g) 类函数`Signal::Disconnect()`
输入一个`connection`,线程安全地:如果`this(Signal)`的`slots_`有`connection`中的`slot`,该`slot`的`connected`置为`false`.清除`slots_`中所有`connected`置为`false`的`slo`t.

#### h) 类函数`Signal::DisconnectAllSlots()`
线程安全地把`slots`_中的所有`slot`的`connected`置为`false`,清空`slot_`.

#### i) `delete`
```
Signal(const Signal&) = delete;
Signal& operator=(const Signal&) = delete;
```
取消拷贝赋值.

#### j) 类函数`Signal::ClearDisconnectedSlots()`
线程安全地清除`slots_`中所有`connected==false`的`slot`,

#### k) 类`Connection`
- 维护了一个`Slot`智能指针和一个`Signal`智能指针.
- 两个`using`,常规重命名

#### k1)类成员`Connection::slot_`

#### k2)类成员`Connection::signal_

#### l) 类构造函数`Connection::Connection()`

常规.

#### m) 类构造函数`Connection::Connection()`

l)的重载.

#### n) 类析构函数`Connection::~Connection()`

常规.

#### o) 类运算符`Connection::=`

判断相等.

#### p) 类函数`Connection::HasSlot()`

输入一个`slot`,判断我自有的`slot_`和输入的`slot`是否相等.

#### q) 类函数`Connection::IsConnected()`

#### r) 类函数`Connection::Disconnect()`

#### s) 类`Slot`

- 维护了一个`std::function`和一个`connected`(布尔值),可以看做一个加强版的`std::function`.
- 一个`using`,常规重命名

#### s1) 类成员`Slot::cb_`

一个`std::function`

#### s2) 类成员`Slot::connected_`

一个布尔值

#### t) 类构造函数`Slot::Slot`()

常规.

#### u) 类构造函数`Slot::Slot()`

t)的重载.

#### v) 类析构函数`Slot::~Slot()`

常规.

#### w) 类析构函数`Slot::~Disconnect()`

常规.

#### x) 类析构函数`Slot::~connected()`

常规.

### 20. signal_test.cc`

它是一个google test,阅读test十分有助于理解被测试代码.

#### a) `BUILD`的控制：

```
cc_test(
    name = "signal_test",
    size = "small",
    srcs = ["signal_test.cc"],
    deps = [
        "//cyber/base:signal",
        "@com_google_googletest//:gtest_main",
    ],
)
```

#### b) 由test分析`signal.h`

- `Signal`看起来和`std::function`极其类似,
- 和std::`function`不同的是`Signal`可以有多个std::`function`,如`sig(lhs, rhs);`可以执行当前所有的`std::function`