#!/usr/bin/python
# Converts an x.509 certificate into a C character array

import sys

def read_file(filename):
    f = open(filename, 'r')
    txt = f.read()
    f.close()
    return txt

def main(infile, outfile):
    certificate = read_file(infile)
    certificate_bytes = bytearray(certificate, encoding='utf8')

    f = open(outfile, 'w')

    f.write('unsigned char ' + (outfile.split('.')[0]) + '[] = {')
    f.write(",".join([("'" + (chr(char) if char != 10 else '\\n') + "'") for char in certificate_bytes]))
    f.write(", 0};")
    f.close()

if __name__ == "__main__":
    if len(sys.argv) == 2:
        infile  = sys.argv[1]
        outfile = sys.argv[1] + ".h"
        main(infile, outfile)
    elif len(sys.argv) == 3:
        infile  = sys.argv[1]
        f = infile.split('.')
        name = ""
        for i in f[2:]:
            name = name + '.' + i
        outfile = sys.argv[2] + name + ".h"
        main(infile, outfile)
    else:
        print ('Usage: %s <input file> [<output array name>]' % sys.argv[0])
