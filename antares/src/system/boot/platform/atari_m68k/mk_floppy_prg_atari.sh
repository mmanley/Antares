#!/bin/sh -x
IMG=$HOME/floppy.img
ZBEOS=antares_loader
jam -q antares.prg $ZBEOS && \
dd if=/dev/zero of=$IMG bs=1k count=1440 && \
dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 conv=notrunc && \
src/system/boot/platform/atari_m68k/fixup_tos_floppy_chksum $IMG && \
true

exit 0

dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 count=9 conv=notrunc && \
dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 count=9 conv=notrunc skip=9 seek=$((9*180)) && \
dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 count=9 conv=notrunc skip=9 seek=$((2*9)) && \
dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 count=9 conv=notrunc skip=$((2*9)) seek=$((2*2*9)) && \
dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 count=9 conv=notrunc skip=$((3*9)) seek=$((3*2*9)) && \
src/system/boot/platform/atari_m68k/fixup_tos_floppy_chksum $IMG && \
true 
#generated.m68k/cross-tools/bin/m68k-unknown-antares-ld -o antares.prg -T src/system/ldscripts/m68k/boot_prg_atari_m68k.ld generated/objects/antares/m68k/release/system/boot/boot_loader_atari_m68k && \
#zip antares.prg.zip antares.prg



#dd if=generated/objects/antares/m68k/release/system/boot/$ZBEOS of=$IMG bs=512 count=500 conv=notrunc && \


