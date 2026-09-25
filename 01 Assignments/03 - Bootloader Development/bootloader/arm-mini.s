.global _start

@trap table vector (instruction executed on trap)
_start:
_trap_table:
    @ Each trap instruction loads the address of its trap handler.
    ldr pc, $handlers + 0	@ Reset
    ldr pc, $handlers + 4	@ Supervisor Call
    ldr pc, $handlers + 8	@ Supervisor Call
    ldr pc, $handlers + 12	@ Prefetch Abort
    ldr pc, $handlers + 16	@ Data Abort
    nop				@ Reserved; no-op.
    ldr pc, $handlers + 20	@ Regular Interrupt
    ldr pc, $handlers + 24	@ Fast Interrupt

@pointers to trap handler routines
handlers:
    .int reset, reset, reset, reset, reset, reset, reset

@reset routine
reset:
    mrs r0, cpsr	@ Get status register
    bic r0, r0, #0xff	@ Clear status register bits 0-7
    orr r0, r0, #0xd3	@ Mask with 0x11010011
    msr cpsr_fc, r0	@ Load value into status register
    b loop_init		@ Jump to looping routine

multiple:
    nop                 @ Do nothing - this is just a demo. :)
    b post_modulo

loop_init:
    mov r0, #0		@ Set limitcounter to 64
loop_body:
    cmp r0, #0x40	@ If r0 >= 64, terminate loop.
    bge post_loop
    mov r1, r0		@ Else, let's see if this is a multiple of 8
    lsr r1, #3		@ Right shift 3 (Divide by 8)
    lsl r1, #3		@ Left shift 3 (Multiply by 8)
    cmp r0, r1          @ If they are equal (r0 is a multiple of 8), do the "multiple" routine
    beq multiple
post_modulo:
    add r0, r0, #1	@ Increment r0
    b loop_body		@ Jump up to the beginning of the loop again

post_loop:
    b post_loop
