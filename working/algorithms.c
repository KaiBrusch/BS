
#include "algorithms.h"


/*  Algorithm
    contains: fifo, clock and clock2 algorithm */

int find_frame(){

    int frame = DUMMY_TAG;

    if(vmem->adm.size < VMEM_NFRAMES) {
        frame = vmem->adm.size;
        vmem->adm.size += 1;
    }

#ifdef FIFO
    else {
        frame = start_fifo();
    }
#endif

#ifdef CLOCK
    else {
        frame = start_clock();
    }
#endif

#ifdef CLOCK2
    else {
        frame = start_clock2();
    }
#endif

#ifdef DEBUG_MESSAGES
    fprintf(stderr, "UPDATE: New Frame: %d\n", frame);
#endif

    if(frame == DUMMY_TAG) {

#ifdef DEBUG_MESSAGES
       fprintf(stderr, "ATTENTION: FAIL, returned Frame is -1");
#endif

       exit(EXIT_FAILURE);
    }

    return frame;

}

int start_fifo() {
    int next_alloc_idx = vmem->adm.next_alloc_idx;
    vmem->adm.next_alloc_idx++;
    // Rotate
    vmem->adm.next_alloc_idx%=(VMEM_NFRAMES);
    return next_alloc_idx;
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