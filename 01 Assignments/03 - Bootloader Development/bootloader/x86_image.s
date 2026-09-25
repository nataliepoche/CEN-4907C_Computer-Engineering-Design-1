.code16
.global _start

_start:
    /* Set string source index to log device memory map */
    mov $0x500, %si         

print_loop:
    /* Load byte from %ds:%si into %al and increment %si */
    lodsb                   
    test %al, %al           
    jz halt_cpu             

wait_uart:
    /* Read LSR to check UART status */
    in $0x3fd, %al          
    test $0x20, %al         
    jz wait_uart            

send_uart:
    /* Retrieve the character and output to THR */
    mov -1(%si), %al        
    out %al, $0x3f8         
    jmp print_loop          

halt_cpu:
    hlt                     
    jmp halt_cpu            

/* Pad remainder of boot sector and add boot signature */
.org 510
.word 0xaa55