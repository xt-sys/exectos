<p align=center>
  <a href="https://git.codingworkshop.eu.org/xt-sys/exectos">
    <img alt="GIT Repository" src="https://img.shields.io/badge/Source-GIT-blueviolet">
  </a>
  <a href="https://ci.codingworkshop.eu.org/xt-sys/exectos">
    <img alt="Build Status" src="https://ci.codingworkshop.eu.org/api/badges/xt-sys/exectos/status.svg">
  </a>
  <a href="https://git.codingworkshop.eu.org/xt-sys/exectos/src/branch/master/COPYING.md">
    <img alt="License" src="https://img.shields.io/badge/License-GPLv3-blue.svg">
  </a>
  <a href="https://discord.com/invite/3zgjQDVmAe">
    <img alt="Discord" src="https://img.shields.io/discord/723186294540206100">
  </a>
</p>

---

# ExectOS Operating System
ExectOS is an open-source, general purpose operating system written from scratch. It aims to be modular,
maintainable and compatible with existing software. It implements a brand new XT architecture and features
own native application interface. On the backend, it contains a powerful driver model between device drivers
and the kernel, that enables kernel level components to be upgraded without a need to recompile all drivers.

# XT Architecture
ExectOS is a preemptive, reentrant multitasking operating system that implements the XT architecture which derives
from NT architecture. It is modular, and consists of two main layers: microkernel and user modes. Its' kernel mode has
full access to the hardware and system resources and runs code in a protected memory area. It consists of executive
services, which is itself made up on many modules that do specific tasks, a kernel and drivers. Unlike the NT, system
does not feature a separate hardware abstraction layer (HAL) between the physical hardware and the rest of the OS.
Instead, XT architecture integrates a hardware specific code with the kernel. The user mode is made up of subsystems
and it has been designed to run applications written for many different types of operating systems. This allows to
implement any environment subsystem to support applications that are strictly written to the corresponding standard
(eg. DOS, or POSIX).

# Features
 * Modern, EFI enabled operating system
 * Runs on x86 and x86_64 architectures
 * Portable to other architectures
 * Modular design, open-source project
 * Own drivers for commonly used devices
 * NT drivers compatibility layer

# Requirements
ExectOS is in very early development stage, thus its requirements have been not specified yet. However according to its
design, it requires a modern EFI enabled hardware. It is not possible currently to boot ExectOS on a legacy BIOS.

# Source structure
| Directory   | Description                                              |
|-------------|----------------------------------------------------------|
| bootdata    | default configuration and data needed to boot XTOS       |
| drivers     | XT native drivers source code                            |
| sdk/cmake   | Host toolchain configuration and build-related functions |
| sdk/xtdk    | XT Software Development Kit headers                      |
| services    | integral subsystems services source code                 |
| subsystems  | environment subsystems source code                       |
| xtoskrnl    | XTOS kernel source code                                  |
| xtldr       | XTOS boot loader source code                             |

# Build
XTOS can be built only by using XTChain, a special toolchain prepared for compiling XT software. Currently, there is
only a Linux version available, so a Linux distribution or WSL is needed. If XTChain is already installed and
available, then building ExectOS is quiet easy. First, open a terminal or WSL console and type the following command
to launch XTChain build console:
```
xtchain
```
While the console is already running, navigate to the directory containing ExectOS source code and use the following
commands to first set target build architecture and configure the sources:
```
charch [i686|amd64]
chbuild [DEBUG|RELEASE]
./configure.sh
```
Once the sources are configured, enter the build directory and compile the source code:
```
cd build
ninja
```
It is also possible to build a disk image ready to use with QEMU with the following command:
```
ninja diskimg
```

# Contributing
There is a ton of work to do in ExectOS and we appreciate any help. If you are interested in writing features,
porting drivers, fixing bugs, writing tests, creating documentation, or helping out in any other way, we would
love the help. More details on how to contrubite can be found it CONTRIBUTING.md file.

# Licensing
ExectOS is licensed to the public under the terms of the GNU General Public License, version 3. For more
detailed information check the COPYING.md file.

# GIT Mirrors
 * Main GIT Repository: https://git.codingworkshop.eu.org/xt-sys/exectos
 * GitHub Mirror: https://github.com/xt-sys/exectos
 * GitLab Mirror: https://gitlab.com/xt-sys/exectos

# Contact
 * Discord Server: https://discord.gg/3zgjQDVmAe
