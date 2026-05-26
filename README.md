# ZYBypass

[![macOS](https://img.shields.io/badge/platform-macOS-000000?logo=apple)](https://www.apple.com/macos/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/CMake-4.0+-064F8C?logo=cmake)](https://cmake.org/)
[![Apple Silicon](https://img.shields.io/badge/Apple%20Silicon-arm64-FF2D55.svg)](https://developer.apple.com/documentation/apple-silicon)

ZYBypass 是一个面向 macOS (Apple Silicon) 的底层 JNI 动态库，提供对 OpenJDK HotSpot VM 运行时内部结构（OOP、Klass、Method、CodeCache 等）的直接访问能力，同时集成 macOS 原生覆盖层渲染（Overlay）功能。

> **⚠️ 免责声明**：本项目涉及对 JVM 内部数据结构的直接操作，仅用于学习研究目的。使用不当可能导致 JVM 崩溃或数据损坏。

---

## ✨ 功能特性

- **🔍 HotSpot 内部探查**
  - 访问 OOP（Ordinary Object Pointer）、Klass、InstanceKlass 等核心对象
  - 遍历对象字段（含基本类型与引用类型）
  - 查询 ClassLoaderDataGraph，定位已加载类

- **⚙️ 运行时方法操作**
  - 使 JIT 编译后的 nmethod 失效（make_not_entrant）
  - 遍历 CodeCache，精确清理指定方法的编译代码

- **🖥️ macOS 原生覆盖层**
  - Objective-C/C++ 实现的独立渲染层
  - 支持外部画面叠加（如辅助信息显示）

---

## 📋 系统要求

| 项目 | 要求 |
|------|------|
| 操作系统 | macOS (Apple Silicon / arm64) |
| 编译器 | Clang (支持 C++23) |
| JDK | OpenJDK / Oracle JDK (提供 JNI 与 libjvm.dylib) |
| 构建工具 | CMake ≥ 4.0，Ninja |

---

## 🗂️ 项目结构

```
zybypass/
├── CMakeLists.txt          # CMake 构建配置
├── src/
│   ├── zybypass.cpp        # 主入口与 JNI 导出函数实现
│   ├── hotspot/            # HotSpot VM 内部结构封装
│   │   ├── classfile/      # 类加载相关（ClassLoaderData、ClassLoaderDataGraph）
│   │   ├── code/           # 代码缓存相关（CodeBlob、nmethod、CodeCache、ScopeDesc）
│   │   ├── debugger/       # aarch64 调试与符号解析
│   │   ├── gc/             # GC 堆结构（Epsilon、G1、ZGC、Shenandoah、Serial、Parallel）
│   │   ├── memory/         # 内存管理（Universe、CodeHeap、VirtualSpace）
│   │   ├── oops/           # 对象模型（OOP、Klass、Method、Field、Symbol 等）
│   │   ├── runtime/        # 运行时（JVM 初始化、Thread、BasicType、AccessFlags）
│   │   ├── types/          # 类型系统基础
│   │   └── utilities/      # 工具类（Array、GrowableArray）
│   └── render/             # macOS 覆盖层渲染
│       ├── overlay.h/mm    # 覆盖层启动/停止控制
│       ├── ascii_png.h     # ASCII 艺术纹理
│       └── widgets_png.h   # UI 控件纹理
└── build/                  # 构建输出目录
```

---

## 🔧 技术细节

- **语言标准**：C++23 (`-std=c++23`)，Objective-C/C++
- **编译优化**：Release 模式启用 `-O3`、Thin LTO、死代码剥离、符号剥离
- **目标微架构**：`-mcpu=apple-m4`
- **PIC**：全代码位置无关 (`-fPIC`)
- **链接框架**：
  - `CoreFoundation`、`Foundation`、`Cocoa`、`CoreGraphics`
  - `JavaRuntimeSupport`（可选）
  - `AudioToolbox`（可选）
  - `libjvm.dylib`

---

## ⚠️ 注意事项

1. **平台限制**：当前仅支持 macOS arm64（Apple Silicon）。x86_64 需要额外适配。
2. **JDK 兼容性**：HotSpot 内部数据结构在不同 JDK 版本间可能变化，建议锁定 JDK 版本。
3. **稳定性**：直接操作 `CodeCache` 与 `nmethod` 属于未公开 API 行为，可能触发 JVM 断言或崩溃。
4. **JIT 写保护**：在修改代码缓存时需临时关闭 `pthread_jit_write_protect_np`，完成后务必恢复。

---

## 📄 许可证

[MIT License](LICENSE)（或根据实际项目许可证填写）

---

> **提示**：本项目代码仅供学习交流，请勿用于非法用途。
