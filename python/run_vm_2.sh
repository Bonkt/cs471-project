#!/bin/bash

/mnt/sdc/cs471_tracability/qemu/build/qemu-system-aarch64 -smp 2 \
    -M virt,gic-version=max,virtualization=off,secure=off \
    -cpu max,pauth=off -m 64G \
    -boot menu=on \
    -bios ./QEMU_EFI.fd \
    -drive if=virtio,file=./root.qcow2,format=qcow2 \
    -nic user,hostfwd=tcp::3333-:22 \
    -rtc clock=vm \
    -icount shift=0,align=off,sleep=off,q=1000 \
    -loadvm base \
    -nographic -no-reboot \
    -plugins libexeclog -d plugin -D trace.log \
    workload

