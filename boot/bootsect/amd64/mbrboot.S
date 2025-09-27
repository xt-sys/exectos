/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            boot/bootsect/amd64/mbrboot.S
 * DESCRIPTION:     XT Boot Loader MBR boot code
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

.text
.code16


.global Start
Start:
    /* Set segments and stack */
    cli
    cld
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %bp
    movw %bp, %sp
    sti

    /* Relocate MBR to 1FE0:7C00 */
    movw $0x1FE0, %ax
    movw %ax, %es
    movw %bp, %si
    movw %bp, %di
    movw $256, %cx
    rep movsw

    /* Jump to the relocated MBR code */
    jmp $0x1FE0, $RealStart

RealStart:
    /* Set segments */
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    /* Print welcome message */
    call Print
    .asciz "Starting XTOS boot loader...\r\n"

    /* Get BIOS boot drive and partition table offset */
    lea 0x1BE(%bp), %si
    movb %dl, .BootDrive
    xorw %cx, %cx

FindActivePartition:
    /* Look for active partition */
    movb (%si), %al
    cmpb $0x80, %al
    je PartitionFound
    addw $16, %si
    incw %cx
    cmpw $4, %cx
    jne FindActivePartition
    jmp PartitionNotFound

PartitionFound:
    /* Save LBA start */
    movl 8(%si), %eax
    movl %eax, .LbaStart

    /* Prepare Disk Address Packet (DAP) */
    lea .Dap, %si
    movb $0x10, 0(%si)
    movb $0x00, 1(%si)
    movw $1, 2(%si)
    movw $0x7C00, 4(%si)
    movw $0x0000, 6(%si)
    movl .LbaStart, %eax
    movl %eax, 8(%si)

    /* Read Volume Boot Record (VBR) */
    movb $0x42, %ah
    movb .BootDrive, %dl
    int $0x13
    jc VbrReadFail

    /* Verify VBR signature */
    cmpw $0xAA55, (0x7C00 + 0x01FE)
    jne InvalidSignature

    /* Jump to the VBR code */
    jmp $0x0000, $0x7C00

InvalidSignature:
    /* Invalid signature error */
    call Print
    .asciz "Invalid partition signature!"
    jmp HaltSystem

PartitionNotFound:
    /* Active partition not found error */
    call Print
    .asciz "Bootable partition not found!"
    jmp HaltSystem

VbrReadFail:
    /* VBR read failed error */
    call Print
    .asciz "VBR read failed!"
    jmp HaltSystem

HaltSystem:
    /* Disable interrupts and stop the CPU */
    cli
    hlt
    jmp HaltSystem

PutChar:
    /* Simple routine to print error messages */
    xor %bx, %bx
    movb $0x0E, %ah
    int $0x10
Print:
    pop %si
    lodsb
    push %si
    cmp $0, %al
    jne PutChar
    ret

.BootDrive:
    /* Storage for the boot drive number */
    .byte 0

.Dap:
    /* Storage for the Disk Address Packet (DAP) */
    .fill 16, 1, 0

.LbaStart:
    /* Storage for the LBA start */
    .long 0

/* Fill the rest of the MBR with zeros and add MBR signature at the end */
.fill (510 - (. - Start)), 1, 0
.word 0xAA55
