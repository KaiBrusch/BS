/* Definitions for virtual memory management model
 * File: mmanage.h
 *
 * Prof. Dr. Wolfgang Fohl, HAW Hamburg
 * 2010
 */
#ifndef MMANAGE_H
#define MMANAGE_H
#include "vmem.h"
#include <limits.h>

// for connection to shared memory
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>



/** Event struct for logging */
struct logevent {
    int req_pageno;
    int replaced_page;
    int alloc_frame;
    int pf_count;
    int g_count;
};

/* Virtual Memory */
void vmem_init(int shm_file_descriptor);

void fetch_page(int pt_idx);

void store_page(int pt_idx);

void update_pagetable(int frame);

void vmem_init_null_data();

void init_pagetable_framepage_data();


/* Algorithms */
int find_frame(void);

int start_fifo(void);

int start_clock(void);

int start_clock2(void);


/* Signals */
void sighandler(int signo);

void signal_loop(int shm_file_descriptor);


/* Physical Memory */
#define PAGEFILE "./pagefile.bin"		/**< pagefile name */

void init_pagefile();

void page_fault();


/* Administrative Procedures */
#define LOGFILE "./logfile.txt"        /**< logfile name */

void on_programm_finished(int shm_file_descriptor);

void logger(struct logevent le);

void open_logfile();

void print_vmem();


/* Random & Misc */
#define SEED_PF 123456        

#define DUMMY_TAG -1


#endif /* MMANAGE_H */
