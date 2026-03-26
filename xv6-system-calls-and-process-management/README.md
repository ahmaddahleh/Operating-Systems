# xv6 System Calls and Process Management

This project is based on the xv6-riscv teaching operating system and extends it with several kernel and userspace features related to system calls, process termination, and multi-process work distribution.

## Overview

The project includes four main tasks:

1. **Hello World userspace program**
   - Added a simple userspace program that prints `Hello World xv6`.

2. **`memsize` system call**
   - Implemented a new system call that returns the memory size of the currently running process.
   - Added a userspace test program to measure memory usage before allocation, after allocation, and after release.

3. **Exit message support**
   - Extended the `exit` and `wait` mechanisms to support an additional exit message.
   - Added a userspace program that exits with the message `Goodbye World xv6`.
   - Modified the shell to print the exit message when a child process terminates.

4. **`forkn` and `waitall` system calls**
   - Implemented new system calls for creating multiple child processes and waiting for all of them to finish.
   - Added a userspace program that distributes array-sum computation across several child processes and collects the results in the parent process.

## Key Concepts

- Operating Systems
- xv6
- Kernel development
- System calls
- Process control
- Parent-child synchronization
- Userspace / kernelspace interaction

## Files and Components

Important modified and added areas include:

- `kernel/syscall.h`
- `kernel/syscall.c`
- `kernel/sysproc.c`
- `kernel/proc.c`
- `kernel/proc.h`
- `user/user.h`
- `user/usys.pl`
- `user/helloworld.c`
- `user/memsize_test.c`
- `user/goodbye.c`
- `user/bigarray.c`
- `user/forkn_test.c`
- `user/forkn_waitall_test.c`
- `user/sh.c`
- `Makefile`

## How to Run

Build and run xv6:
make qemu

Then run the userspace programs from the xv6 shell, for example:

helloworld
memsize_test
goodbye
bigarray
forkn_test
forkn_waitall_test

## What I Learned
Through this project, I gained hands-on experience with:

adding new system calls to an operating system
modifying kernel data structures
transferring data between userspace and kernelspace
extending process exit and wait behavior
coordinating multiple child processes
testing kernel-level features with custom userspace programs
Notes

This project was developed as part of an Operating Systems course using xv6-riscv for educational purposes.

Author:
Ahmad Dahleh
