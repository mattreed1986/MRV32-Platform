#! /bin/bash

rm -f *.o *.bin *.elf dump.txt decdump.txt

riscv64-unknown-elf-gcc -march=rv32im -mabi=ilp32 -ffreestanding -nostdlib -T program.ld -c TinyBasicLike.c target_riscv.c crt0.S

riscv64-unknown-elf-ld -T program.ld -m elf32lriscv --no-relax -nostdlib crt0.o target_riscv.o TinyBasicLike.o -o tinybasic.elf

riscv64-unknown-elf-objcopy -O binary tinybasic.elf tinybasic.bin

riscv64-unknown-elf-objdump -d tinybasic.elf > dump.txt

python addr2pc4.py dump.txt > decdump.txt

python bin2verilog.py tinybasic.bin
