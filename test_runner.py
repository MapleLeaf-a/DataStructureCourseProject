"""
自动化测试脚本 - 测试上海地铁路径规划系统
运行方式: python test_runner.py
"""
import subprocess
import os
import time
import sys

EXE_PATH = r"x64\Debug\Project-root.exe"
WORK_DIR = r"c:\Users\MapleLeaf\Desktop\数据结构课设\1"

def run_test(inputs: list[str], timeout: float = 10.0) -> str:
    """运行程序并发送输入，返回输出"""
    input_str = "\n".join(inputs) + "\n"
    try:
        proc = subprocess.run(
            [EXE_PATH],
            cwd=WORK_DIR,
            input=input_str,
            capture_output=True,
            text=True,
            timeout=timeout,
            encoding='utf-8',
            errors='replace'
        )
        return proc.stdout + proc.stderr
    except subprocess.TimeoutExpired:
        return "[TIMEOUT] 程序超时"
    except Exception as e:
        return f"[ERROR] {e}"

def check_output(output: str, keywords: list[str], anti_keywords: list[str] = None) -> bool:
    """检查输出是否包含/不包含关键词"""
    for kw in keywords:
        if kw not in output:
            return False
    if anti_keywords:
        for kw in anti_keywords:
            if kw in output:
                return False
    return True

results = {}

# ==================== M1 测试 ====================
print("=" * 60)
print("M1 - 用户输入与菜单交互")
print("=" * 60)

# M1-1: 非法菜单输入测试
print("\n--- M1-1: 非法菜单输入 ---")

# M1-1-001: 输入 abc
out = run_test(["abc", "4"])
results["M1-1-001"] = {
    "input": "abc",
    "passed": check_output(out, ["非法编号", "请输入正确选项"]),
    "output_snippet": out[:500]
}
print(f"M1-1-001 (abc): {'PASS' if results['M1-1-001']['passed'] else 'FAIL'}")
print(f"  Output: {out[:200]}")

# M1-1-002: 输入浮点数 1.2
out = run_test(["1.2", "4"])
results["M1-1-002"] = {
    "input": "1.2",
    "passed": check_output(out, ["非法编号", "请输入正确选项"]),
    "output_snippet": out[:500]
}
print(f"M1-1-002 (1.2): {'PASS' if results['M1-1-002']['passed'] else 'FAIL'}")

# M1-1-003: 输入 10 (超过上限)
out = run_test(["10", "4"])
results["M1-1-003"] = {
    "input": "10",
    "passed": check_output(out, ["非法编号", "请输入正确选项"]),
    "output_snippet": out[:500]
}
print(f"M1-1-003 (10): {'PASS' if results['M1-1-003']['passed'] else 'FAIL'}")

# M1-1-004: 输入负数 -1
out = run_test(["-1", "4"])
results["M1-1-004"] = {
    "input": "-1",
    "passed": check_output(out, ["非法编号", "请输入正确选项"]),
    "output_snippet": out[:500]
}
print(f"M1-1-004 (-1): {'PASS' if results['M1-1-004']['passed'] else 'FAIL'}")

# M1-2: 主菜单跳转测试
print("\n--- M1-2: 主菜单跳转 ---")

# M1-2-001: 输入 1 → 跳转站点管理
out = run_test(["1", "8", "4"])
results["M1-2-001"] = {
    "input": "1",
    "passed": check_output(out, ["线路站点信息/运营状态管理"]),
    "output_snippet": out[:500]
}
print(f"M1-2-001 (选项1): {'PASS' if results['M1-2-001']['passed'] else 'FAIL'}")

# M1-2-002: 输入 2 → 跳转最短时间
out = run_test(["2", "3", "4"])
results["M1-2-002"] = {
    "input": "2",
    "passed": check_output(out, ["最短时间线路路径规划"]),
    "output_snippet": out[:500]
}
print(f"M1-2-002 (选项2): {'PASS' if results['M1-2-002']['passed'] else 'FAIL'}")

# M1-2-003: 输入 3 → 跳转最少换乘
out = run_test(["3", "3", "4"])
results["M1-2-003"] = {
    "input": "3",
    "passed": check_output(out, ["最少换乘次数路线路径规划"]),
    "output_snippet": out[:500]
}
print(f"M1-2-003 (选项3): {'PASS' if results['M1-2-003']['passed'] else 'FAIL'}")

