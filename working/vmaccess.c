

#include "vmaccess.h"

struct vmem_struct *vmem = NULL;

void vm_init(){
    // connect to shared memory
    int shm_file_descriptor = shm_open(SHMKEY, O_RDWR, S_IRUSR | S_IWUSR); 
    if(!shm_file_descriptor) {
        perror("shm_open failed!\n");
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG_MESSAGES
    else {
        fprintf(stderr, "shm_open succeeded.\n");
    }
#endif

    // set size of shared memory
    if(ftruncate(shm_file_descriptor, sizeof(struct vmem_struct)) == -1) {
        perror("ftruncate failed! Make sure ./mmanage is running!\n");
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG_MESSAGES
    else {
        fprintf(stderr, "ftruncate succeeded.\n");
    }
#endif

    // make shared memory with the variable vmem accissible
    vmem = mmap(NULL, 
                sizeof(struct vmem_struct), 
                PROT_READ | PROT_WRITE, 
                MAP_SHARED, 
                shm_file_descriptor, 
                0);
    
    if(!vmem) {
        perror("mapping into vmem failed!\n");
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG_MESSAGES
    else {
        fprintf(stderr, "mapping into vmem succeeded!\n");
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

    // mark request for the case of a page fault
    vmem->adm.req_pageno = page;
    
    int flags = vmem->pt.entries[page].flags;
    // check whether the page is currently loaded
    int req_page_is_loaded = ((flags & PTF_PRESENT) == PTF_PRESENT);
    
    if (!req_page_is_loaded) {
	   kill(vmem->adm.mmanage_pid, SIGUSR1);
	   sem_wait(&vmem->adm.sema);
    }
    
    result = read_page(page, offset);
    
    return result;
}

int read_page(int page, int offset) {	
    set_used_bits(page);
    int index = index_from_page_offset(page, offset);
    return vmem->data[index];
}

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

void vmem_write(int address, int data) {
    if(vmem == NULL) {
        vm_init();
    }
    
    int page = address / VMEM_PAGESIZE;
    int offset = address % VMEM_PAGESIZE;
    
    // mark request for the case of a page fault
    vmem->adm.req_pageno = page;
    
    int flags = vmem->pt.entries[page].flags;

    // check whether the page is currently loaded
    int is_page_loaded = ((flags & PTF_PRESENT) == PTF_PRESENT);
    
    if (!is_page_loaded) {
	   kill(vmem->adm.mmanage_pid, SIGUSR1);
	   sem_wait(&vmem->adm.sema);
    }
    
    write_page(page, offset, data);
}

void write_page(int page, int offset, int data) {
    set_used_bits(page);
    
    // mark the change and to make sure it'll be updated
    // into the pagefile.bin
    vmem->pt.entries[page].flags |= PTF_CHANGED;
    
    int index = index_from_page_offset(page, offset);
    vmem->data[index] = data;
}
