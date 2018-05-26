# functional-dependencies
term project of database course

authors: [Zhao Shangming](https://github.com/zhaosm) and [Yang Bifei](https://github.com/effie-0)

## Usage

在`main.cpp`文件中，

```c++
std::string ifile = "input-filepath/filename";
std::string ofile = "output-filepath/filename";
```

更改相应引号内容即可改变输入/输出文件。

## Analysis

由于本项目中大量使用bitset用于存储列信息，并且bitset模版没有自带的比较函数(<, >)，严重影响了项目中map, set的效率，所以整体运行速度较慢。

除此之外，DFD论文中很多细节没有写出导致实现困难也是最终运行速度减慢的原因。

## Result

对于15列将近10万行的输入数据，最终运行时间大致100s。