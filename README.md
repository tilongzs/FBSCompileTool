# FBSCompileTool
FlatBuffers界面编译辅助工具。
# 概述
本软件是flatc.exe的界面辅助工具。调用flatc.exe，将fbs文件编译为对应编程语言。
目前支持C++和Dart两种语言。
![](https://raw.githubusercontent.com/tilongzs/FBSCompileTool/main/doc/screenshot.png)

# 编译环境

- VisualStudio 2022（含MFC组件）
- Windows 11

# 使用方法

###  FlatBuffers环境配置
1. 输入protoc.exe的路径。
2. 选择编程语言。
3. 【可选】输入额外的参数。

###  生成文件的存放路径配置

可以选择与proto文件同级的文件夹，也可以指定文件夹。

### 使用fbs方式配置

可以选择单个fbs文件，在编辑框里输入fbs文件路径。

也可以指定包含fbs文件的文件夹，在编辑框里输入文件夹的路径，这将会把整个文件夹（**不递归包含**子文件夹）下的所有fbs文件转换。

### 开始转换

点击“开始转换”按钮，即可开始转换。完成或者错误消息将显示在右边的窗口中。
