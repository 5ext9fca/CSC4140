# CSC4140

香港中文大学（深圳）CSC4140 计算机图形学 —— 学期仓库：一份跨平台开发环境（DevContainer / Docker，单一事实来源）加上各次作业实现。

## 仓库结构

| 路径 | 内容 |
|---|---|
| `.devcontainer/` | 学期级跨平台开发环境（`Dockerfile` + `devcontainer.json`，依赖的唯一事实来源） |
| `AGENTS.md` | 面向 AI 编码代理的仓库约定（环境事实、构建、Git） |
| `ass1/` | 作业一：向量 / 矩阵基础与图像重建 |
| `ass2/` | 作业二：光栅化渲染 |
| `ass3/` | 作业三：SVG 光栅化渲染器 |
| `install_dependencies.sh` | 无 Docker 场景下裸机 Linux / WSL2 的兜底依赖安装脚本 |
| `Dependencies.png` | 依赖来源说明图 |

> 各作业自带的 vendored 依赖（`CGL`、`GLEW`、`GLFW`、`tinyexr` 等）在各自作业目录内，本环境只提供系统级依赖。

## 开发环境（DevContainer / Docker）

用一份 `Dockerfile` 固化学期内全部作业的 Linux 依赖，在 Windows（WSL2） / Linux 宿主上通过 VS Code Dev Containers 复现同一环境，彻底解决 `libraw1394-dev`、`rdma-core`、ALSA/X11 等 Linux 专属依赖的跨平台问题。

### 宿主要求

- 宿主系统：Windows（WSL2）或 Linux —— 其他宿主不在支持范围内
- Docker（Linux/WSL2 内原生安装，或 Docker Desktop）
- VS Code + [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) 扩展

### 使用

1. 用 VS Code 打开本仓库根目录（devcontainer 配置所在处）
2. `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**（首次自动构建镜像，约数分钟）
3. 容器启动后 `postCreateCommand` 会校验全部依赖，输出「依赖完整性检查通过」即就绪

容器内目录布局（整个仓库被挂载进来，各作业共用此环境）：

| 容器路径 | 内容 |
|---|---|
| `/workspaces/CSC4140` | 仓库根目录（默认工作区） |
| `/workspaces/CSC4140/ass1 …` | 各次作业，可在容器内直接打开 |

在容器内编译运行作业，例如：

```bash
cmake -S /workspaces/CSC4140/ass3 -B /workspaces/CSC4140/ass3/build
cmake --build /workspaces/CSC4140/ass3/build
```

## 图形窗口（OpenGL）

| 宿主 | 说明 |
|---|---|
| Windows WSL2（WSLg） | 开箱即用 |
| Linux X11/Wayland | 若无法弹窗，先在宿主执行 `xhost +local:` |
| 无显示器的服务器 | 删除 `devcontainer.json` 中的 X11 挂载与 `DISPLAY` 两项 |

- 容器内报 `X11: Failed to open display` / `could not initialize GLFW!` 时，先查容器内 `echo $DISPLAY`：
  - 为空：说明宿主环境变量没传进来（`containerEnv` 里的 `${localEnv:DISPLAY}` 只做透传、不设回退值）。容器启动时 `.devcontainer/post-start.sh` 会在 `/tmp/.X11-unix/X0` 存在的前提下把 `export DISPLAY=:0` 写入 `~/.bashrc`；**已打开的终端需新建终端**（或 `source ~/.bashrc`）才生效
  - 仍失败：确认 `devcontainer.json` 的 X11 挂载生效（容器内 `ls /tmp/.X11-unix`）、Linux 宿主已执行 `xhost +local:`，或临时用 `DISPLAY=:0 ./build/draw ...` 验证
- 默认软件渲染（Mesa llvmpipe），受支持的宿主均可运行
- 需要 GPU 硬件加速时，取消 `devcontainer.json` 中 `runArgs` 里 `--device=/dev/dri` 的注释后 Rebuild

## 镜像源

默认使用中国镜像 [USTC](https://mirrors.ustc.edu.cn)（构建期生效）：

- 改回 Debian 官方源：将 `devcontainer.json` 中 `"MIRROR"` 置为 `""` 后 Rebuild
- 换用其他镜像：改为对应地址（如 `https://mirrors.tuna.tsinghua.edu.cn`）后 Rebuild
- 手动构建：`docker build --build-arg MIRROR= -t csc4140-env .devcontainer`

## 依赖清单（28 个包）

| 分组 | 包 |
|---|---|
| 工具链 | `g++` `build-essential` `cmake` `ninja-build` `pkg-config` |
| 线性代数 | `libeigen3-dev` |
| 图像格式 | `libtiff-dev` `libjpeg-dev` `libpng-dev` `libgif-dev` |
| HDR 图像 | `libopenexr-dev` |
| 计算机视觉 | `libopencv-dev` |
| 并行 | `libtbb-dev`（`std::execution::par` 后端） |
| HPC | `hwloc` `libibverbs-dev` `rdma-core` |
| FireWire 相机 | `libraw1394-dev` |
| 多媒体 | `libasound2-dev` `libavcodec-dev` `libavformat-dev` `libswscale-dev` |
| 窗口/图形 | `libx11-dev` `libxext-dev` `xorg-dev` `freeglut3-dev` `mesa-common-dev` `libgl1-mesa-dev` |
| 字体 | `libfreetype6-dev` |

基础镜像：`mcr.microsoft.com/devcontainers/cpp:1-debian-12`（Debian 12 bookworm，内置 git、gdb 与非 root 用户 `vscode`）。

## 与其他部署方式的关系

根目录的 `.devcontainer` 为学期内主开发环境（唯一事实来源）；同目录的 `install_dependencies.sh` 保留为无 Docker 场景下裸机 Linux/WSL2 的兜底安装脚本，二者与 `devcontainer.json` 中 `postCreateCommand` 的校验清单保持同步。
