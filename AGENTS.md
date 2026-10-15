# AGENTS.md

面向在本仓库（CSC4140）及其 DevContainer 环境中工作的 AI 编码代理。

## 仓库定位

- 本仓库为 CSC4140 课程总仓，包含学期开发环境与各次作业实现。
- 仓库根的 `.devcontainer/` 只存放学期环境配置（`Dockerfile`、`devcontainer.json`），不要往这里添加作业代码或构建产物。
- 各次作业位于 `ass1/`、`ass2/`、`ass3/`，容器内统一挂载在 `/workspaces/CSC4140/`（如 `/workspaces/CSC4140/ass3`），共用本环境。
- 作业目录只保留源码、构建配置与运行结果，**不含报告**（报告已移除，勿再提交 `REPORT.md`、`Report.pdf` 等）。

## 环境事实（容器内）

- 基础镜像 `mcr.microsoft.com/devcontainers/cpp:1-debian-12`（Debian 12 bookworm），默认用户为非 root 的 `vscode`。
- 学期全部 28 个依赖已在镜像构建时装好。**不要**重复 `apt install` 已有依赖，也**不要**在运行中的容器里临时装包来"解决"问题——容器可随时重建，运行期改动会丢失。
- 依赖的单一事实来源是 `.devcontainer/Dockerfile`。
- apt 镜像源（USTC）在构建期经 `MIRROR` build arg 固化，**不要**在运行时修改 `/etc/apt/sources.list`。

## 修改依赖时

- 只改 `.devcontainer/Dockerfile`，改完需 Rebuild Container 才生效；直接在容器里 `apt install` 是错误做法。
- `.devcontainer/devcontainer.json` 中 `postCreateCommand` 的 `dpkg -s` 校验列表必须与 `Dockerfile` 安装列表保持同步，改一处必须改另一处。
- 根目录的 `install_dependencies.sh` 是无 Docker 场景的裸机兜底脚本，依赖变更需三处同步。
- 原则：**所有依赖项均不多余**。新增包前先确认作业确实引用（查作业的 `CMakeLists.txt` 与源码 `#include`），删除前同样确认无引用。

## 构建作业

- 在容器内构建各作业，例如：
  ```bash
  cmake -S /workspaces/CSC4140/ass3 -B /workspaces/CSC4140/ass3/build
  cmake --build /workspaces/CSC4140/ass3/build
  ```
- 作业自带 vendored 依赖（如 `ass3/CGL/deps` 下的 GLEW、GLFW、tinyexr 等）使用仓库内版本，**禁止**为其安装系统同名库。
- 构建产物留在各作业自己的 `build/` 目录，不入库。

## 图形 / GPU

- OpenGL 程序依赖 `DISPLAY` 环境变量与 `/tmp/.X11-unix` 挂载；WSL2（WSLg）下开箱即用。
- GL 窗口打不开时，先排查 `DISPLAY` 与 X11 挂载，不要猜测缺包并安装。
- GPU 直通默认关闭（`runArgs` 中 `--device=/dev/dri` 已注释），默认软件渲染（llvmpipe）是保底路径。

## 验证

- 宿主与容器内都未必装有 Docker：修改配置后无法本地实测构建属正常，应提醒用户 Rebuild 验证。
- `devcontainer.json` 是 JSONC（允许 `//` 注释），修改后去掉注释再解析校验 JSON 合法性；注意字符串中的 `https://` 会被朴素的正则误伤。
- `Dockerfile` 的 `RUN` 脚本可提取出来 `bash -n` 做语法检查。

## Git 约定

- 分支 `main`，提交信息用中文祈使句简述变更。
- 不提交构建产物、临时文件、报告与任何密钥。
