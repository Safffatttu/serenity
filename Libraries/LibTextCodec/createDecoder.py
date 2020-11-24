from codecs import decode
from os import write


def createTable(encoding: str, f):
    f.write("const static Vector<u32> ")

    encoding = encoding.replace("-", "_")

    f.write(encoding)
    f.write("_table = {")
    for i in range(256):
        if i % 8 == 0:
            f.write("\n\t")

        inUTF8 = decode(bytes([i]), encoding=encoding, errors="replace")
        if inUTF8 == '\uFFFD':
            print(encoding + " : ERROR" + str(i))

        f.write(hex(ord(inUTF8)))

        if i != 255:
            f.write(", ")

    f.write("\n};")
    f.write("\n\n")


fileName = "/Users/jakubberkop/Documents/serenity/Libraries/LibTextCodec/tables"

with open(fileName, mode="w+") as f:
    createTable("IBM866", f)
    createTable("ISO-8859-2", f)
    createTable("ISO-8859-3", f)
    createTable("ISO-8859-4", f)
    createTable("ISO-8859-5", f)
    createTable("ISO-8859-6", f)
    createTable("ISO-8859-7", f)
    createTable("ISO-8859-8", f)
