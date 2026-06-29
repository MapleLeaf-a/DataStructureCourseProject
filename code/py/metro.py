from collections import defaultdict
import random

import requests
import re
import csv

import os

#��·��
lst = [*range(1, 19), 41, 51]

#վ��-������·�ֵ�
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
        
        # ����1���ȶ�λ locations ���飬����ȡ���е� title
        # ƥ�� "locations":[ ... ] �е�����
        locations_pattern = r'"locations":\s*\[(.*?)\]'
        locations_match = re.search(locations_pattern, text, re.DOTALL)
        
        if locations_match:
            locations_content = locations_match.group(1)
            # �� locations ��������ȡ���� title
            title_pattern = r'"title":"([^"]+)"'
            stations = re.findall(title_pattern, locations_content)
            
            for name in set(stations):  # ȥ��
                if name:
                    station_dict[name].append(i)

# ת��Ϊ��ͨ�ֵ�
station_dict = dict(station_dict)

# ��ӡ����վ�㣨��������ǰ10����
print(f"���ҵ� {len(station_dict)} ��վ��\n")

# # ��ӡ����վ
# transfer = {name: lines for name, lines in station_dict.items() if len(lines) > 1}
# print(f"����վ��{len(transfer)} ������")
# for name, lines in list(transfer.items())[:20]:  # ��ʾǰ20������վ
#     print(f"  {name}: ��· {lines}")

# ��ӡȫ��վ�㣨���Ҫ��ȫ����ȡ��ע�ͣ�
for name, lines in station_dict.items():
    print(f"{name}: ��· {lines}")



#д��վ����Ϣ

csv_file = "../../data/csv/Station.csv"

header_Station = ["id", "name", "line1", "line2", "line3", "line4", "line5", "status"]

with open(csv_file, "w", encoding="utf-8-sig", newline="") as fp:
    writer = csv.writer(fp)
    
    # д���ͷ
    writer.writerow(header_Station)
    
    # д�����ݣ���վ��������
    for i, (name, lines) in enumerate(sorted(station_dict.items()), 1):
        # ���������ݣ�id, name, Ȼ����ÿ����·������Ĳ�0
        row = [i, name] + lines + [0] * (len(header_Station) - 3 - len(lines)) + [1]
        writer.writerow(row)

# print(f"��д�� {len(station_dict)} ����¼�� {csv_file}")
# print(f"��ͷ��{header}")


# ========== 1. ����վ��-����ֵ� ==========
stationIdxDic = {
    name: idx 
    for idx, (name, _) in enumerate(sorted(station_dict.items()), 1)
}

# ========== 2. ���ɱ����� ==========
def generate_edges(station_dict, stationIdxDic, lst, data_dir="../../data/txt"):
    """�������б�����"""
    edges = []
    
    for line_id in lst:
        file_path = f"{data_dir}/metro{line_id}.txt"
        
        if not os.path.exists(file_path):
            print(f"�ļ� {file_path} �����ڣ�����")
            continue
        
        with open(file_path, "r", encoding="utf-8") as fp:
            text = fp.read()
            
            # ��ȡվ���б�
            locations_pattern = r'"locations":\s*\[(.*?)\]'
            locations_match = re.search(locations_pattern, text, re.DOTALL)
            
            if not locations_match:
                continue
                
            locations_content = locations_match.group(1)
            title_pattern = r'"title":"([^"]+)"'
            stations = re.findall(title_pattern, locations_content)
            
            # ȥ�ر���˳��
            seen = set()
            station_list = []
            for name in stations:
                if name not in seen:
                    seen.add(name)
                    station_list.append(name)
            
            if len(station_list) < 2:
                continue
            
            # ȷ����·���ƺͷ���
            if line_id == 4:
                line_name = "4����"
                dir_fwd = "��Ȧ"
                dir_bwd = "��Ȧ"
            else:
                line_name = f"{line_id}����"
                dir_fwd = f"��{station_list[-1]}"  # ���յ�վ
                dir_bwd = f"��{station_list[0]}"   # �����վ
            
            # ���ɱ�
            for j in range(len(station_list) - 1):
                u_name = station_list[j]
                v_name = station_list[j + 1]
                
                u = stationIdxDic.get(u_name)
                v = stationIdxDic.get(v_name)
                
                if u is None or v is None:
                    continue
                
                # �������ʱ��
                time_fwd = random.choice([2, 3])
                time_bwd = random.choice([2, 3])
                
                # ˫���
                edges.append([u, v, line_name, dir_fwd, time_fwd])
                edges.append([v, u, line_name, dir_bwd, time_bwd])
    
    return edges

# ========== 3. ִ�� ==========
edges = generate_edges(station_dict, stationIdxDic, lst)

# ========== 4. д��CSV ==========
csv_file = "../../data/csv/Edge.csv"
os.makedirs(os.path.dirname(csv_file), exist_ok=True)

header_Edge = ["u", "v", "line_name", "direction", "time"]

with open(csv_file, "w", encoding="utf-8-sig", newline="") as fp:
    writer = csv.writer(fp)
    writer.writerow(header_Edge)
    writer.writerows(edges)

print(f"��д�� {len(edges)} ���ߵ� {csv_file}")

# ========== 5. Ԥ�� ==========
print("\n������Ԥ����ǰ10������")
for edge in edges[:10]:
    print(f"  {edge}")