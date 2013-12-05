/* Header file for vmappl.c
 * File: vmappl.h
 * Prof. Dr. Wolfgang Fohl, HAW Hamburg
 */

#ifndef VMACCESS_H
#define VMACCESS_H

// Imports to connect to virtual memory
#include "vmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Connect to shared memory (key from vmem.h) */
void vm_init(void);

/* Read from "virtual" address */
int vmem_read(int address);

/* Write data to "virtual" address */
void vmem_write(int address, int data);

/* Write data to a frame in a page by offset */
void write_page(int frame, int offset, int data);

/* Read out a given frame */
int read_page(int frame, int offset);

void set_used_bits(int page);

int index_from_page_offset(int page, int offset);

#endif
