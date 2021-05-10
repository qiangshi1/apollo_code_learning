## 代码总览

file.h/file.cc/file_test.cc. 

proto与文件之间的读写转, 和对文件(夹)的一些操作.

## 功能/知识

### 1. 方法`GetProtoFromFile()`

- 打开文件读取`protobuf`消息.若是二进制文件(由文件名后缀判定), 优先调用二进制文件读取方法; 若是文本文件, 优先调用文本文件读取方法.
- `std::equal`, 判断两个`vector` `string`或者`[]`是否相等. 注意短链可以匹配的上长链. 一个demo: 
    ```c++
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
### 2. 方法`PathExists()`

- 判断目录或文件是否存在.
- `stat`可以用来判断文件是否存在, 如: 
    ```c++
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
### 3. 函数`DirectoryExists()`

- 判断目录是否存在. 注意目录和文件的字段`st_mode`是不一样的. 一个demo: 
    ```c++
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

### 4. 方法`Glob()`

- 用正则的方法查找文件. demo:
    ```c++
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

### 5. 方法`EnsureDirectory()`

- 确保文件夹存在, 如果文件夹不存在就创建它. 
- 一个使用`mkdir`的demo如下：
    ```c++
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
### 6. 方法`CopyDir()`

- 作用是拷贝目录. 
- 用`struct dirent`和`readdir()`的一个demo：
    ```c++
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

### 7. 方法`GetCurrentPath()`
- 获取当前路径. 一个demo:
    ```c++
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