from collections import defaultdict
import random

import requests
import re
import csv

import os

#线路号
lst = [*range(1, 19), 41, 51]

#站点-经过线路字典
station_dict = defaultdict(list)

'''
for i in lst:
    url = f"https://m.shmetro.com/interface/metromap/metromap.aspx?func=lineStations&line={i}"

    response = requests.get(url = url)

    page_txt = response.text

    with open(f"metro{i}.txt", "w", encoding = "utf-8") as fp:
        fp.write(page_txt)
'''

for i in lst:
    file_path = f"../../data/txt/metro{i}.txt"
    with open(file_path, "r", encoding="utf-8") as fp:
        text = fp.read()
        
        # 方法1：先定位 locations 数组，再提取其中的 title
        # 匹配 "locations":[ ... ] 中的内容
        locations_pattern = r'"locations":\s*\[(.*?)\]'
        locations_match = re.search(locations_pattern, text, re.DOTALL)
        
        if locations_match:
            locations_content = locations_match.group(1)
            # 从 locations 内容中提取所有 title
            title_pattern = r'"title":"([^"]+)"'
            stations = re.findall(title_pattern, locations_content)
            
            for name in set(stations):  # 去重
                if name:
                    station_dict[name].append(i)

# 转换为普通字典
station_dict = dict(station_dict)

# 打印所有站点（不再限制前10个）
print(f"共找到 {len(station_dict)} 个站点\n")

# # 打印换乘站
# transfer = {name: lines for name, lines in station_dict.items() if len(lines) > 1}
# print(f"换乘站（{len(transfer)} 个）：")
# for name, lines in list(transfer.items())[:20]:  # 显示前20个换乘站
#     print(f"  {name}: 线路 {lines}")

# 打印全部站点（如果要看全部，取消注释）
for name, lines in station_dict.items():
    print(f"{name}: 线路 {lines}")



#写入站点信息

csv_file = "../../data/csv/Station.csv"

header_Station = ["id", "name", "line1", "line2", "line3", "line4", "line5", "status"]

with open(csv_file, "w", encoding="ANSI", newline="") as fp:
    writer = csv.writer(fp)
    
    # 写入表头
    writer.writerow(header_Station)
    
    # 写入数据（按站点名排序）
    for i, (name, lines) in enumerate(sorted(station_dict.items()), 1):
        # 构建行数据：id, name, 然后是每条线路，不足的补0
        row = [i, name] + lines + [0] * (len(header_Station) - 3 - len(lines)) + [1]
        writer.writerow(row)

# print(f"已写入 {len(station_dict)} 条记录到 {csv_file}")
# print(f"表头：{header}")


# ========== 1. 创建站点-编号字典 ==========
stationIdxDic = {
    name: idx 
    for idx, (name, _) in enumerate(sorted(station_dict.items()), 1)
}

# ========== 2. 生成边数据 ==========
def generate_edges(station_dict, stationIdxDic, lst, data_dir="../../data/txt"):
    """生成所有边数据"""
    edges = []
    
    for line_id in lst:
        file_path = f"{data_dir}/metro{line_id}.txt"
        
        if not os.path.exists(file_path):
            print(f"文件 {file_path} 不存在，跳过")
            continue
        
        with open(file_path, "r", encoding="utf-8") as fp:
            text = fp.read()
            
            # 提取站点列表
            locations_pattern = r'"locations":\s*\[(.*?)\]'
            locations_match = re.search(locations_pattern, text, re.DOTALL)
            
            if not locations_match:
                continue
                
            locations_content = locations_match.group(1)
            title_pattern = r'"title":"([^"]+)"'
            stations = re.findall(title_pattern, locations_content)
            
            # 去重保持顺序
            seen = set()
            station_list = []
            for name in stations:
                if name not in seen:
                    seen.add(name)
                    station_list.append(name)
            
            if len(station_list) < 2:
                continue
            
            # 确定线路名称和方向
            if line_id == 4:
                line_name = "4号线"
                dir_fwd = "内圈"
                dir_bwd = "外圈"
            else:
                line_name = f"{line_id}号线"
                dir_fwd = f"往{station_list[-1]}"  # 往终点站
                dir_bwd = f"往{station_list[0]}"   # 往起点站
            
            # 生成边
            for j in range(len(station_list) - 1):
                u_name = station_list[j]
                v_name = station_list[j + 1]
                
                u = stationIdxDic.get(u_name)
                v = stationIdxDic.get(v_name)
                
                if u is None or v is None:
                    continue
                
                # 随机生成时间
                time_fwd = random.choice([2, 3])
                time_bwd = random.choice([2, 3])
                
                # 双向边
                edges.append([u, v, line_name, dir_fwd, time_fwd])
                edges.append([v, u, line_name, dir_bwd, time_bwd])
    
    return edges

# ========== 3. 执行 ==========
edges = generate_edges(station_dict, stationIdxDic, lst)

# ========== 4. 写入CSV ==========
csv_file = "../../data/csv/Edge.csv"
os.makedirs(os.path.dirname(csv_file), exist_ok=True)

header_Edge = ["u", "v", "line_name", "direction", "time"]

with open(csv_file, "w", encoding="gbk", newline="") as fp:
    writer = csv.writer(fp)
    writer.writerow(header_Edge)
    writer.writerows(edges)

print(f"已写入 {len(edges)} 条边到 {csv_file}")

# ========== 5. 预览 ==========
print("\n边数据预览（前10条）：")
for edge in edges[:10]:
    print(f"  {edge}")