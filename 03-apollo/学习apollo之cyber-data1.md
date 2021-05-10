### data1. `cache_buffer.h`

- 详细方法参考: 学习apollo之cyber-data1.html
- 维护了一个类: CacheBuffer, 它的写法与C++的容器很接近

#### ①.` CacheBuffer(const CacheBuffer& rhs)`

- `std::lock_guard<std::mutex> lg(*rhs*.mutex_);`这个方法之中为什么要加线程锁? 而且还是加的是源的锁? 


#### ②.`void Fill(const T& value)`

- 如果(!fusion_callback_), 该方法指的是向队列中填值, 从该方法可以看出, 这是一个用vector实现了的循环队列: 若不容量未满,它是:

|      0      |   1   |   2   |   3   |      4      |  5   |  6   | ...  |  n   |
| :---------: | :---: | :---: | :---: | :---------: | :--: | :--: | :--: | :--: |
| data0/head_ | data1 | data2 | data3 | data4/tail_ | NaN  | NaN  | NaN  | NaN  |

若容量已满,它是:

|    0    |    1    |      2      |      3      |   4   |   5   |   6   | ...  |    n    |
| :-----: | :-----: | :---------: | :---------: | :---: | :---: | :---: | :--: | :-----: |
| datan-2 | datan-1 | datan/tail_ | data0/head_ | data1 | data2 | data3 | ...  | datan-3 |

- 可以永远最多保存过去capacity_帧的过去的数据.

- 这个方法写的有点乱, Fill单纯填值就行, 为什么要用fusion_callback_截断它的功能?  每次分析到这儿的时候我都要紧盯着fusion_callback_有没有赋值. 