#!/usr/bin/env python3
"""Convert a flat binary into Verilog ram[] initialization lines."""

import struct
import sys

PROG_START_WORD = 2000

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <program.bin>", file=sys.stderr)
        sys.exit(1)

    data = open(sys.argv[1], 'rb').read()

    # Pad to word boundary
    while len(data) % 4:
        data += b'\x00'

    words = struct.unpack('<' + 'I' * (len(data) // 4), data)
    start = PROG_START_WORD

    for i, w in enumerate(words):
        print(f"\tram[{start+i}] = 32'h{w:08X};")

if __name__ == '__main__':
    main()

