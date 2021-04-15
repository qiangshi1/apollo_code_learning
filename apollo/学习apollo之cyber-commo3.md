# `cyber`

## 一: `common`

### 5. `log.h`

#### a) `BUILD`

`BUILD`中的控制方法：

```
cc_library(
    name = "log",
    hdrs = ["log.h"],
    deps = [
        "//cyber:binary",
        "@com_github_google_glog//:glog",
    ],
)
```

依赖`cyber`下边的`binary.h`,和网络上的`glog`？，生成链接库。

#### b) 宏定义`LEFT_BRACKET`和`RIGHT_BRACKET`

常规宏常量，没什么好讲的。

#### c) 宏定义`MODULE_NAME`

调用`cyber/binary.h`的`GetName()`，`cyber/binary.h`中维护了一个`name(string)`，通过`GetName()`得到它。

#### d) 宏定义`ADEBUG_MODULE`
`<<`连接的字符串。估计是用到一些文本的打印过程中。

`VLOG(4)`在`glog`中定义的。字面阅读的话应该是定义了一个详细的等级。

#### e) 宏定义`ADEBUG`

打印信息的前缀。

#### f) 宏定义`ALOG_MODULE_STREAM`

`ALOG_MODULE_STREAM`貌似用到了宏递归？。

简单字符串拼接。

#### g) 宏定义`ALOG_MODULE`

拼接`log_severity`和`module`。

#### h) `AINFO` `AWARN` `AERROR` `AFATAL`
模块名和`INFO`等的拼接。

#### i) `ALOG_MODULE_STREAM_INFO` `ALOG_MODULE_STREAM_WARN` `ALOG_MODULE_STREAM_ERROR` `ALOG_MODULE_STREAM_FATAL`

调用了`glog`吧。

#### i) `AINFO_IF` `AWARN_IF` `AERROR_IF` `AFATAL_IF`

调用了`ALOG_IF`。

#### j) `ALOG_IF`

#### k) `ACHECK` `AINFO_EVERY` `AWARN_EVERY` `AERROR_EVERY`

glog的一些打印语法以及`[模块名]`

#### l) `RETURN_IF_NULL` 

`#if !defined`等价于`#ifndef`吗？

宏中`#`的用法，阻止宏展开，如：

```
#include <iostream>
#include <thread>
#include <mutex>
#define COUT_A(a) std::cout<<#a<<": "<<a<<std::endl;
int main()
{
  std::string Im_string="Im a.";
  COUT_A(Im_string)
}
```
#### m) `RETURN_VAL_IF_NULL` 
内容结构与l)较为类似。

#### n) `RETURN_IF` 
内容结构与l)较为类似。

#### o) `RETURN_VAL_IF` 
内容结构与l)较为类似。

#### p) `_RETURN_VAL_IF_NULL2__`
内容结构与l)较为类似。

#### q) `_RETURN_VAL_IF2__`
内容结构与l)较为类似。

#### r) `_RETURN_IF2__`
内容结构与l)较为类似。

### 6. `log_test.cc`

#### a) `log.h`的测试，暂不介绍了。

### 7. `macros_test.cc`

#### a) `macros.h`的测试，暂不介绍了。


### 8. `environment.h`

#### a) `BUILD`中的控制方法：

```
cc_library(
    name = "environment",
    hdrs = ["environment.h"],
    deps = [
        "//cyber/common:log",
    ],
)
```

链接库，依赖 5.`log.h`。

#### b)  内联函数`GetEnv`

`std::getenv`返回操作系统环境变量。一个`demo`：

```
#include <iostream>
#include <cstdlib>
int main()
{
    if(const char* env_p = std::getenv("PATH"))
        std::cout << "Your PATH is: " << env_p << '\n';
}
```

我的当前系统返回：

```
Your PATH is: /opt/ros/kinetic/bin:/home/shiqiang/bin:/home/shiqiang/.local/bin:/home/shiqiang/bin:/home/shiqiang/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/home/shiqiang/bin:/home/shiqiang/bin
```

`GetEnv`会查找环境变量`${var_name}`。若`${var_name}`非空，返回`${var_name}`；若`${var_name}`为空，返回`default_value`，`default_value`可以作为输入传入，默认为`""`。

#### c)  内联函数`WorkRoot`

获取环境变量`${CYBER_PATH}`。若`${CYBER_PATH}`非空，返回`${CYBER_PATH}`；若`${CYBER_PATH}`为空，返回"/apollo/cyber"。


### 9. `file.h`&`file.cc`

#### a) `BUILD`中的控制方法：

```
cc_library(
    name = "file",
    srcs = ["file.cc"],
    hdrs = ["file.h"],
    deps = [
        "//cyber/common:log",
        "@com_google_protobuf//:protobuf",
    ],
)
```
链接库，依赖 5.`log.h`。

#### b) 枚举`FileType`

有两个底层类型，`TYPE_FILE`文件类型？`TYPE_DIR`目录类型？

#### c) 函数`SetProtoToASCIIFile`

`protobuf`消息转文本文件。

#### d) 函数`SetProtoToASCIIFile`

`protobuf`消息转文本文件。c) 的重载。

#### e) 函数`GetProtoFromASCIIFile`

打开文本文件读取`protobuf`消息。

#### f) 函数`SetProtoToBinaryFile`

`protobuf`消息转二进制文件。

#### g) 函数`GetProtoFromBinaryFile`

打开二进制文件读取`protobuf`消息。

#### h) 函数`GetProtoFromFile`

打开文件读取`protobuf`消息。若是二进制文件（由文件名后缀判定），优先调用二进制文件读取方法；若是文本文件，优先调用文本文件读取方法。

