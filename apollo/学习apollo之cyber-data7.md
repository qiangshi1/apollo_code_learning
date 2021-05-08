### data7. `all_latest.h`

- 三个类,  也是重载类? 参考 data6. data_fusion.h

- 如果理解代码含义的话, 只看第一个类, 应该就可以了. 

  ```C++
  template <typename M0, typename M1 = NullType, typename M2 = NullType,
            typename M3 = NullType>
  class AllLatest : public DataFusion<M0, M1, M2, M3>
  ```

#### ①. `std::tuple`
 - c++元组. c++11中引入的新的类型，可类比std::pair。 但是std::pair只能支持两个元素。 理论上， 元组支持0~任意个元素。如下: 

   ```C++
   #include <tuple>
   #include <iostream>
   int main() {
     std::tuple<char, int, long, std::string> a_tuple('A', 2, 3, "4");
     int index = 0;
     std::cout << index++ << " = " << std::get<0>(a_tuple) << "\n";
     std::cout << index++ << " = " << std::get<1>(a_tuple) << "\n";
     std::cout << index++ << " = " << std::get<2>(a_tuple) << "\n";
     std::cout << index++ << " = " << std::get<3>(a_tuple).c_str() << "\n";
     return 0;
   }
   ```

   这里有一个很有意思的知识点, 就是数值型模板, 之前用的比较多的是类型型模板. 一个用数值类型模板的例子: 

   ```c++
   #include <iostream>
   class AC{
     public:
     int a_int_array[7]={0,1,2,3,4,5,6};
     template <unsigned int N>
     void accessElementByIndex(){
       std::cout<<N<<": "<<a_int_array[N]<<std::endl;
     }
   };
   int main() {
     AC a_c;
     a_c.accessElementByIndex<1>();
     a_c.accessElementByIndex<5>();
     a_c.accessElementByIndex<6>();
   }
   ```

阅读下`all_latest_test.cc`代码:

