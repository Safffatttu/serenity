from codecs import decode
from os import write


def createTable(encoding: str, f):
    f.write("const static Vector<u32> ")

    usellesCount = 0

    encodingVector = encoding.replace("-", "_")

    f.write(encodingVector)
    f.write("_table = {")
    for i in range(256):
        if i % 8 == 0:
            f.write("\n\t")

        inUTF8 = decode(bytes([i]), encoding=encoding, errors="replace")
        if inUTF8 == '\uFFFD':
            pass
            # print(encoding + " : ERROR" + str(i))

        if i == ord(inUTF8):
            usellesCount += 1

        f.write(hex(ord(inUTF8)))

        if i != 255:
            f.write(", ")

    f.write("\n};")
    f.write("\n\n")

    return usellesCount


fileName = "/Users/jakubberkop/Documents/serenity/Libraries/LibTextCodec/Tables.h"
l = []


with open(fileName, mode="w+") as f:
    f.write("#pragma once\n\n")
    f.write("#include <AK/Types.h>\n")
    f.write("#include <AK/Vector.h>\n\n")
    f.write("namespace TextCodec {\n\n")

    l.append(createTable("IBM866", f))
    l.append(createTable("ISO-8859-2", f))
    l.append(createTable("ISO-8859-3", f))
    l.append(createTable("ISO-8859-4", f))
    l.append(createTable("ISO-8859-5", f))
    l.append(createTable("ISO-8859-6", f))
    l.append(createTable("ISO-8859-7", f))
    l.append(createTable("ISO-8859-8", f))
    l.append(createTable("ISO-8859-10", f))
    l.append(createTable("ISO-8859-13", f))
    l.append(createTable("ISO-8859-14", f))
    l.append(createTable("ISO-8859-15", f))
    l.append(createTable("ISO-8859-16", f))

    l.append(createTable("KOI8-R", f))
    l.append(createTable("KOI8-U", f))

    l.append(createTable("macintosh", f))

    # windows-874
    l.append(createTable("cp874", f))
    l.append(createTable("windows-1250", f))
    l.append(createTable("windows-1251", f))
    l.append(createTable("windows-1252", f))
    l.append(createTable("windows-1253", f))
    l.append(createTable("windows-1254", f))
    l.append(createTable("windows-1255", f))
    l.append(createTable("windows-1256", f))
    l.append(createTable("windows-1257", f))
    l.append(createTable("windows-1258", f))
    # x-mac-cyrillic
    l.append(createTable("mac_cyrillic", f))

    f.write("}")

print(sum(l) / l.__len__() / 256)
