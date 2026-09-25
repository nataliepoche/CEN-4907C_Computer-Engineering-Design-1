all: i386_image.bin akita_image.bin

# x86 Build Process
i386_image.bin: x86_image.s
	as -march=i386 x86_image.s -o x86.o
	ld -Ttext 0x7c00 --oformat binary x86.o -o i386_image.bin

# ARM Build Process
akita_image.bin: akita_image.s
	arm-none-eabi-as -march=armv4t akita_image.s -o arm.o
	arm-none-eabi-ld -Ttext 0x0 arm.o -o arm.elf
	arm-none-eabi-objcopy -O binary --only-section=.text arm.elf akita_image.bin

clean:
	rm -f *.o *.elf *.bin