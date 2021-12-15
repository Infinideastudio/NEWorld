#!/usr/bin/env python
from pathlib import Path

def bin2h(filein, fileout):
    with open(filein, "rb") as fd:
        data = bytearray(fd.read())

    with open(fileout, "w") as fo:
        fo.write("/* %s, %d bytes */\n" % (filein, len(data)))
        fo.write("const uint8_t %s[] =\n{\n" % filein.name.replace(".","_").replace(" ","_"))
        line = "" 

        for i in data:
            num = "%d" % i + ","
            if len(line) + len(num) < 100:
                line += num
            else:
                fo.write("    " + line + "\n")
                line = num

        fo.write("    " + line[:-1] + "\n")
        fo.write("};\n")

def main():
    targets = list(Path(".").glob("External/Neosis/Src/Packages/App/Theme/Data/Theme/*.xaml"))
    targets.extend(Path(".").glob("External/Neosis/Src/Packages/App/Theme/Data/Theme/Fonts/*.otf"))
    for p in targets:
        tarp = str(p)+".bin.h"
        print(p, 'to' , tarp)
        bin2h(p, tarp)
if __name__ == "__main__":
    main()
