## ExectOS Ideas
This is a list of ideas that migh but not must be realized.

### XTLDR
 - [ ] Rewrite memory mapping and paging support in bootloader to make it more flexible and architecture independent.
       This should support paging levels, thus allowing to make a use of PML5 on modern AMD64 processors and increasing
       the addressable virtual memory from 256TB to 128PB.
 - [ ] Find graphics card from all PCI devices and identify its framebuffer address when GOP is not supported by UEFI
       firmware and UGA has to be used instead.
