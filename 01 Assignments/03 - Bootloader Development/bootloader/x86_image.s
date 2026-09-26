.code16 # Instructs the assembler to generate 16-bit instructions, because x86 systems boot in 16-bit Real Mode for backwards compatibility
.global _start # Exposes the _start label globally so the linker can identify the program's entry point

_start:
    /* Set string source index to log device memory map */
    mov $0x500, %si         # Log device convetional memory address 0x500 into the Source Index ($si) register, where the QMEU environment mapped the logging device

print_loop:
    /* Load byte from %ds:%si into %al and increment %si */
    lodsb   # Loads a single byte from the memory address at %ds:%si into the %al register and automatically increments %si to point to the next byte                
    test %al, %al           # Performs a bitwise AND on the % al register with itself to check if the loaded byte is the null terminator (0)
    jz halt_cpu             # Jumps to the halt_cpu label if the zero flag is set, indicating the end of the log string was reached

wait_uart:
    /* Read LSR to check UART status */   
    # Replace direct immediate access:
    # inb $0x3fd, %al Incorrect (truncates to 0xfd)

    # With register-based I/O:
    mov $0x3fd, %dx # Loads the I/O port address 0x3fd (COM1 base 0x3F8 + Line Status Register offset 5) into the %dx register
    inb %dx, %al    # Reads a byte from the Lone Status Register port (in %dx) into %al to check the current UART status
    test $0x20, %al     # Chekc if THR is empty    # Tests the 0x20 bit of the LSR to see if the Transmit Holding Register (THR) is empty
    jz wait_uart            # Jumps back to wait_uart if the bit is not set, meaning the THR is full and the program must keep waiting

send_uart:
    /* Retrieve the character and output to THR */
    mov -1(%si), %al    # Retreives the current character from memory (offsetting the previous %si increment) back into %al for output    
    # Replace direct immediate access:
    # outb %al, $0x3f8 Incorrect (truncates to 0xf8)

    # With register-based I/O:
    mov $0x3f8, %dx # Loads the base COM1 port address (0x3F8), which maps to the Transmit Holding Register on write, into %dx
    outb %al, %dx         # Outputs the character byte in %al to the COM1 THR port mapped in %dx
    jmp print_loop          # Jumps back to print_loop to process and transmit the next character in the string

halt_cpu:
    hlt                     # Halts the CPU execution to stop the bootloader after the message is sent
    jmp halt_cpu            # Creates an infinite loop jumping back to hlt, ensuring the CPU stays halted if awakened by an interrupt

/* Pad remainder of boot sector and add boot signature */
.org 510 # Pads the boot sector with zeros up to byte 510 to fill out the standard 512-byte boot block format
.word 0xaa55 # Writes the 2-byte boot signature (little-endian 0xaa55) at the end to signify to the BIOS that this block is bootable
