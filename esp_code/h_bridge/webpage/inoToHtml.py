mystr = ""
with open("data.txt", "r", encoding="utf-8") as fil:
    for satir in fil:
        if not "//" in satir:
            if len(satir) >= 16:
                satir = satir.lstrip().lstrip()
                satir = satir[:-4]
                satir = satir[16:]
                satir += "\n"
            else:
                satir = "\n"
            mystr += satir
print(mystr)
with open("out.html", "w", encoding="utf-8") as fil:
    fil.writelines(mystr)