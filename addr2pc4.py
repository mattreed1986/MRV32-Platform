#!/usr/bin/env python3
"""Convert objdump hex addresses to decimal PC/4 indexes."""

import re
import sys

def convert(infile):
    for line in infile:
        # Match label lines like "00000848 <_start>:"
        m = re.match(r'^([0-9a-fA-F]+)\s+(<.+>:)', line)
        if m:
            addr = int(m.group(1), 16)
            print(f"{addr // 4:>8d} {m.group(2)}")
            continue

        # Match instruction lines like " 848:  00002137  lui x2,0x2"
        m = re.match(r'^\s*([0-9a-fA-F]+):\s+(.*)', line)
        if m:
            addr = int(m.group(1), 16)
            print(f"{addr // 4:>8d}:\t{m.group(2)}")
            continue

        # Pass through anything else (blank lines, etc.)
        print(line, end='')

if __name__ == '__main__':
    if len(sys.argv) > 1:
        with open(sys.argv[1]) as f:
            convert(f)
    else:
        convert(sys.stdin)