`std::equal`，判断两个`vector`、`string`或者`[]`是否相等。注意短链可以匹配的上长链。一个`demo`：
```
// equal algorithm example
#include <algorithm>  // std::equal
#include <iostream>   // std::cout
#include <string>     //std::string

int main() {
  std::string str1 = "abcdefg";
  std::string str2 = "abcdefg";
  std::string str3 = "abcd";
  std::string str4 = "efg";
  // yes
  if (std::equal(str1.begin(), str1.end(), str2.begin())) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "false" << std::endl;
  }
  // yes
  if (std::equal(str3.begin(), str3.end(), str1.begin())) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "false" << std::endl;
  }
  // false
  if (std::equal(str1.begin(), str1.end(), str3.begin())) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "false" << std::endl;
  }
  // yes
  if (std::equal(str4.rbegin(), str4.rend(), str1.rbegin())) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "false" << std::endl;
  }
}
```

#### i) 函数`GetContent`

以字符串形式读取文件内容。

#### j) 函数`GetAbsolutePath`

返回绝对路径，字符串拼接。

#### k) 函数`PathExists`

判断目录或文件是否存在。

`stat`可以用来判断文件是否存在。如：

```
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
using namespace std;
int main()
{
   struct stat info;
   cout<<stat("test15.cc",&info)<<endl; //存在 0，不存在 -1.
   //不管存在与否，info都会有很多字段（有值的那种）可以参考https://blog.csdn.net/natpan/article/details/81453209
   cout<<"st_dev is:"<<info.st_dev<<endl;
   cout<<"st_ino is:"<<info.st_ino<<endl;
   cout<<"st_mode is:"<<info.st_mode<<endl;
   cout<<"st_nlink is:"<<info.st_nlink<<endl;
   cout<<"st_uid si:"<<info.st_uid<<endl;
   cout<<"st_gid is:"<<info.st_gid<<endl;
   cout<<"st_rdev is:"<<info.st_rdev<<endl;
   cout<<"st_size is:"<<info.st_size<<endl;
   cout<<"st_blksize is:"<<info.st_blksize<<endl;
   cout<<"st_atime is:"<<info.st_atime<<endl;
   cout<<"st_mtime is:"<<info.st_mtime<<endl;
   cout<<"st_ctime is :"<<info.st_ctime<<endl;
}
```
#### l) 函数`DirectoryExists`

判断目录是否存在。注意目录和文件的st_mode是不一样的。一个demo：


```
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
using namespace std;
int main()
{
   struct stat info;
   cout<<stat("test15.cc",&info)<<endl; 
   cout<<"st_mode is:"<<info.st_mode<<endl; // st_mode is:33204
   cout<<stat("/home/shiqiang/",&info)<<endl; 
   cout<<"st_mode is:"<<info.st_mode<<endl;  // st_mode is:16877
}
```
#### l) 函数`Glob`

用正则的方法查找文件。demo：

```
#include <iostream>
#include <vector>
#include <string>
#include <glob.h>

std::vector<std::string> Glob(const std::string &pattern) {
  glob_t globs = {};
  std::vector<std::string> results;
  if (glob(pattern.c_str(), GLOB_TILDE, nullptr, &globs) == 0) {
    for (size_t i = 0; i < globs.gl_pathc; ++i) {
      results.emplace_back(globs.gl_pathv[i]);
      std::cout<<results.at(results.size()-1)<<std::endl;
    }
  }
  globfree(&globs);
  return results;
}

int main()
{
  // 输入的是正则规则
  Glob("test*.cc"); 
}
```

#### m) 函数`CopyFile`

拷贝文件。


#### n) 函数`EnsureDirectory`

确保文件夹存在，如果文件夹不存在就创建它。

一个使用`mkdir`的demo如下：

```
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
int main() { 
    std::string directory_path = "/home/shiqiang/temp_ws/shishisfile/";
    for (size_t i = 1; i < directory_path.size(); ++i) {
      if (directory_path[i] == '/') {
        directory_path[i] = 0;
        std::cout<<"directory_path:"<<directory_path.c_str()<<std::endl;
        if (mkdir(directory_path.c_str(), S_IRWXU) != 0) {  //如果没有成创建
          if (errno != EEXIST) {  //如果没有成功创建的原因不是因为文件夹已经存在
            return 0;
          }
        }
        directory_path[i] = '/';
      }
    }
}
```

#### o) 函数`CopyDir`

拷贝目录。

用`struct dirent`和`readdir()`的一个demo：
```
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>

int main() {
  std::string from="./";
  DIR *directory = opendir(from.c_str());
  struct dirent *entry;
  while ((entry = readdir(directory)) != nullptr) {
    std::cout<<"d_name: "<<entry->d_name<<", d_type: "<<(int)entry->d_type<<std::endl; //文件和目录的d_type是不一样的
  }
  closedir(directory);
  return 0;
}
```
#### p) 函数`Copy`

拷贝，如果是文件就拷贝文件，如果是目录就拷贝目录。

#### q) 函数`RemoveAllFiles`

删除目录下的文件。

#### r) 函数`ListSubPaths`

删除目录下的文件。

#### s) 函数`ListSubPaths`
返回当前目录下的所有子目录。

#### t) 函数`GetFileName`
再看看吧。

#### u) 函数`GetCurrentPath`
获取当前目录。一个demo：
```
#include <unistd.h>
#include <iostream>
#include <string>

int main() {
  char tmp[4096];
  getcwd(tmp,sizeof(tmp));
  std::cout<<tmp<<std::endl;
  return 0;
}
```
#### v) 函数`DeleteFile`

删除文件或者目录

#### w) 函数`CreateDir`

创建目录。







