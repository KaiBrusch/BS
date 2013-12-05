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
void vmem_init(void);

void fetch_page(int pt_idx);

void store_page(int pt_idx);

void update_pagetable(int frame);

void vmem_init_null_data();

void init_pagetable_framepage_data();


/* Algorithms */
int find_frame(void);

int choose_algo(void);

void incr_alloc_idx();

int start_fifo(void);

int start_clock(void);

int start_clock2(void);


/* Signals */
void sighandler(int signo);

void signal_proccessing_loop(void);


/* Physical Memory */
#define PAGEFILE "./pagefile.bin"		/**< pagefile name */

void init_pagefile();

void page_fault();


/* Administrative Procedures */
#define LOGFILE "./logfile.txt"        /**< logfile name */

void cleanup();

void logger(struct logevent le);

void open_logfile();

void dump_vmem_structure();


/* Random & Misc */
#define SEED_PF 123456        

#define VOID_IDX -1


#endif /* MMANAGE_H */
