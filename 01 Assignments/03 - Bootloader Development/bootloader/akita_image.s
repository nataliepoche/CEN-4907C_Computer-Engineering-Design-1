.global _start

_start:
    /* 1. CPU Initialization */
    mrs r0, cpsr
    bic r0, r0, #0xff
    orr r0, r0, #0xd3
    msr cpsr_c, r0

    /* 2. Setup Memory Addresses */
    ldr r1, =0xA3F00000     /* Log device address */
    ldr r2, =0x40100000     /* UART THR (Base) */
    ldr r3, =0x40100014     /* UART LSR */

print_loop:
    /* Load byte and post-increment log pointer */
    ldrb r4, [r1], #1       
    cmp r4, #0              
    beq halt_cpu            

wait_uart:
    /* Read LSR and check if THR is empty (0x20) */
    ldr r5, [r3]            
    tst r5, #0x20           
    beq wait_uart           

send_uart:
    /* Write character to THR */
    strb r4, [r2]           
    b print_loop            

halt_cpu:
    b halt_cpu
