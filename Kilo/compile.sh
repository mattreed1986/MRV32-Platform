#/bin/bash

rm -f *.elf *.o *.bin dump.txt decdump.txt

riscv64-unknown-elf-gcc -c -Os -ffreestanding -nostdinc -I. \
    -march=rv32im -mabi=ilp32  minilib.c kilo_mini.c crt0.s

riscv64-unknown-elf-ld -T program.ld -m elf32lriscv --no-relax -nostdlib -o kilo.elf \
    crt0.o minilib.o kilo_mini.o

riscv64-unknown-elf-objcopy -O binary kilo.elf kilo.bin
python bin2verilog.py kilo.bin

riscv64-unknown-elf-objdump -d -t kilo.elf > dump.txt
python addr2pc4.py dump.txt > decdump.txt
