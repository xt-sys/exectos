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

    /* Print message */
    movw $msgUnavailable, %si
    call Print

    /* Wait for key press and reboot */
    xorw %ax, %ax
    int $0x16
    int $0x19

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

msgUnavailable:
    .ascii "XTLDR requires EFI-based system!\r\nPress any key to restart\r\n"

/* Fill the rest of the VBR with zeros and add VBR signature at the end */
.fill (510 - (. - Start)), 1, 0
.word 0xAA55
