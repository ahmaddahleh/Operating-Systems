# xv6-riscv-shared-memory-logger

An extended version of **xv6-riscv** that adds:

- shared memory mapping between processes
- shared memory unmapping
- support for shared-page ownership tracking
- a userspace shared-memory test program
- a multi-process logging system built on top of shared memory

## Overview

This project extends the original **xv6-riscv** operating system with a simple shared-memory mechanism between processes.

The main idea is to allow one process to expose pages from its virtual address space to another process by mapping the same physical pages into the destination process. On top of that, the project implements a **multi-process logger**, where several child processes write messages into a shared buffer and the parent reads and prints them.

## Features

### Task 1 — Memory Sharing
- Add kernel support for mapping shared pages from one process to another
- Add kernel support for unmapping shared pages
- Mark shared mappings using a dedicated page-table flag
- Prevent shared physical pages from being freed incorrectly on process exit
- Expose the functionality to userspace through new system calls
- Test the implementation with `shmem_test`

### Task 2 — Multi-process Logging
- Implement a shared logging buffer in userspace
- Fork multiple child processes
- Map the parent’s shared buffer into each child
- Use atomic header writes with `__sync_val_compare_and_swap`
- Let the parent scan and print messages safely from the shared buffer
- Test the implementation with `log_test`

## Implemented System Calls

The project adds two userspace-visible system calls:

void* map_shared_pages(int src_pid, int dst_pid, void *src_va, int size);

int unmap_shared_pages(void *addr, int size);

## Main Idea
### Shared Memory

The kernel:

locates the source and destination processes

finds the physical pages backing the source virtual address

maps those physical pages into the destination process

marks the mappings as shared using a custom PTE_S flag

This allows both processes to access the same physical memory through possibly different virtual addresses.

### Safe Cleanup

Since xv6 normally assumes every mapped page belongs to the current process, exiting a process with shared mappings could incorrectly free physical pages more than once.

To fix this, the project updates the unmapping logic so that:

shared mappings are removed from the page table
but their physical memory is not freed by the non-owning process

### Multi-process Logging

The logging system stores messages in a shared buffer using this format:

32-bit header

  upper 16 bits: child index
  
  lower 16 bits: message length

A zero header means the slot is free.

Each child:

scans the buffer

looks for a free header

atomically claims it

writes its message

The parent scans the same shared buffer and prints the collected messages.

### Relevant Files
## Kernel

kernel/proc.c — shared page mapping and unmapping logic

kernel/sysproc.c — syscall handlers

kernel/syscall.c — syscall dispatch table

kernel/syscall.h — syscall numbers

kernel/defs.h — function declarations

kernel/riscv.h — shared-page flag definition

kernel/vm.c — unmapping behavior adjusted for shared pages

## User Space

user/shmem_test.c — Task 1 test program

user/log_test.c — Task 2 logging program

user/user.h — syscall declarations

user/usys.pl — syscall stubs

## Build

Makefile — includes the new user programs in the xv6 image

## How to Build

Make sure you have:

make

gcc

qemu-system-riscv64

a RISC-V cross compiler toolchain

Then build and run xv6:

make qemu

## How to Run

After xv6 boots, run the test programs from the xv6 shell.

## Shared memory test
shmem_test

This runs the shared-memory test and checks that the parent can read data written by the child.

To run the version that performs explicit unmapping:

shmem_test -u
## Logging test
log_test 4

You can also try a larger number of children:

log_test 6

log_test 8

## Expected Behavior
### shmem_test

The program demonstrates:

process size before mapping

process size after mapping

writing "Hello daddy" in the child

reading the same content in the parent

optional unmapping and size updates

safe cleanup when a process exits

### log_test

The program demonstrates:

multiple children writing to one shared buffer

atomic claiming of log slots

parent-side reading and decoding of log entries

no overlapping or corrupted messages in the normal path

### Example output may look like:

[Child] wrote to shared mem: "Hello daddy"
[Parent] read from shared: "Hello daddy"

[child 0] Hello daddy!

[child 1] Hello daddy!

[child 2] Hello daddy!

[child 3] Hello daddy!

## Learning Goals

-This project demonstrates:

-how virtual memory maps virtual addresses to physical memory

-how page tables can be reused for inter-process shared memory

-how to safely track ownership of mapped pages

-how to expose kernel functionality through system calls

-how to use atomic operations for multi-process coordination

## Notes
-This project is based on the original xv6-riscv teaching operating system

-The implementation is intended for educational use in an Operating Systems course

-The shared logging system is a simple teaching example, not a production-grade logging framework
