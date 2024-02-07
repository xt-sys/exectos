## XTOSKRNL
XTOSKRNL is an XT system kernel executable, providing the kernel and executive layers for XTOS kernel space. It is
a fundamental part of ExectOS, responsible for various core services like hardware abstraction or memory management.
This kernel, contains the scheduler (called sometimes as Dispatcher), cache, object and memory managers, security
manager and other kernel executives described below.

All routines in kernel are prefixed to indicate the kernel subsystem they belong and put inside separate directory.
This is a list of them:

 * Ar - Architecture library
 * Ex - Kernel Executive
 * Hl - Hardware Abstraction Layer (HAL)
 * Ke - Core kernel library
 * Mm - Memory manager
 * Po - Plug&Play and Power Manager
 * Rtl - Runtime library

### AR: Architecture Library
This module contains processor architecture specific functions. This includes enabling and disabling interrupts at
the processor, getting the address of a page fault, getting CPUID information, and performing very early processor
initialization. This module does not contain any manufacturer or board-specific code, only CPU architecture specific
code.

### EX: Kernel Executive
The kernel executive supplies heap management, including support for allocating system memory from paged/non-paged
pools, as well as synchronization primitives like push locks and fast mutexes, interlocked memory access, and worker
threads.

### HL: Hardware Abstraction Layer
Hardware Abstraction Layer (HAL), is a layer between the physical hardware of the computer and the rest of the operating
system. It was designed to hide differences in hardware and therefore it provides a consistent platform on which
the system and applications may run.

### KE: Kernel Library
The kernel implements its core functionality that everything else in the system depends upon. This includes basic
low-level operations such as routing hardware interrupts.

### MM: Memory Manager
Memory Manager is one of core subsystems. It manages virtual memory, controls memory protection and the paging of memory
in and out of physical memory to secondary storage. It also implements a general-purpose allocator of physical memory.

### PO: Plug&Play and Power Manager
This subsystem deals with power events, such as power-off, stand-by, or hibernate. It also handles Plug&Play and
supports device detection and installation at boot time. It is responsible for starting and stopping devices on demand.

### RTL: Runtime Library
This is a required static copy of C runtime objects. It includes many utility functions that can be used by native
applications.

## Functions Naming Convention
When naming a kernel functions, certain conventions are used. The function name is usually structured with
&lt;Prefix&gt;&lt;Operation&gt;&lt;Object&gt;. The prefix denotes the module to which it belongs, thus module
can be identified simply by the name of the function. Additionally, the prefix identifies the function visibility
as well. Thus all private functions, that should not be used from outside of the module has added "p" suffix to
the prefix. For example, KepInitializeStack() routine:
 * Kep - prefix meaning this is private routine belonging to Kernel library (Ke) module,
 * Initialize - operation this function does with the object,
 * Stack - the object is stack.
