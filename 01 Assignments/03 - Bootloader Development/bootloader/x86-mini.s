# use 16 bit instructions, and mark the start of the text
.code16
.global _start

# reset routine
_start:
    jmp loop_init	# Jump to looping routine

multiple:
    nop                 # Do nothing - this is just a demo. :)
    jmp post_modulo

loop_init:
    mov $0, %cx		# Set limitcounter to 0
loop_body:
    cmp $0x40, %cx	# If r0 >= 64, terminate loop.
    jge post_loop
    mov %cx, %dx	# Else, let's see if this is a multiple of 8
    shr $3, %dx		# Right shift 3 (Divide by 8)
    shl $3, %dx		# Left shift 3 (Multiply by 8)
    cmp %cx, %dx	# If they are equal (r0 is a multiple of 8), do the "multiple" routine
    je multiple
post_modulo:
    inc %cx		# Increment r0
    jmp loop_body	# Jump up to the beginning of the loop again

post_loop:
    hlt

. = _start + 510		# Skip forward to byte 510
.word 0xaa55			# magic number that tells the BIOS that this device is bootable
