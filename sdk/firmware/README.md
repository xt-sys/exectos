## Open Virtual Machine Firmware (OVMF)
This directory contains the essential components of the Open Virtual Machine Firmware (OVMF), which provides
an open-source implementation of the Unified Extensible Firmware Interface (UEFI) for virtual machines.

The ovmf_code files contain the CPU architecture specific UEFI firmware code, which serves as the initial boot code
for virtual machines used to test the XTOS. It includes the necessary instructions and functions to initialize hardware,
load the operating system, and provide various system services during the boot process. The ovmf_code file is
responsible for establishing a UEFI environment within the virtual machine, enabling it to boot and operate effectively.

The ovmf_pure files contains the minimum version of the CPU specific UEFI firmware code, suitable for use with Bochs.

The ovmf_vars files, store UEFI variables, which are used to store and retrieve system configuration information, such as
boot options, device settings, and system preferences. The ovmf_vars file contains the persistent variables specific to
a virtual machine, allowing it to maintain its configuration across multiple boot sessions.

## Video BIOS (LGPL'd VGABios)
The vgabios.bin file contains the Video Bios for Bochs and QEMU. This VGA Bios is very specific to the emulated VGA card.
It is NOT meant to drive a physical vga card. It also implements support for VBE version 2.0.
