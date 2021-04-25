### 26. `class_loader_register_macro.h`

- 它的build在24)中已经介绍。

#### a) 宏定义`CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)`

- \#运算符，字符串化；\##运算符，粘合字符串。
- 所以宏展开之后应该是：

```C++
// #define CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)  
namespace {                                                             
  struct ProxyTypeUniqueID {                                            
    ProxyTypeUniqueID() {                                               
      apollo::cyber::class_loader::utility::RegisterClass<Derived, Base>( 
          "Derived", "Base");                                               
    }                                                                     
  };                                                                      
  static ProxyTypeUniqueID g_register_class_UniqueID;                 
  }
```

#### b) 宏定义`CLASS_LOADER_REGISTER_CLASS_INTERNAL_1(Derived, Base, UniqueID)`

- 和a)一样

#### c) 宏定义`#define CLASS_LOADER_REGISTER_CLASS(Derived, Base)`

- 相对于a),`UniqueID`被替换成了`__COUNTER__`。

### 27. `class_loader.h`&`class_loader.cc`

- 它的build在24)中已经介绍。
- 代码中维护了一个类: `ClassLoader`.

#### a) 类`ClassLoader`

- 它维护了一个类。

#### b) 类构造函数: `ClassLoader::ClassLoader()`

- 初始化一些成员: `library_path_`,`loadlib_ref_count_`,`classobj_ref_count_`

#### c) 类析构函数: `ClassLoader::~ClassLoader()`

- 调用`UnloadLibrary();`

#### d) 类函数: `ClassLoader::IsLibraryLoaded()`

- 类是否被加载？

#### e) 类函数: `ClassLoader::LoadLibrary()`

- 加载类，每加载一次，`loadlib_ref_count_`加1.

#### f) 类函数: `ClassLoader::UnloadLibrary()`

- 卸载类，每卸载一次，`loadlib_ref_count_`减1.

#### g) 类函数: `ClassLoader::GetLibraryPath()`

- 返回：`library_path_`

#### h) 类函数: `ClassLoader::GetValidClassNames()`

- 模板函数。
- 作用？

#### i) 类函数: `ClassLoader::CreateClassObj()`

- 模板函数。
- 作用？
- `std::shared_ptr`在构造时可以传入一个析构器https://blog.csdn.net/baidu_31541363/article/details/95802210

```C++
std::shared_ptr<Base> classObjSharePtr(class_object, std::bind(&ClassLoader::OnClassObjDeleter<Base>, this,std::placeholders::_1));
```



#### j) 类函数: `ClassLoader::IsClassValid()`

- 模板函数。
- 根据名字找类？



#### k) 类函数: `ClassLoader::OnClassObjDeleter()`

- 模板函数。
- 删除一个`Base*`

### 27. `class_loader_manager.h`&`class_loader_manager.cc`

- 有一个类`ClassLoaderManager`，这个类应该是管理多个`class_loader`
- 这块看得云里雾里的原因大概有太多代码嵌套，类嵌套，this指针，另外一些类的目的（尤其是类名称的含义）不是很,,, 再看吧。

#### a) Build

```bazel
cc_library(
    name = "class_loader_manager",
    srcs = ["class_loader_manager.cc"],
    hdrs = ["class_loader_manager.h"],
    deps = [
        ":class_loader",
    ],
```

链接库。

#### b) 类构造函数`ClassLoaderManager::ClassLoaderManager()`

#### c) 类析构函数`ClassLoaderManager::~ClassLoaderManager()`

#### d) 类函数`ClassLoaderManager::LoadLibrary()`

- 向`libpath_loader_map_`更新一对键值，键: `library_path`; 值`new class_loader::ClassLoader(library_path)`

#### e) 类函数`ClassLoaderManager::UnloadAllLibrary()`

- 卸载所有的`class_loader`

#### g) 类函数`ClassLoaderManager::IsLibraryValid()`

- 是否有`library_name`

#### h) 类函数`ClassLoaderManager::CreateClassObj()`

- 模板函数
- class_loader里的方法写得太乱了。
- 似乎是创建一个类实例指针。

- `

#### i) 类函数`ClassLoaderManager::CreateClassObj()`

- 模板函数
- h)的重载
- ？

- 

#### j) 类函数`ClassLoaderManager::IsClassValid()`

- 模板函数
- ？

#### k) 类函数`ClassLoaderManager::GetValidClassNames()`

- 模板函数
- ？

#### l) 类函数`ClassLoaderManager::GetClassLoaderByLibPath()`

- 键值查找`libpath_loader_map_`

#### m) 类函数`ClassLoaderManager::GetAllValidClassLoaders()`

- map转vector，map的second

#### n) 类函数`ClassLoaderManager::GetAllValidLibPath()`

- map转vector，map的first

- 

#### o) 类函数`ClassLoaderManager::UnloadLibrary()`

- 卸载`class_loader`
- ？