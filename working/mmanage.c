/* Description: Memory Manager BSP3*/
/* Prof. Dr. Wolfgang Fohl, HAW Hamburg */
 /*
 * This is the memory manager process that
 * works together with the vmaccess process to
 * mimic virtual memory management.
 *
 * The memory manager process will be invoked
 * via a SIGUSR1 signal. It maintains the page table
 * and provides the data pages in shared memory
 *
 * This process is initiating the shared memory, so
 * it has to be started prior to the vmaccess process
 *
 * */

#include "mmanage.h"


FILE *pagefile = NULL;
FILE *logfile = NULL;
struct vmem_struct *vmem = NULL;

int signal_number = 0;



/* Main */
int main(void){

    struct sigaction sigact;
    int shm_file_descriptor = shm_open(SHMKEY, 
        O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    init_pagefile();

    open_logfile();

    vmem_init(shm_file_descriptor);

    // initializing signal handler
    // installing the signal handler for SIGUSR1
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if(sigaction(SIGUSR1, &sigact, NULL) != 0) {
        perror("ATTENTION: Failed to install signal handler for SIGUSR1!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    else {
        fprintf(stderr, "SUCCESS: SIGUSR1 handler successfully installed.\n");
    }
#endif

    if(sigaction(SIGUSR2, &sigact, NULL) != 0) {
        perror("ATTENTION: Failed to install signal handler for SIGUSR2!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    else {
        fprintf(stderr, "SUCCESS: SIGUSR2 handler successfully installed.\n");
    }
#endif

    if(sigaction(SIGINT, &sigact, NULL) != 0) {
        perror("ATTENTION: Failed to install signal handler for SIGINT!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    else {
        fprintf(stderr, "SUCCESS: SIGINT handler successfully installed.\n");
    }
#endif

    signal_loop(shm_file_descriptor);

    exit(EXIT_SUCCESS);
}
/* End Main */



/* Signal Processing */
void signal_loop(int shm_file_descriptor) {
    fprintf(stderr, "UPDATE: Memory Manager is at pid(%d).\n", getpid());

    while(1) {

    	signal_number = 0;
    	pause();

        if(signal_number == SIGINT) {

#ifdef DEBUG_MESSAGES
           fprintf(stderr, "UPDATE: Signal recieved(SIGINT) for quitting\n");
#endif

           signal_number = 0;
           on_programm_finished(shm_file_descriptor);
           break;

        }

    	if(signal_number == SIGUSR2) {

#ifdef DEBUG_MESSAGES
           fprintf(stderr, "UPDATE: Signal recieved(SIGUSR2) for dumping virtual memory.\n");
#endif

           signal_number = 0;
           print_vmem();

    	}
    }
}

void sighandler(int signo) {
    signal_number = signo;

    // SIGUSR1 has been assigned to page fault, this will occur if a page fault occurs
    if(signal_number == SIGUSR1) {

#ifdef DEBUG_MESSAGES
        fprintf(stderr, "UPDATE: Signal recieved(SIGUSR1) for page fault\n");
#endif

        signal_number = 0;
        page_fault();
    }
}

void page_fault() {
    int new_page = DUMMY_TAG;
    int new_frame = DUMMY_TAG;
    int req_page = vmem->adm.req_pageno;

    // page fault debug message for log
#ifdef DEBUG_MESSAGES
    fprintf(stderr, "\n ATTENTION: Pagefault Occured \n");
    fprintf(stderr, "UPDATE: Requested Page: %d\n", req_page);
#endif

    vmem->adm.pf_count++;

    new_frame = find_frame();

    new_page = vmem->pt.framepage[new_frame];

    if(vmem->adm.size >= VMEM_NFRAMES) {
	   store_page(new_page);
    }

    update_pagetable(new_frame);

    fetch_page(req_page);


    // we are logging events to ensure transparency
    struct logevent le;
    le.req_pageno = vmem->adm.req_pageno;
    le.replaced_page = new_page;
    le.alloc_frame = new_frame;
    le.pf_count = vmem->adm.pf_count;
    le.g_count = 0;
    logger(le);

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "UPDATE: Page loaded. pf_count: %d\n", vmem->adm.pf_count);
#endif

// sem_post to free up semaphore
    sem_post(&vmem->adm.sema);
}
/* End Signal Processing */



/*  Algorithm
    contains: fifo, clock and clock2 algorithm */

int find_frame(){

    int frame = DUMMY_TAG;

    if(vmem->adm.size < VMEM_NFRAMES) {
        frame = vmem->adm.size;
        vmem->adm.size += 1;

#ifdef DEBUG_MESSAGES
        fprintf(stderr, "UPDATE: New Frame: %d\n", frame);
#endif

#ifdef FIFO
    } else {
        frame = start_fifo();
    }
#endif

#ifdef CLOCK
    } else {
        frame = start_clock();
    }
#endif

#ifdef CLOCK2
    } else {
        frame = start_clock2();
    }
#endif

#ifdef DEBUG_MESSAGES
       fprintf(stderr, "UPDATE: New Frame: %d\n", frame);
#endif

    }

    if(frame == DUMMY_TAG) {

#ifdef DEBUG_MESSAGES
       fprintf(stderr, "ATTENTION: FAIL, returned Frame is -1");
#endif

       exit(EXIT_FAILURE);
    }
    return frame;
}

