# xv6-riscv-peterson-tournament-lock

An extended version of **xv6-riscv** implementing synchronization primitives for an Operating Systems assignment.

This project adds:

- **Peterson locks** in the kernel
- New **system calls** for lock management
- A **userspace tournament tree lock library**
- A test user program called `tournament`

## Overview

The project extends the original xv6-riscv operating system with support for mutual exclusion using **Peterson’s algorithm** and a **tournament-tree lock** built on top of it.

The kernel exposes lock operations through system calls, and the userspace library uses these calls to coordinate multiple competing processes.

## Features

- Kernel-level Peterson lock table
- System calls for:
  - `peterson_create()`
  - `peterson_acquire()`
  - `peterson_release()`
  - `peterson_destroy()`
- Userspace tournament lock implementation in `libtournament.c`
- User program `tournament` for testing lock acquisition and release across multiple processes
  
## Main Idea
1. Peterson Lock
A Peterson lock supports mutual exclusion between two competitors using:

two intent flags

one turn variable

This project manages multiple Peterson locks in the kernel and assigns each one a lock ID.

2. Tournament Tree Lock
To support more than two processes, the project builds a tournament tree:

each internal node is a Peterson lock

each process climbs from leaf to root

acquiring all locks on its path grants entry to the critical section

release happens in reverse order

## Modified / Added Files
Kernel

kernel/petersonlock.h

kernel/petersonlock.c

kernel/syscall.h

kernel/syscall.c

kernel/sysproc.c

kernel/defs.h

kernel/main.c

## User Space
user/libtournament.c

user/tournament.c

user/user.h

user/usys.pl
## Build
Makefile
## Build and Run
Requirements
You need:
qemu-system-riscv64

a RISC-V cross compiler toolchain

make

gcc

## Build and launch xv6
make qemu

After xv6 boots, run the tournament test from the xv6 shell.

## Usage

Inside the xv6 shell:

tournament 2

or:

tournament 4

tournament 8

tournament 16

The program creates a tournament tree for the given number of processes, forks child processes, and shows how they acquire and release the lock while entering the critical section.

## Example Output
[PID 5] Acquired lock (ID assigned for the process 0)

[PID 5] Releasing lock (ID assigned for the process 0)

[PID 6] Acquired lock (ID assigned for the process 1)

[PID 6] Releasing lock (ID assigned for the process 1)


## How It Works
1)The user program calls tournament_create(n)

2)The library allocates the tournament structure

3)Each internal node gets a Peterson lock from the kernel

4)Child processes are forked and assigned positions in the tree

5)Each child acquires locks from bottom to top

6)After entering the critical section, locks are released from top to bottom

## Notes
The number of processes must be a power of 2

The tournament lock is built on top of Peterson locks

This project is intended for educational use as part of an Operating Systems assignment

The codebase is based on the original xv6-riscv project and extends it with synchronization support

## Repository Purpose:
This repository demonstrates:

kernel-user interaction through system calls

mutual exclusion using Peterson’s algorithm

scalable locking using a tournament tree

process synchronization in xv6
