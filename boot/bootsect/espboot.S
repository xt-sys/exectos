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
    /* Read second VBR sector with extra boot code (3 sectors starting from sector 2) */
    movl HiddenSectors - Start(%bp), %eax
    addl $0x02, %eax
    movw $0x03, %cx
    xorw %bx, %bx
    movw %bx, %es
    movw $0x7E00, %bx
    call ReadSectors
    jmp StartExtraCode

ReadSectors:
    /* Check for extended BIOS functions and use it only if available */
    pushw %es
    pushal
    movb $0x41, %ah
    movw $0x55AA, %bx
    movb DriveNumber - Start(%bp), %dl
    int $0x13
    jc .ReadCHS
    cmpw $0xAA55, %bx
    jne .ReadCHS
    testb $0x01, %cl
    jz .ReadCHS

    /* Verify drive size and determine whether to use CHS or LBA */
    cmpl %edi, %eax
    jnb .ReadLBA

.ReadCHS:
    /* Read sectors using CHS */
    popal

.CHSLoop:
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
    loop .CHSLoop
    popw %es
    ret

.ReadLBA:
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
    jnz .ReadLBA
    popw %es
    ret

DiskError:
    /* Display disk error message and reboot */
    movw $.MsgDiskError, %si
    call Print
    jmp Reboot

FsError:
    /* Display FS error message and reboot */
    movw $.MsgFsError, %si
    call Print
    jmp Reboot

Print:
    /* Simple routine to print messages */
    lodsb
    orb     %al, %al
    jz      .DonePrint
    movb    $0x0E, %ah
    movw    $0x07, %bx
    int     $0x10
    jmp     Print
.DonePrint:
    retw

Reboot:
    /* Display a message, wait for a key press and reboot */
    movw $.MsgAnyKey, %si
    call Print
    xorw %ax, %ax
    int $0x16
    int $0x19

.MsgAnyKey:
    .ascii "Press any key to restart...\r\n\0"

.MsgDiskError:
    .ascii "Disk error!\r\n\0"

.MsgFsError:
    .ascii "File system error!\r\n\0"

/* Fill the rest of the VBR with zeros and add VBR signature at the end */
.fill (510 - (. - Start)), 1, 0
.word 0xAA55


StartExtraCode:
    /* Load XTLDR file from disk */
    call LoadStage2

    /* Enable A20 gate */
    call EnableA20

    /* Call architecture specific initialization code */
    call InitializeCpu

    /* Jump to Stage2 */
    call RunStage2

Clear8042:
    /* Clear 8042 PS/2 buffer */
    nop
    nop
    nop
    nop
    inb $0x64, %al
    cmpb $0xff, %al
    je .Clear8042_Done
    testb $0x02, %al
    jnz Clear8042
.Clear8042_Done:
    ret

EnableA20:
    /* Enable A20 gate */
    pushaw
    call Clear8042
    movb $0xD1, %al
    outb %al, $0x64
    call Clear8042
    movb $0xDF, %al
    outb %al, $0x60
    call Clear8042
    movb $0xFF, %al
    outb %al, $0x64
    call Clear8042
    popaw
    ret

FindFatEntry:
    /* Find a file or directory in the FAT table */
    pushw %bx
    pushw %cx
    pushw %dx
    pushw %si
    pushw %di
.FindFatCluster:
    /* Find FAT32 cluster holding the entry */
    cmp $0x0FFFFFF8, %eax
    jae .FindEntryFail
    pushl %eax
    movw $0x0200, %bx
    movw %bx, %es
    call ReadCluster
    popl %eax
    movb SectorsPerCluster - Start(%bp), %cl
    shlw $0x04, %cx
    xorw %di, %di
.FindEntryLoop:
    /* Find the entry */
    movb %es:(%di), %al
    cmpb $0x00, %al
    je .FindEntryFail
    cmpb $0xE5, %al
    je .FindSkipEntry
    movb %es:0x0B(%di), %ah
    cmpb $0x0F, %ah
    je .FindSkipEntry
    pushw %di
    pushw %si
    pushw %cx
    movw $0x0B, %cx
    repe cmpsb
    popw %cx
    popw %si
    popw %di
    jnz .FindSkipEntry
    movw %es:0x1A(%di), %ax
    movw %es:0x14(%di), %dx
    shll $0x10, %edx
    orl %edx, %eax
    clc
    jmp .FindEntryDone
.FindSkipEntry:
    /* Skip to the next entry */
    addw $0x20, %di
    decw %cx
    jnz .FindEntryLoop
    call GetFatEntry
    jmp .FindFatCluster
.FindEntryFail:
    /* Error, file/directory not found */
    stc
.FindEntryDone:
    /* Clean up the stack */
    popw %di
    popw %si
    popw %dx
    popw %cx
    popw %bx
    ret

