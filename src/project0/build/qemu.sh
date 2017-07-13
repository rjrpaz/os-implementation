#!/bin/sh
modprobe kqemu 2>/dev/null
qemu -fda fd.img -m 4
