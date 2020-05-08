#### 模仿了tree工具

##### 只使用C++基础语法，这是我学完`C++PrimerPlus`之后的练手项目(可能方向不太对?:flushed:),只有一个`.cpp`和两个`.h`文件

-  `regex_tree.h` 负责使用正则引擎判定命令行参数的合法性
- `file_tree.h`负责二进制读写文件功能，这个文件是缓存查询过的目录信息的，它默认为E:\\tree.ini，可自行修改PATH常量

用到的C++11自带的库：

```

#include <regex>	//提供正则表达式功能
#include <io.h>	//提供对文件目录的读取功能
#include <libloaderapi.h>	//提供获取tree.exe路径的功能

#include <iostream>	//以下为基础库
#include <string>	
#include <vector>	
#include <iterator>
#include <algorithm>	
#include <iomanip>
#include <set>
#include <map>	

```