GetFatEntry:
    /* Get FAT32 sector and offset from FAT table */
    shll $0x02, %eax
    movl %eax, %ecx
    xorl %edx, %edx
    movzwl BytesPerSector - Start(%bp), %ebx
    pushl %ebx
    divl %ebx
    movzwl ReservedSectors - Start(%bp), %ebx
    addl %ebx, %eax
    movl HiddenSectors - Start(%bp), %ebx
    addl %ebx, %eax
    popl %ebx
    decl %ebx
    andl %ebx, %ecx
    movzwl ExtendedFlags - Start(%bp), %ebx
    andw $0x0F, %bx
    jz LoadFatSector
    cmpb FatCopies - Start(%bp), %bl
    jae FsError
    pushl %eax
    movl BigSectorsPerFat - Start(%bp), %eax
    mull %ebx
    popl %edx
    addl %edx, %eax

LoadFatSector:
    /* Load FAT32 sector from disk */
    pushl %ecx
    movw $0x9000, %bx
    movw %bx, %es
    cmpl %esi, %eax
    je .LoadFatSectorDone
    movl %eax, %esi
    xorw %bx, %bx
    movw $0x01, %cx
    call ReadSectors
.LoadFatSectorDone:
    /* Clean up the stack */
    popl %ecx
    movl %es:(%ecx), %eax
    andl $0x0FFFFFFF, %eax
    ret

LoadStage2:
    /* Load Stage2 executable, first find file in the path */
    movl $0xFFFFFFFF, %esi
    pushl %esi
    movl 0x7C2C, %eax
    movw $.EfiDirName, %si
    call FindFatEntry
    jc Stage2NotLoaded
    movw $.BootDirName, %si
    call FindFatEntry
    jc Stage2NotLoaded
    movw $.Stage2FileName, %si
    call FindFatEntry
    jc Stage2NotLoaded
    popl %esi
    /* Load XTLDR file from disk */
    cmpl $0x02, %eax
    jb FileNotFound
    cmpl $0x0FFFFFF8, %eax
    jae FileNotFound
    movw $(0xF800 / 16), %bx
    movw %bx, %es
.LoadStage2Loop:
    /* Load file data from disk */
    pushl %eax
    xorw %bx, %bx
    pushw %es
    call ReadCluster
    popw %es
    xorw %bx, %bx
    movb SectorsPerCluster - Start(%bp), %bl
    shlw $0x05, %bx
    movw %es, %ax
    addw %bx, %ax
    movw %ax, %es
    popl %eax
    pushw %es
    call GetFatEntry
    popw %es
    cmpl $0x0FFFFFF8, %eax
    jb .LoadStage2Loop
    ret

ParseExecutableHeader:
    /* Parse Stage2 PE/COFF executable header */
    pushw %es
    movw $(0xF800 / 16), %ax
    movw %ax, %es
    movl %es:60, %eax
    addl $(4 + 20), %eax
    movl %es:16(%eax), %eax
    addl $0xF800, %eax
    popw %es
    ret

ReadCluster:
    /* Read FAT32 cluster from disk */
    decl %eax
    decl %eax
    xorl %edx, %edx
    movzbl SectorsPerCluster - Start(%bp), %ebx
    mull %ebx
    pushl %eax
    xorl %edx, %edx
    movzbl FatCopies - Start(%bp), %eax
    mull BigSectorsPerFat - Start(%bp)
    movzwl ReservedSectors - Start(%bp), %ebx
    addl %ebx, %eax
    addl HiddenSectors - Start(%bp), %eax
    popl %ebx
    addl %ebx, %eax
    xorw %bx, %bx
    movzbw SectorsPerCluster - Start(%bp), %cx
    call ReadSectors
    ret

/* Include architecture specific code */
.include ARCH_ESP_SOURCE

CpuUnsupported:
    /* Display CPU unsupported message and reboot */
    popal
    movw $.MsgCpuUnsupported, %si
    call Print
    jmp Reboot

FileNotFound:
    /* Display XTLDR not found message and reboot */
    movw $.MsgXtLdrNotFound, %si
    call Print
    jmp Reboot

Stage2NotLoaded:
    /* Clean up the stack and display XTLDR not found message and reboot */
    popl %esi
    jmp FileNotFound

.BootDirName:
    /* Boot directory name */
    .ascii "BOOT       "

.EfiDirName:
    /* EFI directory name */
    .ascii "EFI        "

.MsgCpuUnsupported:
    .ascii "CPU not supported!\r\n\0"

.MsgXtLdrNotFound:
    .ascii "XTLDR Stage2 not found!\r\n\0"

/* Fill the rest of the extra VBR with zeros and add signature */
.fill (2043 - (. - Start)), 1, 0
.ascii "XTLDR"