int start_fifo() {
    int frame = vmem->adm.next_alloc_idx;
    vmem->adm.next_alloc_idx++;
    vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);
    return frame;
}

//clock
int start_clock() {
    int frame = DUMMY_TAG;

    while(frame == DUMMY_TAG) {

        int next_alloc_idx = vmem->adm.next_alloc_idx;
        int framepage_by_idx = vmem->pt.framepage[next_alloc_idx];
        int entry_flags = vmem->pt.entries[framepage_by_idx].flags;
        int is_bit1_used = (entry_flags & PTF_USEDBIT1) == PTF_USEDBIT1;

        if(is_bit1_used) {
            vmem->pt.entries[framepage_by_idx].flags &= ~PTF_USEDBIT1;
            vmem->adm.next_alloc_idx++;
            vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);
        } else {
            frame = next_alloc_idx;
        }

    }

    vmem->adm.next_alloc_idx++;
    vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);

    return frame;
}



// clock2 will also check for the second used bit
// if the first USED bit is set, delete the second
// if the second was not deleted, delete the first one
int start_clock2() {
    int frame = DUMMY_TAG;

    while(frame == DUMMY_TAG) {

        int next_alloc_idx = vmem->adm.next_alloc_idx;
        int framepage_by_idx = vmem->pt.framepage[next_alloc_idx];
        int entry_flags = vmem->pt.entries[framepage_by_idx].flags;
        int is_bit1_used = (entry_flags & PTF_USEDBIT1) == PTF_USEDBIT1;

        if(is_bit1_used) {

    	   int is_bit2_used = (entry_flags & PTF_USEDBIT2) == PTF_USEDBIT2;

    	   if(is_bit2_used) {
    	       vmem->pt.entries[framepage_by_idx].flags &= ~PTF_USEDBIT2;
    	   } else {
    	       vmem->pt.entries[framepage_by_idx].flags &= ~PTF_USEDBIT1;
    	   }

    	   // current frame was not eligible so we rotate to the next.
    	   vmem->adm.next_alloc_idx++;
           vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);

    	} else {
    	   frame = next_alloc_idx;
    	}
    }

    vmem->adm.next_alloc_idx++;
    vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);

    return frame;
}
/* End Algorithm */



/* Page Table Functions */
void store_page(int page) {
    int frame_has_changed = (vmem->pt.entries[page].flags & PTF_CHANGED) == PTF_CHANGED;
    if(frame_has_changed) {
        int frame = vmem->pt.entries[page].frame;
        // find position in pagefile for current frame
        fseek(pagefile, sizeof(int)*VMEM_PAGESIZE*page, SEEK_SET);
        int write_to_page = fwrite(&vmem->data[VMEM_PAGESIZE*frame], sizeof(int), VMEM_PAGESIZE, pagefile);
        if(write_to_page != VMEM_PAGESIZE) {
            perror("ATTENTION: Write was not successful!\n");
            exit(EXIT_FAILURE);
        }
    }
}

void fetch_page(int page) {
    int frame = vmem->pt.entries[page].frame;
    fseek(pagefile, sizeof(int)*VMEM_PAGESIZE*page, SEEK_SET);
    int readen_ints = fread(&vmem->data[VMEM_PAGESIZE*frame], sizeof(int), VMEM_PAGESIZE, pagefile);
    if(readen_ints != VMEM_PAGESIZE) {
       perror("ATTENTION: Read was not successful!\n");
       exit(EXIT_FAILURE);
    }
}

void update_pagetable(int frame){
    // unset previous page
    int oldpage = vmem->pt.framepage[frame];

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "UPDATE: Oldpage: %d OldFrame: %d\n", oldpage, frame);
#endif

    // setting a flag to 0 will delete the flag
    vmem->pt.entries[oldpage].flags = 0;
    // after deleting the flag we need to also delete the reference
    vmem->pt.entries[oldpage].frame = DUMMY_TAG;

    // after deleting the reference we have to update the state
    int req_page = vmem->adm.req_pageno;
    vmem->pt.framepage[frame] = req_page;
    vmem->pt.entries[req_page].frame = frame;
    vmem->pt.entries[req_page].flags |= PTF_PRESENT;
}
/* End Page Table Functions */



