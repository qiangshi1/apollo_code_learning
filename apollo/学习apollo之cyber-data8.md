### data7. `data_visitor_test.h`


阅读下`data_visitor_test.cc`代码:

```c++
#include "cyber/data/data_visitor.h"
#include <memory>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "cyber/common/log.h"
#include "cyber/cyber.h"
#include "cyber/message/raw_message.h"

namespace apollo {
namespace cyber {
namespace data {

using apollo::cyber::message::RawMessage;
using apollo::cyber::proto::RoleAttributes;
std::hash<std::string> str_hash;

auto channel0 = str_hash("/channel0");
// 类型是uint64_t
auto channel1 = str_hash("/channel1");
// 类型是uint64_t
auto channel2 = str_hash("/channel2");
// 类型是uint64_t
auto channel3 = str_hash("/channel3");
// 类型是uint64_t
void DispatchMessage(uint64_t channel_id, int num) {
  for (int i = 0; i < num; ++i) {
    auto raw_msg = std::make_shared<RawMessage>();
    // 创建一个智能指针
    DataDispatcher<RawMessage>::Instance()->Dispatch(channel_id, raw_msg);
    // 单例模式, 键值查找DataDispatcher<RawMessage>::Instance()->buffers_map_ 如果查找到对应的值(类型, 可以视作vector<CacheBuffer<T>>), 对于vector<CacheBuffer<T>>的每个元素: CacheBuffer<T>, 执行Fill(msg), Fill的功能再说一次, 如果fusion_callback_有值, 执行fusion_callback_; 如果fusion_callback_没值, 向CacheBuffer<T>的buffer_填值. 再执行notifier_->Notify(channel_id), 是执行键值查找notifier_->notifies_map_,如果找到, 执行值的每个元素(值是一个vector)的callback. 
  }
}

std::vector<VisitorConfig> InitConfigs(int num) {
  std::vector<VisitorConfig> configs;
  configs.reserve(num);
  for (int i = 0; i < num; ++i) {
    uint64_t channel_id = str_hash("/channel" + std::to_string(i));
    uint32_t queue_size = 10;
    configs.emplace_back(channel_id, queue_size);
  }
  return configs;
}
// 一个VisitorConfig vector

TEST(DataVisitorTest, one_channel) {
  // 测试开始
  auto channel0 = str_hash("/channel");
  // channel0类型是uint64_t
  auto dv = std::make_shared<DataVisitor<RawMessage>>(channel0, 10);
  // dv是一个智能指针, 上述构造函数还操作了单例: DataDispatcher<RawMessage>::Instance()
  /* 
  记b1(CacheBuffer)={
      T=shared_ptr<RawMessage>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=10(capacity_)
  }
  dv->{
    T=RawMessage, 
    buffer_(它是一个ChannelBuffer)={
      T=RawMessage,
      channel_id_=channel0, 
      buffer_(它是一个CacheBuffer)->b1
    },
    notifier_(智能指针, 在data_visitor_base.h中, 被派生而来, 在基类的构造函数中被创建)->{
      callback=nullptr
    },
    data_notifier_(单例, 在data_visitor_base.h中, 被派生而来)->{
      notifies_map_={
        [channel0->{notifier_,}](键值对),
      }
    },
    next_msg_index_(在data_visitor_base.h中, 被派生而来)=0,
  } 
  DataDispatcher<RawMessage>::Instance()->{
    buffers_map_={
      [channel0->{{->b1(一个智能指针指向b1)},}](键值对),
    }
  }
  */
  DispatchMessage(channel0, 1);
  // 更新了DataDispatcher<RawMessage>::Instance(), 其实也就是b1填了一个值
  /* 
  b1={
      T=shared_ptr<RawMessage>, head_=0, tail_=1, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=10(capacity_, buffer_={0, raw_msg, 0, ...})(raw_msg是一个RawMessage的智能指针)
  }
  DataDispatcher<RawMessage>::Instance()->{
    buffers_map_={
      [channel0->{{->b1(一个智能指针指向b1)},}](键值对),
    }
  }
  */
  std::shared_ptr<RawMessage> msg;
  // 创建一个智能指针
  EXPECT_TRUE(dv->TryFetch(msg));
  /*  dv->buffer_.buffer_->不为空, 所以是true, 执行后:
  dv->{
    T=RawMessage, 
    buffer_(它是一个ChannelBuffer)={
      T=RawMessage,
      channel_id_=channel0, 
      buffer_(它是一个CacheBuffer)->b1
    }
    data_notifier_(单例, 在data_visitor_base.h中, 被派生而来)->{
      notifies_map_={
        [channel0->{{callback=nullpr},}](键值对),
      }
    }
    next_msg_index_=2
  } 
  msg=raw_msg
  */
  EXPECT_FALSE(dv->TryFetch(msg));
  /*  虽然dv->buffer_.buffer_->不为空, 但next_msg_index_越界了(next_msg_index_==dv->buffer_.buffer_.tail_+1==3), 在Fetch()中判断为false, 所以返回false
    不改变任何对象的值
  */
  DispatchMessage(channel0, 10);
  // 更新了DataDispatcher<RawMessage>::Instance(), 其实也就是b1填值
  /* 
  b1={
      T=shared_ptr<RawMessage>, head_=1, tail_=11, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=10(capacity_, buffer_={raw_msg, raw_msg, raw_msg1, ...})(raw_msg是一个RawMessage的智能指针)
  }
  DataDispatcher<RawMessage>::Instance()->{
    buffers_map_={
      [channel0->{{->b1(一个智能指针指向b1)},}](键值对),
    }
  }
  */
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(dv->TryFetch(msg));
    // 关注next_msg_index_, 它如果越界, 就返回false, 因为上边b1又重新填了10个值, 所以这里可以取10个值
  }
  EXPECT_FALSE(dv->TryFetch(msg));
  // 同样地, 第12次取失败了
}
// 下边的分析和上边的极其类似
TEST(DataVisitorTest, two_channel) {
  auto dv =
      std::make_shared<DataVisitor<RawMessage, RawMessage>>(InitConfigs(2));

  std::shared_ptr<RawMessage> msg0;
  std::shared_ptr<RawMessage> msg1;
  DispatchMessage(channel0, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1));
  DispatchMessage(channel1, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1));
  DispatchMessage(channel0, 1);
  EXPECT_TRUE(dv->TryFetch(msg0, msg1));
  DispatchMessage(channel0, 10);
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(dv->TryFetch(msg0, msg1));
  }
  EXPECT_FALSE(dv->TryFetch(msg0, msg1));
}

TEST(DataVisitorTest, three_channel) {
  auto dv = std::make_shared<DataVisitor<RawMessage, RawMessage, RawMessage>>(
      InitConfigs(3));

  std::shared_ptr<RawMessage> msg0;
  std::shared_ptr<RawMessage> msg1;
  std::shared_ptr<RawMessage> msg2;
  DispatchMessage(channel0, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2));
  DispatchMessage(channel1, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2));
  DispatchMessage(channel2, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2));
  DispatchMessage(channel0, 1);
  EXPECT_TRUE(dv->TryFetch(msg0, msg1, msg2));
  DispatchMessage(channel0, 10);
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(dv->TryFetch(msg0, msg1, msg2));
  }
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2));
}

TEST(DataVisitorTest, four_channel) {
  auto dv = std::make_shared<
      DataVisitor<RawMessage, RawMessage, RawMessage, RawMessage>>(
      InitConfigs(4));

  std::shared_ptr<RawMessage> msg0;
  std::shared_ptr<RawMessage> msg1;
  std::shared_ptr<RawMessage> msg2;
  std::shared_ptr<RawMessage> msg3;
  DispatchMessage(channel0, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2, msg3));
  DispatchMessage(channel1, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2, msg3));
  DispatchMessage(channel2, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2, msg3));
  DispatchMessage(channel3, 1);
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2, msg3));
  DispatchMessage(channel0, 1);
  EXPECT_TRUE(dv->TryFetch(msg0, msg1, msg2, msg3));
  DispatchMessage(channel0, 10);
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(dv->TryFetch(msg0, msg1, msg2, msg3));
  }
  EXPECT_FALSE(dv->TryFetch(msg0, msg1, msg2, msg3));
}

}  // namespace data
}  // namespace cyber
}  // namespace apollo
```

