

#include "vmaccess.h"

struct vmem_struct *vmem = NULL;

/* Virtual Memory */
void vm_init(){
    // opens connection to shared memory
    int shm_file_descriptor = shm_open(SHMKEY, O_RDWR, S_IRUSR | S_IWUSR);
    if(!shm_file_descriptor) {
        perror("ATTENTION: Failed to open shm_open !\n");
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG_MESSAGES
    else {
        fprintf(stderr, "SUCCESS: shm_open successfuly openend.\n");
    }
#endif

    // determining and set size of shared memory
    if(ftruncate(shm_file_descriptor, sizeof(struct vmem_struct)) == -1) {
        perror("ATTENTION: ftruncate failed! Urgs.. sorry! \n");
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG_MESSAGES
    else {
        fprintf(stderr, "SUCCESS: ftruncate succeeded.\n");
    }
#endif


    // assign vmem mapping the vmem, this elimantes reference friction
    vmem = mmap(NULL, sizeof(struct vmem_struct),
                PROT_READ | PROT_WRITE,
                MAP_SHARED, shm_file_descriptor, 0);

    if(!vmem) {
        perror("ATTENTION: Failed to map vmem!\n");
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG_MESSAGES
    else {
        fprintf(stderr, "SUCCESS: Successfuly mapped into vmem !\n");
    }
#endif

}

int vmem_read(int address) {
    if(vmem == NULL) {
        vm_init();
    }

    int result;

    int page = address / VMEM_PAGESIZE;
    int offset = address % VMEM_PAGESIZE;

    vmem->adm.req_pageno = page;

    int flags = vmem->pt.entries[page].flags;

    // we need to check if required resource is already present
    int req_page_is_loaded = ((flags & PTF_PRESENT) == PTF_PRESENT);

    if (!req_page_is_loaded) {
	   kill(vmem->adm.mmanage_pid, SIGUSR1);
	   sem_wait(&vmem->adm.sema);
    }

    result = read_page(page, offset);

    return result;
}

void vmem_write(int address, int data) {
    if(vmem == NULL) {
        vm_init();
    }

    int page = address / VMEM_PAGESIZE;
    int offset = address % VMEM_PAGESIZE;


    vmem->adm.req_pageno = page;

    int flags = vmem->pt.entries[page].flags;

    // we need to check if required resource is already present
    int is_page_loaded = ((flags & PTF_PRESENT) == PTF_PRESENT);

    if (!is_page_loaded) {
       kill(vmem->adm.mmanage_pid, SIGUSR1);
       sem_wait(&vmem->adm.sema);
    }

    write_page(page, offset, data);
}
/* End Virtual Memory */



/* Page Functions */
int read_page(int page, int offset) {
    set_used_bits(page);
    int index = index_from_page_offset(page, offset);
    return vmem->data[index];
}

void write_page(int page, int offset, int data) {
    set_used_bits(page);

    // mark changes in pagefileb.bin
    vmem->pt.entries[page].flags |= PTF_CHANGED;

    int index = index_from_page_offset(page, offset);
    vmem->data[index] = data;
}
/* End Page Functions */



/* Helper Functions */
int index_from_page_offset(int page, int offset) {
    return (vmem->pt.entries[page].frame*VMEM_PAGESIZE) + offset;
}

void set_used_bits(int page) {
    int used_bit1_is_set = (vmem->pt.entries[page].flags & PTF_USEDBIT1) == PTF_USEDBIT1;
    if(used_bit1_is_set) {
        // then set the second usedbit
        vmem->pt.entries[page].flags |= PTF_USEDBIT2;
    }

    vmem->pt.entries[page].flags |= PTF_USEDBIT1;
}
