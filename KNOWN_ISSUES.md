## ExectOS Known Issues
This is a list of well known bugs that exists in all master branch builds.

### XTLDR
 - [ ] In some specific scenarios (most probably EFI by Insyde) XTLDR cannot load modules. Calling the EFI's
       BootServices->LoadImage() fails with STATUS_EFI_NOT_FOUND (0x800000000000000E) status code. Possibly this is
       a bug in BlFindVolumeDevicePath() routine.
 - [ ] EFI Runtime Services are not mapped properly into higher half. They are mapped itself, but all pointers inside
       that structure point to some physical address that is unavailable after paging is enabled.
