import subprocess
exe = r'x64\Debug\Project-root.exe'
wd = r'c:\Users\MapleLeaf\Desktop\数据结构课设\1'

def test(name, inputs, expect):
    try:
        p = subprocess.run([exe], cwd=wd, input='\n'.join(inputs)+'\n',
                          capture_output=True, text=True, timeout=15,
                          encoding='utf-8', errors='replace')
        out = p.stdout + p.stderr
        ok = all(k in out for k in expect)
        status = "PASS" if ok else "FAIL"
        print(f'  {name}: {status}')
        if not ok:
            print(f'    Expected: {expect}')
            print(f'    Got: {out[-400:]}')
    except Exception as e:
        print(f'  {name}: ERROR - {e}')

print('=== Regression Tests ===')

print('--- Fix 1: Float input rejection ---')
test('M1-1-002 float 1.2', ['1.2', '4'], ['非法编号'])

print('--- Fix 3: Restore confirmation ---')
test('M2-4 Y/N confirm', ['1', '4', 'N', '8', '4'], ['取消恢复'])

print('--- Fix 4: Invalid line number ---')
test('M2-5 invalid line', ['1', '5', '99', '8', '4'], ['线路编号无效'])

print('--- Fix 5: Same station check ---')
test('M3 same start/end', ['2', '1', '人民广场', '1', '人民广场', '2', '3', '4'], ['相同'])

print('--- Fix 5: Closed start station ---')
test('M3 closed start', ['1', '2', '莘庄', '1', '关闭', 'exit', '8',
      '2', '1', '莘庄', '2', '人民广场', '1', '3', '4'], ['已关闭'])

print('--- Fix 6: Normal path still works ---')
test('M3 normal path', ['2', '1', '人民广场', '1', '徐家汇', '1', '3', '4'], ['总耗时'])

print('Done!')
