all: i386_image.bin akita_image.bin # Sets the default target to build both the x86 and arm bootloader binary images

# x86 Build Process
i386_image.bin: x86_image.s
#  	as -march=i386 x86_image.s -o x86.o
	as --32 -march=i386 x86_image.s -o x86.o # Uses GNu assember to generate a 32-bit object file targeting oldest common instruction set (i386)
# 	ld -Ttext 0x7c00 --oformat binary x86.o -o i386_image.bin
	ld -m elf_i386 -Ttext 0x7c00 --oformat binary x86.o -o i386_image.bin # Links the x86 object file into a raw binary format with the text segment starting at 0x7c00, standard address where BIOS loads the boot block

# ARM Build Process
akita_image.bin: akita_image.s
	arm-none-eabi-as -march=armv4t akita_image.s -o arm.o # Cross-compiles the ARM source code into an object file targeting the ARMv4t architecture to maintain backward compatibility
	arm-none-eabi-ld -Ttext 0x0 arm.o -o arm.elf # Links the ARM object file into an ELF executable, setting the text section address to 0x0 because execution begins at the start of the trap table on this architecture
	arm-none-eabi-objcopy -O binary --only-section=.text arm.elf akita_image.bin # Extracts only the text section from the ELF file to create the final raw binary ARM bootloader image, avoiding inclusion of unnecessary ELF headers

KEEP_BIN := lemon_log.bin
ALL_BINS := $(wildcard *.bin)
BINS_TO_REMOVE := $(filter-out $(KEEP_BIN), $(ALL_BINS))

clean:
	rm -f *.o *.elf $(BINS_TO_REMOVE)