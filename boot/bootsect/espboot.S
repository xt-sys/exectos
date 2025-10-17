/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            boot/bootsect/espboot.S
 * DESCRIPTION:     XT Boot Loader ESP boot code (FAT32)
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

.text
.code16


.global Start
Start:
    /* Jump to the real start to omit the BPB (BIOS Parameter Block) */
    jmp RealStart
    nop

/* BIOS Parameter Block */
OsName:
    .ascii "XTOS "
OsVersion:
    .ascii "1.0"
BytesPerSector:
    .word 512
SectorsPerCluster:
    .byte 2
ReservedSectors:
    .word 8
FatCopies:
    .byte 1
RootDirEntries:
    .word 1024
TotalSectors:
    .word 0
MediaType:
    .byte 0xF8
SectorsPerFat:
    .word 0
SectorsPerTrack:
    .word 17
NumberOfHeads:
    .word 0
HiddenSectors:
    .long 0
TotalBigSectors:
    .long 0x200000
BigSectorsPerFat:
    .long 0x1FE0
ExtendedFlags:
    .word 0
FsVersion:
    .word 0
RootDirStartCluster:
    .long 0
FSInfoSector:
    .word 0
BackupBootSector:
    .word 6
Reserved:
    .fill 12, 1, 0
DriveNumber:
    .byte 0x80
CurrentHead:
    .byte 0
Signature:
    .byte 0x29
SerialNumber:
    .long 0
VolumeLabel:
    .ascii "NO NAME    "
FileSystem:
    .ascii "FAT32   "

RealStart:
    /* Set segments and stack */
    cli
    cld
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %bp
    leaw -16(%bp), %sp
    sti

    /* Get drive number */
    cmpb $0xFF, DriveNumber - Start(%bp)
    jne GetDriveParameters
    movb %dl, DriveNumber - Start(%bp)

GetDriveParameters:
    /* Get drive parameters from the BIOS */
    movb DriveNumber - Start(%bp), %dl
    movb $0x08, %ah
    movb $0x00, %al
    int $0x13
    jnc GetDriveSize
    movw $0xFFFF, %cx
    movb %cl, %dh

GetDriveSize:
    /* Get drive size from the BIOS */
    movzbl %dh, %eax
    incw %ax
    movzbl %cl, %edx
    andb $0x3F, %dl
    mulw %dx
    xchgb %cl, %ch
    shrb $0x06, %ch
    incw %cx
    movzwl %cx, %ecx
    mull %ecx
    movl %eax, %edi

VerifyBiosParameterBlock:
    /* Verify the FAT32 BPB */
    cmpw $0x00, SectorsPerFat - Start(%bp)
    jne FsError
    cmpw $0x00, FsVersion - Start(%bp)
    ja FsError

ReadExtraCode:
    /* Read second VBR sector with extra boot code (1 sector starting from sector 2) */
    movl HiddenSectors - Start(%bp), %eax
    addl $0x02, %eax
    movw $0x01, %cx
    xorw %bx, %bx
    movw %bx, %es
    movw $0x7E00, %bx
    call ReadSectors
    jmp StartSectors

ReadSectors:
    /* Check for extended BIOS functions and use it only if available */
    pushw %es
    pushal
    movb $0x41, %ah
    movw $0x55AA, %bx
    movb DriveNumber - Start(%bp), %dl
    int $0x13
    jc ReadCHS
    cmpw $0xAA55, %bx
    jne ReadCHS
    testb $0x01, %cl
    jz ReadCHS

    /* Verify drive size and determine whether to use CHS or LBA */
    cmpl %edi, %eax
    jnb ReadLBA

ReadCHS:
    /* Read sectors using CHS */
    popal

CHSLoop:
    /* Read sector by sector using CHS */
    pushw %cx
    pushal
    xorl %edx, %edx
    movzwl SectorsPerTrack - Start(%bp), %ecx
    divl %ecx
    incb %dl
    movb %dl, %cl
    movl %eax, %edx
    shrl $0x10, %edx
    divw NumberOfHeads - Start(%bp)
    movb %dl, %dh
    movb DriveNumber - Start(%bp), %dl
    movb %al, %ch
    rorb $0x01, %ah
    rorb $0x01, %ah
    orb %ah, %cl
    movw $0x0201, %ax
    int $0x13
    popal
    popw %cx
    jc DiskError
    incl %eax
    movw %es, %dx
    addw $0x20, %dx
    movw %dx, %es
    loop CHSLoop
    popw %es
    ret

ReadLBA:
    /* Prepare DAP packet and read sectors using LBA */
    popal
    pushw %cx
    pushal
    pushw $0x00
    pushw $0x00
    pushl %eax
    pushw %es
    pushw %bx
    pushw %cx
    pushw $0x10
    movw %sp, %si
    movb DriveNumber - Start(%bp), %dl
    movb $0x42, %ah
    int $0x13
    jc DiskError
    addw $0x10, %sp
    popal
    popw %si
    pushw %bx
    movzwl %si, %ebx
    addl %ebx, %eax
    shll $0x05, %ebx
    movw %es, %dx
    addw %bx, %dx
    movw %dx, %es
    popw %bx
    subw %si, %cx
    jnz ReadLBA
    popw %es
    ret

DiskError:
    /* Display disk error message and reboot */
    movw $msgDiskError, %si
    call Print
    jmp Reboot

FsError:
    /* Display FS error message and reboot */
    movw $msgFsError, %si
    call Print
    jmp Reboot

Print:
    /* Simple routine to print messages */
    lodsb
    orb     %al, %al
    jz      DonePrint
    movb    $0x0E, %ah
    movw    $0x07, %bx
    int     $0x10
    jmp     Print
DonePrint:
    retw

Reboot:
    /* Display a message, wait for a key press and reboot */
    movw $msgAnyKey, %si
    call Print
    xorw %ax, %ax
    int $0x16
    int $0x19

msgAnyKey:
    .ascii "Press any key to restart\r\n"

msgDiskError:
    .ascii "Disk error\r\n"

msgFsError:
    .ascii "File system error\r\n"

/* Fill the rest of the VBR with zeros and add VBR signature at the end */
.fill (510 - (. - Start)), 1, 0
.word 0xAA55


StartSectors:
    /* Print message */
    movw $msgUnavailable, %si
    call Print

    /* Wait for key press and reboot */
    xorw %ax, %ax
    int $0x16
    int $0x19

/* Include architecture specific code */
.include ARCH_ESP_SOURCE

msgUnavailable:
    .ascii "XTLDR requires EFI-based system!\r\nPress any key to restart\r\n"

/* Fill the rest of the extra VBR with zeros */
.fill (1024 - (. - Start)), 1, 0
