## ExectOS Ideas
This is a list of ideas that migh but not must be realized.

### SDK
 - [ ] Currently XT Development Kit (XTDK) is a garbage. It should be cleaned up the way, it contains all structures
       and definitions, as well as all routines that are exported and can be used by other components or software
       dynamically linked. All other routines should be available as well in some form, as some libraries can share
       code with others (eg. XTLDR calls routines exported by XTOSKRNL).

### XTLDR
 - [ ] Rewrite memory mapping and paging support in bootloader to make it more flexible and architecture independent.
       This should support paging levels, thus allowing to make a use of PML5 on modern AMD64 processors and increasing
       the addressable virtual memory from 256TB to 128PB.
 - [ ] Find graphics card from all PCI devices and identify its framebuffer address when GOP is not supported by UEFI
       firmware and UGA has to be used instead.

### XTOSKRNL
 - [ ] Implement mechanism for detecting CPU features and checking hardware requirements. If CPU does not meet
       requirements, it should cause a kernel panic before any non-supported instruction is being used.