# M1-2-004: 输入 4 → 退出
out = run_test(["4"])
results["M1-2-004"] = {
    "input": "4",
    "passed": check_output(out, ["已退出系统"]),
    "output_snippet": out[:500]
}
print(f"M1-2-004 (选项4): {'PASS' if results['M1-2-004']['passed'] else 'FAIL'}")

# M1-2-005: 非法输入（与M1-1相同）
results["M1-2-005"] = results["M1-1-001"]  # 复用 abc 测试结果
print(f"M1-2-005 (非法): {'PASS' if results['M1-2-005']['passed'] else 'FAIL'}")

# M1-3: 最短时间二级菜单
print("\n--- M1-3: 最短时间二级菜单 ---")

# M1-3-001: 二级菜单选项1
out = run_test(["2", "1", "人民广场", "1", "徐家汇", "1", "3", "4"])
results["M1-3-001"] = {
    "input": "1 (二级)",
    "passed": check_output(out, ["请输入起始站关键词"]),
    "output_snippet": out[:500]
}
print(f"M1-3-001 (选项1): {'PASS' if results['M1-3-001']['passed'] else 'FAIL'}")

# M1-3-002: 二级菜单选项2
out = run_test(["2", "2", "人民广场", "1", "徐家汇", "1", "3", "4"])
results["M1-3-002"] = {
    "input": "2 (二级)",
    "passed": check_output(out, ["请输入起始站关键词"]),
    "output_snippet": out[:500]
}
print(f"M1-3-002 (选项2): {'PASS' if results['M1-3-002']['passed'] else 'FAIL'}")

# M1-3-003: 二级菜单选项3 返回上级
out = run_test(["2", "3", "4"])
results["M1-3-003"] = {
    "input": "3 (返回)",
    "passed": check_output(out, ["地铁路径规划系统"]),
    "output_snippet": out[:500]
}
print(f"M1-3-003 (选项3返回): {'PASS' if results['M1-3-003']['passed'] else 'FAIL'}")

# M1-3-004: 非法输入
out = run_test(["2", "abc", "3", "4"])
results["M1-3-004"] = {
    "input": "非法",
    "passed": check_output(out, ["非法编号"]),
    "output_snippet": out[:500]
}
print(f"M1-3-004 (非法): {'PASS' if results['M1-3-004']['passed'] else 'FAIL'}")

# M1-4: 最少换乘二级菜单
print("\n--- M1-4: 最少换乘二级菜单 ---")

# M1-4-001: 二级菜单选项1
out = run_test(["3", "1", "人民广场", "1", "徐家汇", "1", "3", "4"])
results["M1-4-001"] = {
    "input": "1 (二级)",
    "passed": check_output(out, ["请输入起始站关键词"]),
    "output_snippet": out[:500]
}
print(f"M1-4-001 (选项1): {'PASS' if results['M1-4-001']['passed'] else 'FAIL'}")

# M1-4-002: 二级菜单选项2
out = run_test(["3", "2", "人民广场", "1", "徐家汇", "1", "3", "4"])
results["M1-4-002"] = {
    "input": "2 (二级)",
    "passed": check_output(out, ["请输入起始站关键词"]),
    "output_snippet": out[:500]
}
print(f"M1-4-002 (选项2): {'PASS' if results['M1-4-002']['passed'] else 'FAIL'}")

# M1-4-003: 二级菜单选项3 返回上级
out = run_test(["3", "3", "4"])
results["M1-4-003"] = {
    "input": "3 (返回)",
    "passed": check_output(out, ["地铁路径规划系统"]),
    "output_snippet": out[:500]
}
print(f"M1-4-003 (选项3返回): {'PASS' if results['M1-4-003']['passed'] else 'FAIL'}")

# M1-4-004: 非法输入
out = run_test(["3", "abc", "3", "4"])
results["M1-4-004"] = {
    "input": "非法",
    "passed": check_output(out, ["非法编号"]),
    "output_snippet": out[:500]
}
print(f"M1-4-004 (非法): {'PASS' if results['M1-4-004']['passed'] else 'FAIL'}")

# ==================== 汇总 ====================
print("\n" + "=" * 60)
print("测试结果汇总")
print("=" * 60)
passed = sum(1 for r in results.values() if r["passed"])
total = len(results)
print(f"通过: {passed}/{total} ({100*passed/total:.1f}%)")

for key, val in results.items():
    status = "✓ PASS" if val["passed"] else "✗ FAIL"
    print(f"  {key}: {status}")
