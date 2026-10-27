#!/usr/bin/env bash
# 容器启动后为交互 shell 准备可用的 DISPLAY。
#
# devcontainer.json 用 ${localEnv:DISPLAY} 透传宿主显示，但 VS Code 客户端环境
# 里没有 DISPLAY 时（常见于从 Windows 侧启动），容器会带着空 DISPLAY 启动，
# GLFW 报 "[CGL] GLFW Error: X11: Failed to open display"。
# 此时 X11 socket 通常已由 mounts 挂入容器，按它回退到 WSLg / X11 的默认 :0。
set -eu

# 透传成功（DISPLAY 非空）或没有 X11 socket（无显示环境）时不介入
if [ -n "${DISPLAY:-}" ] || [ ! -S /tmp/.X11-unix/X0 ]; then
    exit 0
fi

marker='# CSC4140: DISPLAY fallback'
line='[ -n "${DISPLAY:-}" ] || export DISPLAY=:0'

[ -e "$HOME/.bashrc" ] || : >"$HOME/.bashrc"
for rc in "$HOME/.bashrc" "$HOME/.zshrc"; do
    [ -e "$rc" ] || continue
    grep -qF "$marker" "$rc" && continue
    printf '\n%s\n%s\n' "$marker" "$line" >>"$rc"
done
