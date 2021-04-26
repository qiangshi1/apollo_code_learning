### 31. ` intra_reader.h`

- 这个代码写得很漂亮，很干净。

#### a) build的红控制

```b
cc_library(
    name = "intra_reader",
    hdrs = ["intra_reader.h"],
    deps = [
        ":blocker_manager",
    ],
)
```

虽然build中没有写reader.h，但是在头文件里确实包含了：`#include "cyber/node/reader.h"`，神奇

#### b) 类IntraReader

- 继承了`apollo::cyber::Reader<MessageT`
- To be filled

#### b) 类IntraWriter

- 继承了`apollo::cyber::Reader<MessageT`
- To be filled