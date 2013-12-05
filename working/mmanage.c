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
 * TODO:
 * currently nothing
 * */

#include "mmanage.h"

struct vmem_struct *vmem = NULL;
FILE *pagefile = NULL;
FILE *logfile = NULL;
int signal_number = 0;

int shm_file_descriptor;

/* MAIN */
int main(void){
    
    struct sigaction sigact;

    init_pagefile();

    open_logfile();

    vmem_init();
    
    /* Setup signal handler */
    /* Handler for USR1 */
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if(sigaction(SIGUSR1, &sigact, NULL) != 0) {
        perror("Error installing signal handler for USR1!\n");
        exit(EXIT_FAILURE);
    } 

#ifdef DEBUG_MESSAGES
    else {
        fprintf(stderr, "USR1 handler successfully installed.\n");
    }
#endif

    if(sigaction(SIGUSR2, &sigact, NULL) != 0) {
        perror("Error installing signal handler for USR2!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    else {
        fprintf(stderr, "USR2 handler successfully installed.\n");
    }
#endif

    if(sigaction(SIGINT, &sigact, NULL) != 0) {
        perror("Error installing signal handler for INT!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    else {
        fprintf(stderr, "INT handler successfully installed.\n");
    }
#endif

    signal_proccessing_loop();
    
    exit(EXIT_SUCCESS);
}
/* End Main */



/* Signal Processing */
void signal_proccessing_loop() {
    fprintf(stderr, "Memory Manager at pid(%d).\n", getpid());

    while(1) {

    	signal_number = 0;
    	pause();

    	if(signal_number == SIGUSR2) {

#ifdef DEBUG_MESSAGES
           fprintf(stderr, "Signal recieved(SIGUSR2): dumping virtual memory.\n");
#endif

           signal_number = 0;
           dump_vmem_structure();
    	} else if(signal_number == SIGINT) {

#ifdef DEBUG_MESSAGES
           fprintf(stderr, "Signal recieved(SIGINT): Quitting...\n");
#endif

           signal_number = 0;
    	   cleanup();
    	   break;
    	}
    }
}

void sighandler(int signo) {
    signal_number = signo;
    
    // page fault has to be processed inside sig_handler
    if(signal_number == SIGUSR1) {

#ifdef DEBUG_MESSAGES
        fprintf(stderr, "Signal recieved(SIGUSR1): Processing Pagefault\n");
#endif
           
        signal_number = 0;
        page_fault();
    }
}

void page_fault() {
    int new_page = VOID_IDX;
    int new_frame = VOID_IDX;
    int req_page = vmem->adm.req_pageno;

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "\n<=== Pagefault ===>\n");
    // A Pagefault has occured
    fprintf(stderr, "Requested Page: %d\n", req_page);
#endif  

    vmem->adm.pf_count++;
    
    new_frame = find_frame();
    
    new_page = vmem->pt.framepage[new_frame];
    
    if( vmem->adm.size >= VMEM_NFRAMES ) {
	   store_page(new_page);
    }

    update_pagetable(new_frame);
    
    fetch_page(req_page);
    
    
    // make Logs
    struct logevent le;
    le.req_pageno = vmem->adm.req_pageno;
    le.replaced_page = new_page;
    le.alloc_frame = new_frame;
    le.pf_count = vmem->adm.pf_count;
    le.g_count = 0;
    logger(le);

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "Page loaded. pf_count: %d\n", vmem->adm.pf_count);
#endif  

    // free the calling process
    sem_post(&vmem->adm.sema);
}
/* End Signal Processing */



