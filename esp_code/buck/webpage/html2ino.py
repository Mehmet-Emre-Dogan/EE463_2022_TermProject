mystr = ""
with open("data.html", "r", encoding="utf-8") as fil:
    for line in fil:
            if len(line) >=2:
                line = line.strip("\n")
                line = f'client.println("{line}");\n'
            else:
                line = 'client.println();\n'

            mystr += line
print(mystr)
with open("out.txt", "w", encoding="utf-8") as fil:
    fil.writelines(mystr)