### data1. `cache_buffer.h`

- 详细方法参考: 学习apollo之cyber-data1.html
- 维护了一个类: CacheBuffer, 它的写法与C++的容器很接近

#### ①.` CacheBuffer(const CacheBuffer& rhs)`

- `std::lock_guard<std::mutex> lg(*rhs*.mutex_);`这个方法之中为什么要加线程锁? 而且还是加的是源的锁? 


#### ②.`void Fill(const T& value)`

- 从该方法可以看出, 这是一个用vector实现了的循环队列: 若不容量未满,它是:

|      0      |   1   |   2   |   3   |      4      |  5   |  6   | ...  |  n   |
| :---------: | :---: | :---: | :---: | :---------: | :--: | :--: | :--: | :--: |
| data0/head_ | data1 | data2 | data3 | data4/tail_ | NaN  | NaN  | NaN  | NaN  |

若容量已满,它是:

|    0    |    1    |      2      |      3      |   4   |   5   |   6   | ...  |    n    |
| :-----: | :-----: | :---------: | :---------: | :---: | :---: | :---: | :--: | :-----: |
| datan-2 | datan-1 | datan/tail_ | data0/head_ | data1 | data2 | data3 | ...  | datan-3 |

- 可以永远最多保存过去capacity_帧的过去的数据.

