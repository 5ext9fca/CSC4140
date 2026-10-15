# CSC4140 学期开发环境（方案一：DevContainer / Docker —— 单一事实来源）
# 依赖来源: 仓库根目录 Dependencies.png 与作业说明，覆盖学期内所有作业
# 镜像源: 默认中国镜像 (USTC)，构建时可用 --build-arg MIRROR= 置空恢复官方源
FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

ARG MIRROR=https://mirrors.ustc.edu.cn
ARG DEBIAN_SUITE=bookworm

RUN set -eux; \
    if [ -n "${MIRROR}" ]; then \
        printf 'deb %s/debian %s main contrib non-free non-free-firmware\n' \
            "${MIRROR}" "${DEBIAN_SUITE}" > /etc/apt/sources.list; \
        printf 'deb %s/debian %s-updates main contrib non-free non-free-firmware\n' \
            "${MIRROR}" "${DEBIAN_SUITE}" >> /etc/apt/sources.list; \
        printf 'deb %s/debian-security %s-security main contrib non-free non-free-firmware\n' \
            "${MIRROR}" "${DEBIAN_SUITE}" >> /etc/apt/sources.list; \
        rm -f /etc/apt/sources.list.d/debian.sources; \
    fi; \
    apt-get update; \
    apt-get -y upgrade; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        g++ \
        build-essential cmake ninja-build pkg-config \
        libeigen3-dev libtiff-dev libjpeg-dev libpng-dev libgif-dev \
        libopenexr-dev libraw1394-dev \
        libopencv-dev libtbb-dev hwloc libibverbs-dev rdma-core \
        libasound2-dev libavcodec-dev libavformat-dev libswscale-dev \
        libx11-dev libxext-dev freeglut3-dev mesa-common-dev \
        libfreetype6-dev xorg-dev libgl1-mesa-dev; \
    apt-get clean; \
    rm -rf /var/lib/apt/lists/*
