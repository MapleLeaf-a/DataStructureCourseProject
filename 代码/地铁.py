from collections import defaultdict

import requests
import re

lst = [*range(1, 19), 41, 51]

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
    with open(f"metro{i}.txt", "r", encoding="utf-8") as fp:
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