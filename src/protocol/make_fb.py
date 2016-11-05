#!/usr/bin/python
# Works on CPython2.7 and CPython3.5,older version may work,too.

import os
import sys

print("* Building fbs into './gen' ...")

fbsList = []

sep = os.sep

flatc_path=sys.argv[1] if len(sys.argv)==2 else ""
if flatc_path!="" and not flatc_path[-1] in ["\\","/"]: flatc_path+="/"

all_protocols=[]

for dirpath, dirs, files in os.walk("."):
    for filename in files:
        name,suffix = os.path.splitext(filename)
        if suffix == ".fbs":
            type = dirpath.replace("."+os.sep,"")
            cmd = '%sflatc -c -o "./gen/%s" "./%s/%s"'%(flatc_path,type,type,filename)
            all_protocols.append(type+name.split("-")[1])
            print(cmd)
            os.system(cmd)
            fbsList.append(dirpath + sep + filename)

print("* Generating 'protocol.h' ...")

if not os.path.exists('./gen/'): os.mkdir('./gen/')
out = open('./gen/protocol.h','w')
out.write("""// A part of NEWorld, a free game with similar game rules to Minecraft
// Automatically generated by make_fb.py
// DO NOT MODIFY!
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <raknet/MessageIdentifiers.h>
""")

for f in fbsList:
    out.write("#include \"" + f.replace(".fbs","") + "_generated.h\"\n")
out.write("\n")
out.write("enum class Identifier {Unknown = ID_USER_PACKET_ENUM, " + ", ".join(all_protocols) + ", EndIdentifier};\n")
out.write("template <class Type> inline Identifier packetType2Id() {return Identifier::Unknown;}\n")
for protocol in all_protocols:
    out.write("template <> inline Identifier packetType2Id<%s::%s>() { return Identifier::%s; }\n"%(protocol[:3],protocol[3:],protocol));
out.write("#endif\n")
out.close()

print("* Done.")