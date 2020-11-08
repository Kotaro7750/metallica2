UEFI_ENTRYPOINT = efi_main
CC = x86_64-w64-mingw32-gcc
CCFLAGS = -Wall -Wextra -e $(UEFI_ENTRYPOINT) -nostdinc -nostdlib -fno-builtin -Wl,--subsystem,10

QEMU = qemu-system-x86_64
QEMUFLAGS = -m 4G -bios OVMF.fd -hda fat:rw:fs

SRC =  common.c efi.c fs.c fb.c mem.c main.c

main.efi: $(SRC)
	$(CC) $(CCFLAGS) -o $@ $^

run: main.efi
	cp boot.conf fs/boot.conf
	cp main.efi fs/EFI/BOOT/BOOTX64.EFI
	$(QEMU) $(QEMUFLAGS)

clean:
	rm -f main.efi fs/EFI/BOOT/BOOTX64.EFI

.PHONY: clean