/* Initialization */
void vmem_init(int shm_file_descriptor){

    if(!shm_file_descriptor) {
    	perror("ATTENTION: Bad shared memory file descriptor!\n");
    	exit(EXIT_FAILURE);
    }

    if(ftruncate(shm_file_descriptor, SHMSIZE) == -1) {
    	perror("ATTENTION: Shared memory did not fit in the specified space!\n");
    	exit(EXIT_FAILURE);
    }

    vmem = mmap(NULL, SHMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_file_descriptor, 0);
    if(!vmem){
    	perror("ATTENTION: Shared Memory could not be mapped into 'vmem'!\n");
    	exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "SUCCESS: Shared Vmem created. Initializing now\n");
#endif

    vmem_init_null_data();

    int sem = sem_init(&vmem->adm.sema, 1, 0);
    if(sem != 0) {
    	perror("ATTENTION: Semaphor has not been initialized!\n");
    	exit(EXIT_FAILURE);
    }

    init_pagetable_framepage_data();

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "SUCCESS: Vmem sucessfully created\n");
#endif

}

void init_pagefile() {
    int no_elements = VMEM_NPAGES*VMEM_PAGESIZE;
    int data[no_elements];

    srand(SEED_PF);
    // fill with random data. using our own rand_mod
    for(int i=0; i < no_elements; i++) {
        data[i] = rand() % 1000;
    }

    pagefile = fopen(PAGEFILE, "w+b");
    if(!pagefile) {
        perror("ATTENTION: Error creating pagefile!\n");
        exit(EXIT_FAILURE);
    }

    int write_to_page = fwrite(data, sizeof(int), no_elements, pagefile);
    if(!write_to_page) {
        perror("ATTENTION: Error creating pagefile!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "SUCCESS: Pagefile created.\n");
#endif

}

void init_pagetable_framepage_data(){
    // pagetable for loop
    for(int i = 0; i < VMEM_NPAGES; i++) {
        vmem->pt.entries[i].flags = 0;
        vmem->pt.entries[i].frame = DUMMY_TAG;
    }
    // framepage for loop
    for(int i = 0; i < VMEM_NFRAMES; i++) {
       vmem->pt.framepage[i] = DUMMY_TAG;
    }
    // data for loop
    for(int i = 0; i < (VMEM_NFRAMES * VMEM_PAGESIZE); i++) {
       vmem->data[i] = DUMMY_TAG;
    }
}

void vmem_init_null_data(){
    vmem->adm.size = 0;
    vmem->adm.mmanage_pid = getpid();
    vmem->adm.shm_id = DUMMY_TAG;
    vmem->adm.req_pageno = DUMMY_TAG;
    vmem->adm.next_alloc_idx = 0;
    vmem->adm.pf_count = 0;
}
/* End Initialization */



/* Administrative Functions */
void print_vmem() {
    fprintf(stderr, "< adm_struct >\n");
    fprintf(stderr, "size: %d, pf_count: %d ",
        vmem->adm.size, vmem->adm.req_pageno); 
    fprintf(stderr, "req_pageno: %d, next_alloc_idx: %d\n",
        vmem->adm.pf_count, vmem->adm.next_alloc_idx); 
    fprintf(stderr, "< data >\n");
    int total_frame_count = VMEM_NFRAMES * VMEM_PAGESIZE;
    for(int i = 0; i < total_frame_count; i++) {
       fprintf(stderr, "%d \n", vmem->data[i]);
    }
}

void on_programm_finished(int shm_file_descriptor){
    // delete shared memory
    munmap(vmem, SHMSIZE);
    close(shm_file_descriptor);
    shm_unlink(SHMKEY);

    // close files
    fclose(logfile);
    fclose(pagefile);

    printf("SUCESS: Succesful \n");
}
/* End Administrative Functions */



/* Logging */
void open_logfile(){
    logfile = fopen(LOGFILE, "w");
    if(!logfile) {
        perror("ATTENTION: Error creating logfile!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "SUCCESS: Logfile created.\n");
#endif

}

/* Do not change!  */
void
logger(struct logevent le)
{
    fprintf(logfile, "Page fault %10d, Global count %10d:\n"
            "Removed: %10d, Allocated: %10d, Frame: %10d\n",
            le.pf_count, le.g_count,
            le.replaced_page, le.req_pageno, le.alloc_frame);
    fflush(logfile);
}

