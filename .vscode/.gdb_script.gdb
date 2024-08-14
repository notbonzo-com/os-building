    symbol-file /home/notbonzo/code/os-building/build/i686_debug/kernel/kernel.elf
    set disassembly-flavor intel
    target remote | qemu-system-i386 -S -gdb stdio -m 32 -no-reboot -no-shutdown -M smm=off -drive format=raw,file=/home/notbonzo/code/os-building/build/i686_debug/image.img,if=ide
