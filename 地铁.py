import requests

lst = [*range(1, 19), 41, 51]

'''
for i in lst:
    url = f"https://m.shmetro.com/interface/metromap/metromap.aspx?func=lineStations&line={i}"

    response = requests.get(url = url)

    page_txt = response.text

    with open(f"metro{i}.txt", "w", encoding = "utf-8") as fp:
        fp.write(page_txt)
'''
        
with open(f"metro{i}.txt", "r", encoding = "utf-8") as fp:
    print(fp.readlines() )   
