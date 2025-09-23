## XTOSKRNL
XTOSKRNL is the core kernel executable for ExectOS, providing the fundamental kernel and executive layers that operate
within the XTOS kernel space. It is responsible for various core services, such as hardware abstraction, memory
management, and process scheduling. The kernel contains the scheduler (sometimes referred to as the Dispatcher), the
cache, object, and memory managers, the security manager, and other executive components described below.

The source code of the kernel is organized into subsystem-specific directories. Each directory name also defines the
corresponding C++ namespace in which the subsystem's classes and routines reside. These subsystems include:

 * Ar - Architecture-specific Library
 * Ex - Kernel Executive
 * Hl - Hardware Layer
 * Kd - Kernel Debugger
 * Ke - Core Kernel Library
 * Mm - Memory Manager
 * Po - Plug&Play and Power Manager
 * Rtl - Runtime library

### AR: Architecture Library
This module contains functions specific to the processor architecture. These include routines for enabling and disabling
interrupts, retrieving the faulting address on a page fault, querying CPUID information, and performing very early
processor initialization. This module contains only CPU architecture-specific code, with no manufacturer or
board-specific implementations.

### EX: Kernel Executive
The Kernel Executive provides services for allocating system memory from paged and non-paged pools. It also supplies
synchronization primitives such as pushlocks and fast mutexes, routines for interlocked memory access, and support for
worker threads.

### HL: Hardware Layer
The Hardware Layer is an abstraction layer between the physical hardware and the rest of the operating system. It is
designed to abstract away hardware differences, providing a consistent platform on which the kernel and applications
can run.

### KD: Kernel Debugger
The Kernel Debugger (KD) subsystem provides debugging support for the kernel. The KD is initialized early in the boot
process to facilitate debugging from the very beginning of the kernel's execution.

### KE: Kernel Library
The Core Kernel Library implements the core functionality upon which the rest of the system depends. This includes
fundamental low-level operations, such as routing hardware interrupts and managing dispatcher objects.

### MM: Memory Manager
The Memory Manager is one of the core subsystems. It manages virtual memory, controls memory protection, and
handles paging memory between physical RAM and secondary storage. It also implements a general-purpose allocator for
physical memory.

### PO: Plug&Play and Power Manager
This subsystem handles power management events, such as shutdown or standby. It also manages Plug and Play (PnP),
supporting device detection and installation at boot time. Furthermore, it is responsible for starting and stopping
devices on demand.

### RTL: Runtime Library
The Runtime Library provides a kernel-mode implementation of common C library functions. It includes many utility
routines, for use by other kernel components.

## Function Naming Convention
All kernel functions adhere to a strict naming convention to enhance code readability and maintainability. The structure
of all public interfaces exposed by the kernel are generally composed of three parts:
&lt;Prefix&gt;&lt;Operation&gt;&lt;Object&gt;

The prefix identifies the component to which the function belongs. For example, consider the **KeInitializeThread()**
routine:
 * **Ke** - The prefix indicates a routine belonging to the Core Kernel Library (Ke).
 * **Initialize** - The operation performed by the function.
 * **Thread** - The object on which the operation is performed.

For all C++ code inside the kernel the naming model has evolved. Consider the **KE::KThread::InitializeThread()**
routine:
 * **KE** - The namespace replaces the prefix and indicates the subsystem. Namespaces are written in uppercase and no
            longer use the trailing p for private routines, because classes use C++ visibility to control access.
 * **KThread** - Within each namespace, related functionality is grouped into classes, which encapsulate variables and
                 methods.
 * **InitializeThread** - Method names follow the `<Operation><Object>` pattern.
 