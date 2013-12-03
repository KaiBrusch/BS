/* Header file for vmappl.c
 * File: vmappl.h
 * Prof. Dr. Wolfgang Fohl, HAW Hamburg
 * 2010
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

// refactores functions for writing and eading into vmem->data
void write_page(int frame, int offset, int data);

int read_page(int frame, int offset);

void count_used(int page);

int calc_index_from_poffset(int page, int offset);

// Misc. - for testing purposes
void dump();

#endif
