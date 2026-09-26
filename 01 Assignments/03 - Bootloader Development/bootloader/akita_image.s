.global _start // Exposes the _start label globally so the linker recognizes it as the entry point

_start:
    /* 1. CPU Initialization */
    mrs r0, cpsr // Moves the Current Program Status Register (CPSR) into register r0 to read the CPU's current state
    bic r0, r0, #0xff // Uses Bit Clear to clear out the lowest 8 bits (control byte) of the CPSR stored in r0
    orr r0, r0, #0xd3 // Uses Bitwise OR to set supervisor mode, disable THUMB instructions, and mask IRQs by writing b11010011 (0xd3)
    msr cpsr_c, r0 // Moves the modified control byte back into the CPSR_c to finalize CPU initialization

    /* 2. Setup Memory Addresses */
    ldr r1, =0xA3F00000     /* Log device address */ // Loads the memory-mapped address of the Akita log device into r1
    ldr r2, =0x40100000     /* UART THR (Base) */ // Loads the base memory-mapped address for the Akita UART Transmit Holding Register into r2
    ldr r3, =0x40100014     /* UART LSR */ // Loads the memory-mapped address of the UART Line Status Register (base 0x40100000 + 5 * 4 byte padding) into r3

print_loop:
    /* Load byte and post-increment log pointer */
    ldrb r4, [r1], #1    // Loads a single byte from the log memory address in r1 into r4, then increments the pointer address in r1 by 1   
    cmp r4, #0              // Compares the loaded byte in r4 to 0 to check if it is the null terminator marking the end of the string
    beq halt_cpu            // Branches to halt_cpu if the comparison is equal, exiting the print loop

wait_uart:
    /* Read LSR and check if THR is empty (0x20) */
    ldr r5, [r3]       // Loads the 32-bit Line Status Register value from memory into r5     
    tst r5, #0x20           // Tests the 0x20 bit of the LSR to verify if the Transmit Holding Register is empty and ready for data
    beq wait_uart         // Branches back to wait_uart if the bit is not set, creating a polling loop until the THR is empty  

send_uart:
    /* Write character to THR */
    strb r4, [r2]       // Stores the character byte from r4 into the UART Transmit Holding Register memory address held in r2    
    b print_loop     // Branches back to print_loop to read and process the next byte in the log       

halt_cpu:
    b halt_cpu // Branches infinitely to itself, permanently halting further execution
