## ExectOS Known Issues
This is a list of well known bugs that exists in all master branch builds.

### XTLDR
 - [ ] EFI Runtime Services are not mapped properly into higher half. They are mapped itself, but all pointers inside
       that structure point to some physical address that is unavailable after paging is enabled.
