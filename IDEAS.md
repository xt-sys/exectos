# ExectOS Ideas
This is a list of ideas that migh but not must be realized.

[ ] Move processor context initialization (GDT, IDT, LDT, PCR, TSS) from XTLDR to XTOSKRNL to simplify the XTOS boot
    protocol.
[ ] Rewrite memory mapping and paging support in bootloader to make it more flexible and architecture independent.
    This should support paging levels, thus allowing to make a use of PML5 on modern AMD64 processors and increasing
    the addressable virtual memory from 256TB to 128PB.
