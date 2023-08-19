# ArcFormulator 几何曲线计算

## 项目描述

`ArcFormulator` 是一个用于计算两点间形成的曲线的工具。它可以用于科学计算、数据可视化、图形设计等各种场景。

该工具的核心功能是根据用户输入的两个点，计算和生成它们之间的各种类型的曲线。这些曲线类型可以包括但不限于贝塞尔曲线、样条曲线等。
不仅能够计算曲线的数学方程和参数，还能够将这些曲线可视化地呈现出来。用户可以轻松地通过简洁的图形界面选择点、调整参数，并实时查看曲线的形状。
计算结果可以导出为常见的数据格式（如 CSV、JSON 等），方便进一步的分析和处理。

以下是该项目的一些主要用途和应用场景：

- **科学计算与工程设计**：帮助工程师和科学家们更有效地进行精确计算和数据分析。
- **数据可视化**：用于生成精美的图表和曲线图，支持复杂的数据可视化项目。
- **图形设计与艺术创作**：为设计师提供一个灵活的工具，帮助他们轻松地创造优美的曲线和形状。
- **教育与培训**：作为一个教学工具，帮助学生更直观地理解和学习曲线和几何的相关概念。

`ArcFormulator` 是一个开源项目，鼓励社区的参与和贡献。我们的目标是创建一个既强大又易于使用的曲线计算工具，能够满足不同用户群体的广泛需求。


## 目录结构
```
.
├── 3dparty/          # 第三方依赖库
│   
├── build/            # 临时构建文件，编译输出文件夹
│
├── doc/              # 文档目录，包括项目说明、API文档等
│
├── out/              # 输出目录，可能包含编译生成的二进制文件、库文件等
│   └── build/        # CMake产生的构建文件目录
│
├── resources/        # 资源文件目录，如图片、图标、配置文件等
│   ├── images/       # 图片资源文件夹
│   ├── icons/        # 图标资源文件夹
│   └── ...           # 其他资源文件
│
├── src/              # 源代码目录
│   ├── ArcCore/      # 包含项目核心（如几何计算）的源代码文件
│   └── ui/           # 包含用户界面相关的源代码文件
│
├── CMakeLists.txt    # 顶层CMake构建文件
├── README.md         # 项目的README文件，描述项目信息、使用方法等
└── LICENSE           # 开源许可证文件
```
- 3dparty/：用于存放项目依赖的第三方库和工具。

- build/：这是一个临时目录，用于存放CMake构建生成的临时文件和输出。

- doc/：这个目录用于存放与项目相关的文档，比如设计文档、用户手册、API文档等。

- out/：用于存放构建输出文件，比如编译生成的二进制文件和库文件。

- resources/：存放项目用到的资源文件，如图片、图标、配置文件等。

- src/：项目的源代码目录。它通常包含各种子目录，每个子目录负责项目的一个特定部分。在ArcCore/包含了项目的核心计算逻辑，而ui/则包含了用户界面相关的代码。

- CMakeLists.txt：项目的顶层CMake构建文件。这个文件包含了项目的构建规则，并可能引用其他子目录中的CMakeLists.txt文件。

- README.md：自述文件。

- LICENSE：该文件包含项目的许可证信息。它定义了其他人可以如何使用这个项目的代码。


## 功能特性

- 计算两点间的贝塞尔曲线
- 生成和可视化曲线图形
- 导出曲线数据到各种格式（如 CSV、JSON 等）（计划实现）

## 安装与使用

### 安装要求

- C++11 或更高版本
- Qt 5.15.2
- CMake 3.10 或更高版本

### 构建步骤

```bash
git clone https://github.com/1619513467/ArcFormulator.git
cd ArcFormulator
mkdir build
cd build
cmake ..
make