/* Algorithms */
int find_frame(){
    int frame = VOID_IDX;
    if( vmem->adm.size < VMEM_NFRAMES ) {
        frame = vmem->adm.size;
        vmem->adm.size += 1;

#ifdef DEBUG_MESSAGES
        fprintf(stderr, "New Frame: %d (by free space)\n", frame);
#endif

    } else {
       frame = choose_algo();

#ifdef DEBUG_MESSAGES
       fprintf(stderr, "New Frame: %d (by algorithm)\n", frame);
#endif

    }
    
    if(frame == VOID_IDX) {

#ifdef DEBUG_MESSAGES
       fprintf(stderr, "<================= FAIL returned Frame is -1 ==============>\n");
#endif

       exit(EXIT_FAILURE);
    }
    return frame;
}

int choose_algo() {
#ifdef FIFO
    return start_fifo();
#endif
#ifdef CLOCK
    return start_clock();
#endif
#ifdef CLOCK2
    return start_clock2();
#endif
}

int start_fifo() {
    int frame = vmem->adm.next_alloc_idx;
    incr_alloc_idx();
    return frame;
}

int start_clock() {
    int frame = VOID_IDX;
    
    while( frame == VOID_IDX ) {
        int alloc_idx = vmem->adm.next_alloc_idx;
        int frame_by_alloc_idx = vmem->pt.framepage[alloc_idx];
        int flags = vmem->pt.entries[frame_by_alloc_idx].flags;
        
        if((flags & PTF_USEDBIT1) == PTF_USEDBIT1) {
            vmem->pt.entries[frame_by_alloc_idx].flags &= ~PTF_USEDBIT1;
            incr_alloc_idx();
        } else {
            frame = alloc_idx;
        }
    }
    incr_alloc_idx();
    
    return frame;
}

int start_clock2() {
    int frame = VOID_IDX;
    
    while( frame == VOID_IDX ) {
    	int alloc_idx = vmem->adm.next_alloc_idx;
    	int frame_by_alloc_idx = vmem->pt.framepage[alloc_idx];
    	int flags = vmem->pt.entries[frame_by_alloc_idx].flags;
    	
    	// if the first USED bit is set, delete the second
    	// if the second was not deleted, delete the first one
        if((flags & PTF_USEDBIT1) == PTF_USEDBIT1) {
    	   int is_second_frame_flag_used = (flags & PTF_USEDBIT2) == PTF_USEDBIT2;
    	   
    	   if( is_second_frame_flag_used ) {
    	       vmem->pt.entries[frame_by_alloc_idx].flags &= ~PTF_USEDBIT2;
    	   } else {
    	       vmem->pt.entries[frame_by_alloc_idx].flags &= ~PTF_USEDBIT1;
    	   }
    	    
    	   // current frame was not eligible so we rotate to the next.
    	   incr_alloc_idx();
    	} else {
    	   frame = alloc_idx;
    	}
    }
    incr_alloc_idx();
    
    return frame;
}

void incr_alloc_idx() {
    vmem->adm.next_alloc_idx++;
    vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);
}
/* End Algorithms */



/* Page Table functions */
void store_page(int page) {
    int frame_has_changed = (vmem->pt.entries[page].flags & PTF_CHANGED) == PTF_CHANGED;
    if(frame_has_changed) {
        int frame = vmem->pt.entries[page].frame;
        // find position in pagefile for current frame
        fseek(pagefile, sizeof(int)*VMEM_PAGESIZE*page, SEEK_SET);
        int write_to_page = fwrite(&vmem->data[VMEM_PAGESIZE*frame], sizeof(int), VMEM_PAGESIZE, pagefile);
        if(write_to_page != VMEM_PAGESIZE) {
            perror("Not everything could be written into the page!\n");
            exit(EXIT_FAILURE);
        }
    }
}

void fetch_page(int page) {
    int frame = vmem->pt.entries[page].frame;
    fseek(pagefile, sizeof(int)*VMEM_PAGESIZE*page, SEEK_SET);
    int readen_ints = fread(&vmem->data[VMEM_PAGESIZE*frame], sizeof(int), VMEM_PAGESIZE, pagefile);
    if(readen_ints != VMEM_PAGESIZE) {
       perror("Not everything could be read!\n");
       exit(EXIT_FAILURE);
    }
}

