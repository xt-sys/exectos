## XT Boot Loader (XTLDR)
The XTLDR, or XTOS Boot Loader, is an EFI (Extensible Firmware Interface) boot loader specifically designed for XTOS.
As an EFI boot loader, XTLDR operates exclusively with EFI-based hardware and is not compatible with non-EFI systems,
like old and deprecated BIOS.

One of the notable features of XTLDR is its modular design. The boot loader is divided into different modules, with only
the essential core being loaded during the boot process. This modular approach allows for a more efficient and
streamlined boot experience, as only the necessary functionality is loaded, reducing the boot time and system resource
usage.

XTLDR includes various modules that provide specific functionalities required for the boot process. For example, there is
a module dedicated to supporting the XTOS boot protocol, which is the specific protocol used by XTOS for loading and
executing the OS kernel. Additionally, there is a module for handling PE/COFF (Portable Executable) binaries, which is
a commonly used format of executable files used by the XTOS.
