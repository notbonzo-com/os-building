#!/bin/bash

QEMU_ARGS='-debugcon stdio -m 32 -no-reboot -no-shutdown -M smm=off'

if [ "$#" -le 1 ]; then
    echo "Usage: ./run.sh <image_type> <image>"
    exit 1
fi

case "$1" in
    "floppy")   QEMU_ARGS="${QEMU_ARGS} -drive format=raw,file=$2,if=floppy"
    ;;
    "disk")     QEMU_ARGS="${QEMU_ARGS} -drive format=raw,file=$2,if=ide"
    ;;
    *)          echo "Unknown image type $1."
                exit 2
esac


clear
qemu-system-x86_64 $QEMU_ARGS