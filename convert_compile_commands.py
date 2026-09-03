#!/usr/bin/env python3
"""
convert_compile_commands.py

将 EIDE 生成的 ARMCC compile_commands.json 转换为 clangd 兼容版本。

用法:
    python convert_compile_commands.py <armcc_compile_commands_json> [--output <path>] [--root <path>]

参数:
    <armcc_compile_commands_json>   EIDE 生成的 ARMCC compile_commands.json 路径 (必填)
    --output <path>                 输出路径 (默认: 脚本同级目录下的 compile_commands.json)
    --root <path>                   项目根目录 (默认: 从输入文件自动检测)

示例:
    python convert_compile_commands.py MDK-ARM/build/f103_menu_tree/compile_commands.json
    python convert_compile_commands.py D:/project/MDK-ARM/build/target/compile_commands.json --root D:/project
    python convert_compile_commands.py ./build/compile_commands.json --output ./compile_commands.json

功能:
    1. 读取 ARMCC compile_commands.json
    2. 自动检测项目根目录 (所有源文件和 directory 的最近公共祖先)
    3. 解析每个条目，提取 -I 和 -D 标志，过滤 ARMCC 专有标志
    4. 跳过汇编文件 (.s / .S)
    5. 生成 clang 兼容的 compile_commands.json
    6. 写入输出路径 (默认为脚本所在目录)
"""

import argparse
import json
import os
import sys
from pathlib import Path

try:
    import shlex
except ImportError:
    shlex = None


# ============================================================
# clangd 目标配置
# ============================================================
TARGET_FLAGS = ["--target=armv7m-none-eabi", "-mcpu=cortex-m3", "-std=gnu99"]

# ARMCC 专有布尔标志 (无后续参数，直接跳过)
ARMCC_BOOL_FLAGS = {
    "--li", "--c99", "--split_sections", "--no_depend_system_headers",
    "--enum_is_int", "--gnu", "--signed_chars", "--split_ldm",
    "--execute_only", "--strict", "-Otime",
}

# ARMCC 专有标志 (带 1 个后续参数，跳过标志+参数)
ARMCC_ARG_FLAGS = {
    "--cpu", "--depend", "-o",
}


def find_common_ancestor(paths: list[Path]) -> Path:
    """
    找到一组路径的最近公共祖先目录。
    用于从 compile_commands.json 的 file/directory 字段自动检测项目根目录。
    """
    if not paths:
        return Path.cwd()

    # 全部转为绝对路径
    abs_paths = [p.resolve() for p in paths if p.exists() or True]

    # 取第一个路径为基准，逐级向上检查
    candidate = abs_paths[0]
    if not candidate.is_dir():
        candidate = candidate.parent

    while True:
        if all(str(p).startswith(str(candidate)) for p in abs_paths):
            return candidate
        parent = candidate.parent
        if parent == candidate:
            return candidate
        candidate = parent


def parse_armcc_command(command_str: str) -> dict:
    """
    解析 ARMCC 命令行，提取 -I, -D 和 clang 兼容标志。
    ARMCC 专有标志被过滤掉。
    """
    # 使用 shlex 解析 (正确处理引号)
    if shlex:
        try:
            tokens = shlex.split(command_str)
        except ValueError:
            tokens = command_str.split()
    else:
        tokens = command_str.split()

    includes = []
    defines = []
    other_flags = []

    i = 0
    while i < len(tokens):
        token = tokens[i]

        # 跳过编译器可执行文件路径 (第一个 token)
        if i == 0:
            i += 1
            continue

        # -I<路径> 或 -I <路径>
        if token.startswith("-I"):
            if len(token) > 2:
                includes.append(token[2:])
            elif i + 1 < len(tokens):
                i += 1
                includes.append(tokens[i])
            i += 1
            continue

        # -D<宏> 或 -D <宏>
        if token.startswith("-D"):
            if len(token) > 2:
                defines.append(token[2:])
            elif i + 1 < len(tokens):
                i += 1
                defines.append(tokens[i])
            i += 1
            continue

        # ARMCC 专有: --apcs=xxx, --diag_suppress=xxx (等号形式，单 token)
        if token.startswith("--apcs=") or token.startswith("--diag_suppress="):
            i += 1
            continue

        # ARMCC 专有: 带 1 个参数的标志
        if token in ARMCC_ARG_FLAGS:
            i += 2
            continue

        # ARMCC 专有: 布尔标志 (无参数)
        if token in ARMCC_BOOL_FLAGS:
            i += 1
            continue

        # clang 兼容标志
        if token in ("-g", "-c") or token.startswith("-O"):
            other_flags.append(token)
            i += 1
            continue

        # 源文件路径 (.c 结尾且不以 - 开头) — 跳过，使用 file 字段
        if token.endswith(".c") and not token.startswith("-"):
            i += 1
            continue

        # 其他未知标志，跳过
        i += 1

    return {"includes": includes, "defines": defines, "other_flags": other_flags}


