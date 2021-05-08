### data3. `channel_buffer.h`

- 详细方法参考: 学习apollo之cyber-data3.html
- 维护了一个类: ChannelBuffer, 

#### ①.`bool Fetch(uint64_t* index, std::shared_ptr<T>& m);`

- Fetch实现的功能: buffer_是一个队列, 假设队列长度是10的话, 可以是buffer_.head_=0, buffer_tail_=5(未满); 可以是buffer_.head_=0, buffer_tail_=9(满了), 可以是buffer_.head_=10, buffer_tail_=19(不仅满了, 而且还新值替换旧值了),
- 对分析*index的情形: 其实还是可以理解啦, *index == 0返回队尾; *index == buffer_->Tail() + 1超出队列当然返回false; *index < buffer_->Head()表示太陈旧的值被替换了, 所以会报一个提醒, 同时返回队尾; 当然剩余的情况就是 *index在队列中, 所以正常返回它代表的值. 
- 这个方法显得乱的原因是它的`*index`含义不清楚, 到底是偏移值还是绝对值; 某些情况转化了`*index`
- `else if (*index == buffer_->Tail() + 1)`处是否应该是`(*index >= buffer_->Tail() + 1)`?