```c++
#include <memory>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "cyber/common/log.h"
#include "cyber/cyber.h"
#include "cyber/data/data_visitor.h"
#include "cyber/data/fusion/all_latest.h"

namespace apollo {
namespace cyber {
namespace data {

using apollo::cyber::message::RawMessage;
using apollo::cyber::proto::RoleAttributes;

// using FusionDataType = tuple<shared_ptr<RawMessage>, shared_ptr<RawMessage>>
std::hash<std::string> str_hash;
// hash返回的是一个size_t(unsigned int) 对任何对象操作, 返回一个无符号整型. 

TEST(AllLatestTest, two_channels) {
  auto cache0 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  // cache0->{T=shared_ptr<RawMessage>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=10(capacity_)}
  auto cache1 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  // cache1->{T=shared_ptr<RawMessage>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11(10+1), buffer_.size=10(capacity_)}
  ChannelBuffer<RawMessage> buffer0(static_cast<uint64_t>(0), cache0);
  // buffer0={T=RawMessage, channel_id_=0, buffer_(和cache0绑定)}
  ChannelBuffer<RawMessage> buffer1(static_cast<uint64_t>(1), cache1);
  // buffer1={T=RawMessage, channel_id_=0, buffer_(和cache1绑定)}
  std::shared_ptr<RawMessage> m;
  // m=nullptr
  std::shared_ptr<RawMessage> m0;
  // m0=nullptr
  std::shared_ptr<RawMessage> m1;
  // m1=nullptr
  uint64_t index = 0;
  // index=0
  fusion::AllLatest<RawMessage, RawMessage> fusion(buffer0, buffer1);
  /* fusion={
      M0=RawMessage, M1=RawMessage, buffer_m0_=buffer0, buffer_m1_=buffer1, 
      buffer_fusion_={
        T=FusionDataType, channel_id_=0, 
        buffer_->{
          T=shared_ptr<FusionDataType>, head_=0, tail_=0, fusion_callback_=nullptr, capacity_=11, buffer_.size=10
        }
      }, 
      buffer_m0_.buffer(和cache0绑定)->fusion_callback_={
        函数体, 输入一个const shared_ptr<RawMessage>& m0, 如果buffer_m1_.buffer_(和cache1绑定)为空, 返回void; 如果buffer_m1_.buffer_(和cache1绑定)不为空, 获取buffer_m1_.buffer_(和cache1绑定)最后一个元素m1, 执行buffer_fusion_.Buffer()->Fill(make_shared<FusionDataType>(m0, m1))
      }
    }
  */
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1));
  // 返回false, 因为fusion.buffer_fusion_.buffer_->Empty()=0, 不更改任何对象的值
  cache0->Fill(std::make_shared<RawMessage>("0-0"));
  // 因为cache0->fusion_callback_不为nullptr, 所以执行cache0->fusion_callback_(make_shared<RawMessage>("0-0")). 对于cache0->fusion_callback_(make_shared<RawMessage>("0-0")), 此时buffer_m1_.buffer_(和cache1绑定)为空, 所以返回void. 所以该步其实没有更改任何对象的值
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1));
  // 返回false, 因为fusion.buffer_fusion_.buffer_->Empty()=0, 不更改任何对象的值
  cache1->Fill(std::make_shared<RawMessage>("1-0"));
  // 因为cache1->的fusion_callback_=nullptr, 所以该步是在cache1->buffer_处填值, 操作后: cache1->{T=shared_ptr<RawMessage>, head_=0, tail_=1, fusion_callback_=nullptr, capacity_=11, buffer_.size=10, buffer_={0, make_shared<RawMessage>("1-0"), 0, ...}}
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1));
  // 返回false, 因为fusion.buffer_fusion_.buffer_->Empty()=0, 不更改任何对象的值
  cache0->Fill(std::make_shared<RawMessage>("0-1"));
  // 因为cache0->fusion_callback_不为nullptr, 所以执行cache0->fusion_callback_(make_shared<RawMessage>("0-1")). 对于cache0->fusion_callback_(make_shared<RawMessage>("0-1")), 此时buffer_m1_.buffer_(和cache1绑定)不为空, 执行buffer_fusion_.buffer_(和cache1绑定)->Fill(make_shared<FusionDataType>(m0, m1)), m0是make_shared<RawMessage>("0-1"), m1是buffer_m1_.buffer_(和cache1绑定)最后一个元素, 对于buffer_fusion_.buffer_(和cache1绑定)->Fill(make_shared<FusionDataType>(m0, m1)), buffer_fusion_.buffer_->fusion_callback_=nullptr, 所以给buffer_fusion_.buffer_填值, 操作后:  
  /* fusion={
    M0=RawMessage, M1=RawMessage, buffer_m0_=buffer0(和之前的fusion相比, 该成员变量没有更新), buffer_m1_=buffer1(和之前的fusion相比, 该成员变量有更新), 
    buffer_fusion_(和之前的fusion相比, 该成员变量有更新)={
      T=FusionDataType, channel_id_=0, 
      buffer_(智能指针)->{
        T=shared_ptr<FusionDataType>, head_=0, tail_=1, fusion_callback_=nullptr, capacity_=11, buffer_.size=10, buffer_={0, make_shared<FusionDataType>("0-1", "1-0"), 0, ...}
      }
    }, 
    buffer_m0_.buffer(和cache0绑定)->fusion_callback_={
      函数体, 输入一个const shared_ptr<RawMessage>& m0, 如果buffer_m1_.buffer_(和cache1绑定)为空, 返回void; 如果buffer_m1_.buffer_(和cache1绑定)不为空, 获取buffer_m1_.buffer_(和cache1绑定)最后一个元素m1, 执行buffer_fusion_.Buffer()->Fill(make_shared<FusionDataType>(m0, m1))
    }
  }
  */
  EXPECT_TRUE(fusion.Fusion(&index, m0, m1));
  // fusion.buffer_fusion_.buffer_不为空, 执行完毕:*index=1, m0=make_shared<RawMessage>("0-1"), m1=make_shared<RawMessage>("1-0")
  index++;
  // index=2
  EXPECT_EQ(std::string("0-1"), m0->message);
  // 对的
  EXPECT_EQ(std::string("1-0"), m1->message);
  // 对的
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1));
  // fusion.buffer_fusion_.buffer_不为空, index=2,  所以在fusion.buffer_fusion_.Fetch(index, fusion_data)中*index == buffer_->Tail() + 1, 所以返回false
  cache0->Fill(std::make_shared<RawMessage>("0-2"));
  // 因为cache0->fusion_callback_不为nullptr, 所以执行cache0->fusion_callback_(make_shared<RawMessage>("0-2")). 对于cache0->fusion_callback_(make_shared<RawMessage>("0-2")), 此时buffer_m1_.buffer_(和cache1绑定)不为空, 执行buffer_fusion_.buffer_(和cache1绑定)->Fill(make_shared<FusionDataType>(m0, m1)), m0是make_shared<RawMessage>("0-2"), m1是buffer_m1_.buffer_(和cache1绑定)最后一个元素, 对于buffer_fusion_.buffer_(和cache1绑定)->Fill(make_shared<FusionDataType>(m0, m1)), buffer_fusion_.buffer_->fusion_callback_=nullptr, 所以给buffer_fusion_.buffer_填值, 操作后:  
  /* fusion={
    M0=RawMessage, M1=RawMessage, buffer_m0_=buffer0(和之前的fusion相比, 该成员变量没有更新), buffer_m1_=buffer1(和之前的fusion相比, 该成员变量没有更新), 
    buffer_fusion_(和之前的fusion相比, 该成员变量有更新)={
      T=FusionDataType, channel_id_=0, 
      buffer_(智能指针)->{
        T=shared_ptr<FusionDataType>, head_=0, tail_=2, fusion_callback_=nullptr, capacity_=11, buffer_.size=10, buffer_={0, make_shared<FusionDataType>("0-1", "1-0"), make_shared<FusionDataType>("0-2", "1-0"), 0, ...}
      }
    }, 
    buffer_m0_.buffer(和cache0绑定)->fusion_callback_={
      函数体, 输入一个const shared_ptr<RawMessage>& m0, 如果buffer_m1_.buffer_(和cache1绑定)为空, 返回void; 如果buffer_m1_.buffer_(和cache1绑定)不为空, 获取buffer_m1_.buffer_(和cache1绑定)最后一个元素m1, 执行buffer_fusion_.Buffer()->Fill(make_shared<FusionDataType>(m0, m1))
    }
  }
  */
  EXPECT_TRUE(fusion.Fusion(&index, m0, m1));
  // fusion.buffer_fusion_.buffer_不为空, index=2, fusion.buffer_fusion_.buffer_->tail_=2, 执行完毕: m0=make_shared<RawMessage>("0-2"), m1=make_shared<RawMessage>("1-0")
  index++;
  // index=3
  EXPECT_EQ(std::string("0-2"), m0->message);
  // 对的
  EXPECT_EQ(std::string("1-0"), m1->message);
  // 对的
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1));
  // fusion.buffer_fusion_.buffer_不为空, index=3, 所以在fusion.buffer_fusion_.Fetch(index, fusion_data)中*index == buffer_->Tail() + 1, 所以返回false
  cache1->Fill(std::make_shared<RawMessage>("1-1"));
  // 因为cache1->的fusion_callback_=nullptr, 所以该步是在cache1->buffer_处填值, 操作后: cache1->{T=shared_ptr<RawMessage>, head_=0, tail_=2, fusion_callback_=nullptr, capacity_=11, buffer_.size=10, buffer_={0, make_shared<RawMessage>("1-0"), make_shared<RawMessage>("1-1"), 0, ...}}
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1));
  // fusion.buffer_fusion_.buffer_不为空, index=3, 所以在fusion.buffer_fusion_.Fetch(index, fusion_data)中*index == buffer_->Tail() + 1, 所以返回false
  for (int i = 0; i < 100; i++) {
    cache0->Fill(std::make_shared<RawMessage>(std::string("0-") +
                                              std::to_string(2 + i + 1)));                                   
  }
  // 因为cache0->fusion_callback_不为nullptr, 所以执行cache0->fusion_callback_(make_shared<RawMessage>("0-#(2+i+1)")). 对于cache0->fusion_callback_(make_shared<RawMessage>("0-#(2+i+1)")), 此时buffer_m1_.buffer_(和cache1绑定)不为空, 执行buffer_fusion_.buffer_(和cache1绑定)->Fill(make_shared<FusionDataType>(m0, m1)), m0是make_shared<RawMessage>("0-#(2+i+1)"), m1是buffer_m1_.buffer_(和cache1绑定)最后一个元素, 对于buffer_fusion_.buffer_(和cache1绑定)->Fill(make_shared<FusionDataType>(m0, m1)), buffer_fusion_.buffer_->fusion_callback_=nullptr, 所以给buffer_fusion_.buffer_填值, for循环操作后:  
  /* fusion={
    M0=RawMessage, M1=RawMessage, buffer_m0_=buffer0(和之前的fusion相比, 该成员变量没有更新), buffer_m1_=buffer1(和之前的fusion相比, 该成员变量有更新), 
    buffer_fusion_(和之前的fusion相比, 该成员变量有更新)={
      T=FusionDataType, channel_id_=0, 
      buffer_(智能指针)->{
        T=shared_ptr<FusionDataType>, head_=92, tail_=102, fusion_callback_=nullptr, capacity_=11, buffer_.size=10, buffer_={
          make_shared<FusionDataType>("0-99", "1-1"), 
          make_shared<FusionDataType>("0-100", "1-1"), 
          make_shared<FusionDataType>("0-101", "1-1"), 
          make_shared<FusionDataType>("0-102", "1-1"), 
          make_shared<FusionDataType>("0-92", "1-1"), 
          make_shared<FusionDataType>("0-93", "1-1"), 
          make_shared<FusionDataType>("0-94", "1-1"), 
          make_shared<FusionDataType>("0-95", "1-1"), 
          make_shared<FusionDataType>("0-96", "1-1"), 
          make_shared<FusionDataType>("0-97", "1-1"), 
          make_shared<FusionDataType>("0-98", "1-1"), 
        }
      }
    }, 
    buffer_m0_.buffer(和cache0绑定)->fusion_callback_={
      函数体, 输入一个const shared_ptr<RawMessage>& m0, 如果buffer_m1_.buffer_(和cache1绑定)为空, 返回void; 如果buffer_m1_.buffer_(和cache1绑定)不为空, 获取buffer_m1_.buffer_(和cache1绑定)最后一个元素m1, 执行buffer_fusion_.Buffer()->Fill(make_shared<FusionDataType>(m0, m1))
    }
  }
  */ 
  EXPECT_TRUE(fusion.Fusion(&index, m0, m1));
  // fusion.buffer_fusion_.buffer_不为空, *index=3, fusion.buffer_fusion_.buffer_->head_=92, 所以*index < buffer_->Head(), 执行完毕: *index=102, m0=make_shared<RawMessage>("0-102"), m1=make_shared<RawMessage>("1-1")
  index++;
  //index=103
  EXPECT_EQ(std::string("0-102"), m0->message);
  // 对的
}
// 下边的TEST(AllLatestTest, three_channels)和TEST(AllLatestTest, four_channels)与上边及其类似. 不再分析

TEST(AllLatestTest, three_channels) {
  auto cache0 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  auto cache1 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  auto cache2 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  ChannelBuffer<RawMessage> buffer0(0, cache0);
  ChannelBuffer<RawMessage> buffer1(1, cache1);
  ChannelBuffer<RawMessage> buffer2(2, cache2);
  std::shared_ptr<RawMessage> m;
  std::shared_ptr<RawMessage> m0;
  std::shared_ptr<RawMessage> m1;
  std::shared_ptr<RawMessage> m2;
  uint64_t index = 0;
  fusion::AllLatest<RawMessage, RawMessage, RawMessage> fusion(buffer0, buffer1,
                                                               buffer2);

  // normal fusion
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2));
  cache0->Fill(std::make_shared<RawMessage>("0-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2));
  cache1->Fill(std::make_shared<RawMessage>("1-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2));
  cache2->Fill(std::make_shared<RawMessage>("2-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2));
  cache0->Fill(std::make_shared<RawMessage>("0-1"));
  EXPECT_TRUE(fusion.Fusion(&index, m0, m1, m2));
  index++;
  EXPECT_EQ(std::string("0-1"), m0->message);
  EXPECT_EQ(std::string("1-0"), m1->message);
  EXPECT_EQ(std::string("2-0"), m2->message);
}

TEST(AllLatestTest, four_channels) {
  auto cache0 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  auto cache1 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  auto cache2 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  auto cache3 = new CacheBuffer<std::shared_ptr<RawMessage>>(10);
  ChannelBuffer<RawMessage> buffer0(0, cache0);
  ChannelBuffer<RawMessage> buffer1(1, cache1);
  ChannelBuffer<RawMessage> buffer2(2, cache2);
  ChannelBuffer<RawMessage> buffer3(3, cache3);
  std::shared_ptr<RawMessage> m;
  std::shared_ptr<RawMessage> m0;
  std::shared_ptr<RawMessage> m1;
  std::shared_ptr<RawMessage> m2;
  std::shared_ptr<RawMessage> m3;
  uint64_t index = 0;
  fusion::AllLatest<RawMessage, RawMessage, RawMessage, RawMessage> fusion(
      buffer0, buffer1, buffer2, buffer3);

  // normal fusion
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2, m3));
  cache0->Fill(std::make_shared<RawMessage>("0-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2, m3));
  cache1->Fill(std::make_shared<RawMessage>("1-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2, m3));
  cache2->Fill(std::make_shared<RawMessage>("2-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2, m3));
  cache3->Fill(std::make_shared<RawMessage>("3-0"));
  EXPECT_FALSE(fusion.Fusion(&index, m0, m1, m2, m3));
  cache0->Fill(std::make_shared<RawMessage>("0-1"));
  EXPECT_TRUE(fusion.Fusion(&index, m0, m1, m2, m3));
  index++;
  EXPECT_EQ(std::string("0-1"), m0->message);
  EXPECT_EQ(std::string("1-0"), m1->message);
  EXPECT_EQ(std::string("2-0"), m2->message);
  EXPECT_EQ(std::string("3-0"), m3->message);
}

}  // namespace data
}  // namespace cyber
}  // namespace apollo
```

