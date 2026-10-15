#!/usr/bin/env bash
# 无人值守安装 CSC4140 学期全部作业的依赖项 (Ubuntu/Debian)
# 依赖来源: Dependencies.png，清单与 .devcontainer/Dockerfile 保持同步（单一事实来源）
# 默认通过中国镜像 (USTC) 下载，仅本次运行生效，不修改任何系统配置
# 可用环境变量覆盖: MIRROR=... ./install_dependencies.sh
set -euo pipefail

MIRROR="${MIRROR:-https://mirrors.ustc.edu.cn}"

if [[ $EUID -ne 0 ]]; then
    SUDO="sudo"
else
    SUDO=""
fi

. /etc/os-release
suite="${VERSION_CODENAME:-}"
if [[ -z "$suite" ]]; then
    echo "无法确定发行版代号，改用系统默认源" >&2
    APT_MIRROR_OPTS=()
else
    if [[ "$ID" == "ubuntu" ]]; then
        m="$MIRROR/ubuntu"
        comps="main restricted universe multiverse"
    else
        m="$MIRROR/debian"
        comps="main contrib non-free non-free-firmware"
        security_m="$MIRROR/debian-security"
    fi
    tmp_sources="$(mktemp)"
    if [[ "$ID" == "ubuntu" ]]; then
        cat >"$tmp_sources" <<EOF
deb $m $suite $comps
deb $m $suite-updates $comps
deb $m $suite-security $comps
EOF
    else
        cat >"$tmp_sources" <<EOF
deb $m $suite $comps
deb $m $suite-updates $comps
deb $security_m $suite-security $comps
EOF
    fi
    APT_MIRROR_OPTS=(-o "Dir::Etc::sourcelist=$tmp_sources" -o "Dir::Etc::sourceparts=-")
    trap 'rm -f "$tmp_sources"' EXIT
fi

$SUDO apt "${APT_MIRROR_OPTS[@]}" update
$SUDO apt "${APT_MIRROR_OPTS[@]}" upgrade -y
$SUDO apt "${APT_MIRROR_OPTS[@]}" install -y g++

$SUDO apt "${APT_MIRROR_OPTS[@]}" install -y \
    build-essential cmake ninja-build pkg-config \
    libeigen3-dev libtiff-dev libjpeg-dev libpng-dev libgif-dev \
    libopenexr-dev libraw1394-dev \
    libopencv-dev libtbb-dev hwloc libibverbs-dev rdma-core \
    libasound2-dev libavcodec-dev libavformat-dev libswscale-dev \
    libx11-dev libxext-dev freeglut3-dev mesa-common-dev \
    libfreetype6-dev xorg-dev libgl1-mesa-dev

echo "所有依赖项安装完成。"
