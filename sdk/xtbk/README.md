## XT Building Kit (XTBK)
The XTBK, or XT Building Kit is a kind of SDK (Software Development Kit) utilized internally by XTOS, the XT Operating
System. It is designed to provide a collection of public functions that are available within the operating system but
not necessarily exposed or accessible to software and driver developers.

Unlike XTDK, which focuses on providing headers for external developers to create kernel mode drivers and user mode
applications, XTBK serves as an extension to XTDK and aids in the code-sharing process between different XTOS
components. This enables the reuse of code across various components of the operating system, resulting in a more
efficient and streamlined development process.

By incorporating XTBK, XTOS can optimize code reuse, particularly in low-level kernel code that can be shared with other
components like the boot loader. This approach helps in reducing code duplication and improving overall code
maintainability. Additionally, it allows for consistent implementation of functionality across different parts of the OS.