void update_pagetable(int frame){
    // unset old page
    int oldpage = vmem->pt.framepage[frame];

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "Update Table: Oldpage: %d OldFrame: %d\n", oldpage, frame);
#endif

    // delete all flags
    vmem->pt.entries[oldpage].flags = 0;
    // delete the reference to the frame itwas occupiding
    vmem->pt.entries[oldpage].frame = VOID_IDX;
    
    // update loaded state
    int req_page = vmem->adm.req_pageno;
    vmem->pt.framepage[frame] = req_page;
    vmem->pt.entries[req_page].frame = frame;
    vmem->pt.entries[req_page].flags |= PTF_PRESENT;
}
/* End Page Table functions */


/* Initialization */
void vmem_init(){

    shm_file_descriptor = shm_open(SHMKEY, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(!shm_file_descriptor) {
    	perror("Bad shared memory file descriptor!\n");
    	exit(EXIT_FAILURE);
    }

    if(ftruncate(shm_file_descriptor, SHMSIZE) == -1) {
    	perror("Shared memory could not be truncated with the specified size!\n");
    	exit(EXIT_FAILURE);
    }

    vmem = mmap(NULL, SHMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_file_descriptor, 0);
    if(!vmem){
    	perror("Shared Memory could not be mapped into 'vmem'!\n");
    	exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "Shared Virtual Memory space created. Initializing....\n");
#endif

    vmem_init_null_data();
    
    // initialize Semaphore
    int sem = sem_init(&vmem->adm.sema, 1, 0);
    if(sem != 0) {
    	perror("Semaphor initialization failed!\n");
    	exit(EXIT_FAILURE);
    }
    
    init_pagetable_framepage_data();

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "Virtual Memory sucessfully created and accessible.\n");
#endif

}

void init_pagetable_framepage_data(){
    // pagetable
    for(int i = 0; i < VMEM_NPAGES; i++) {
        vmem->pt.entries[i].flags = 0;
        vmem->pt.entries[i].frame = VOID_IDX;
    }
    // framepage
    for(int i = 0; i < VMEM_NFRAMES; i++) {
       vmem->pt.framepage[i] = VOID_IDX;
    }
    // data
    for(int i = 0; i < (VMEM_NFRAMES * VMEM_PAGESIZE); i++) {
       vmem->data[i] = VOID_IDX;
    }
}

void vmem_init_null_data(){
    vmem->adm.size = 0;
    vmem->adm.mmanage_pid = getpid();
    vmem->adm.shm_id = VOID_IDX;
    vmem->adm.req_pageno = VOID_IDX;
    vmem->adm.next_alloc_idx = 0;
    vmem->adm.pf_count = 0;
}
/* End Initialization */



/* Administrative Functions */
void dump_vmem_structure() {
    fprintf(stderr, " <========== DUMP OF VMEM =========> \n");
    fprintf(stderr, "Administrative Structures:\n");
    fprintf(stderr, "Filled: %d, Next_request: %d pf_count: %d Next_alloc_idx: %d\n",
        vmem->adm.size, vmem->adm.req_pageno, vmem->adm.pf_count, vmem->adm.next_alloc_idx);
    fprintf(stderr, " <========== Data in vmem =========> \n");
    fprintf(stderr, "(index, data)\n");
    for(int i = 0; i < (VMEM_NFRAMES * VMEM_PAGESIZE); i++) {
       fprintf(stderr, "(%d, %d) \n", i, vmem->data[i]);
    }
}

void cleanup(){
    // delete shared memory
    munmap(vmem, SHMSIZE);
    close(shm_file_descriptor);
    shm_unlink(SHMKEY);
    
    // close files
    fclose(logfile);
    fclose(pagefile);
    
    printf("Over and Out!\n");
}
/* End Administrative Functions */



/* Logging */
void open_logfile(){
    logfile = fopen(LOGFILE, "w");
    if(!logfile) {
        perror("Error creating logfile!\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "Logfile created.\n");
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