def convert_include_path(inc_path: str, armcc_dir: Path, project_root: Path) -> str:
    """
    将 ARMCC 的 -I 路径转换为相对于项目根目录的路径。

    ARMCC compile_commands.json 的 directory 字段通常是 MDK-ARM/，
    其 -I 路径相对于该目录。需要转为相对于项目根目录。
    """
    if os.path.isabs(inc_path):
        abs_path = Path(inc_path)
    else:
        abs_path = (armcc_dir / inc_path).resolve()

    try:
        rel_path = abs_path.relative_to(project_root)
        return str(rel_path).replace("\\", "/")
    except ValueError:
        return str(abs_path).replace("\\", "/")


def convert_entry(entry: dict, project_root: Path) -> dict | None:
    """将单个 ARMCC compile_commands 条目转换为 clang 兼容格式。"""
    file_path = entry.get("file", "")
    command_str = entry.get("command", "")
    armcc_dir = Path(entry.get("directory", ""))

    # 跳过汇编文件
    if file_path.lower().endswith((".s", ".S")):
        return None

    parsed = parse_armcc_command(command_str)

    # 转换 include 路径
    inc_flags = []
    for inc in parsed["includes"]:
        converted = convert_include_path(inc, armcc_dir, project_root)
        inc_flags.append(f"-I{converted}")

    # 构建 -D 标志
    def_flags = [f"-D{d}" for d in parsed["defines"]]

    # 源文件相对路径 (相对于项目根目录)
    try:
        file_rel = Path(file_path).relative_to(project_root)
        file_rel_str = str(file_rel).replace("\\", "/")
    except ValueError:
        file_rel_str = file_path.replace("\\", "/")

    # 组装 clang 命令
    clang_parts = [
        "clang", "-c",
        *TARGET_FLAGS,
        *inc_flags,
        *def_flags,
        *parsed["other_flags"],
        file_rel_str,
    ]

    return {
        "directory": str(project_root).replace("\\", "/"),
        "file": file_path.replace("\\", "/"),
        "command": " ".join(clang_parts),
    }


def detect_project_root(entries: list[dict]) -> Path:
    """
    从 compile_commands.json 条目中自动检测项目根目录。
    策略: 找到所有 file 路径和 directory 路径的最近公共祖先。
    """
    paths = []
    for entry in entries:
        if "file" in entry:
            paths.append(Path(entry["file"]))
        if "directory" in entry:
            paths.append(Path(entry["directory"]))
    return find_common_ancestor(paths)


def main():
    parser = argparse.ArgumentParser(
        description="将 EIDE 生成的 ARMCC compile_commands.json 转换为 clangd 兼容版本"
    )
    parser.add_argument(
        "input",
        type=str,
        help="EIDE 生成的 ARMCC compile_commands.json 路径",
    )
    parser.add_argument(
        "--output", "-o",
        type=str,
        default=None,
        help="输出路径 (默认: 脚本同级目录下的 compile_commands.json)",
    )
    parser.add_argument(
        "--root", "-r",
        type=str,
        default=None,
        help="项目根目录 (默认: 从输入文件自动检测)",
    )
    args = parser.parse_args()

    # 输入文件
    input_path = Path(args.input).resolve()
    if not input_path.exists():
        print(f"[ERROR] 输入文件不存在: {input_path}")
        sys.exit(1)

    # 输出路径: 默认为脚本所在目录下的 compile_commands.json
    if args.output:
        output_path = Path(args.output).resolve()
    else:
        output_path = Path(__file__).resolve().parent / "compile_commands.json"

    print(f"[convert] 输入: {input_path}")
    print(f"[convert] 输出: {output_path}")

    # 读取 ARMCC compile_commands.json
    with open(input_path, "r", encoding="utf-8") as f:
        armcc_entries = json.load(f)

    print(f"[convert] 读取到 {len(armcc_entries)} 个条目")

    # 检测项目根目录
    if args.root:
        project_root = Path(args.root).resolve()
    else:
        project_root = detect_project_root(armcc_entries)

    print(f"[convert] 项目根目录: {project_root}")

    # 转换每个条目
    clang_entries = []
    skipped = 0
    for entry in armcc_entries:
        result = convert_entry(entry, project_root)
        if result is None:
            skipped += 1
            continue
        clang_entries.append(result)

    print(f"[convert] 转换完成: {len(clang_entries)} 个 C 文件, 跳过 {skipped} 个汇编文件")

    # 写入输出
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(clang_entries, f, indent=2, ensure_ascii=False)

    print(f"[convert] 已写入: {output_path}")

    # 打印示例
    if clang_entries:
        sample = clang_entries[0]
        fname = Path(sample["file"]).name
        cmd_preview = sample["command"][:120]
        print(f"\n[示例] 文件: {fname}")
        print(f"[示例] 命令: {cmd_preview}...")


if __name__ == "__main__":
    main()
