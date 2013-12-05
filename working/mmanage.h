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
// struct for the logging event
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

void update_pt(int frame);


/* Algorithms */
int find_frame(void);

int choose_algo(void);

void rotate_alloc_idx();

int start_fifo(void);

int start_clock(void);

int start_clock2(void);


/* Signals */
void sighandler(int signo);

void signal_proccessing_loop(void);


/* Physical Memory */
#define MMANAGE_PFNAME "./pagefile.bin"

void init_pagefile(const char *pfname);

void page_fault();


/* Administrative Procedures */
#define MMANAGE_LOGFNAME "./logfile.txt"        /**< logfile name */

void cleanup();

void logger(struct logevent le);

void open_logfile();

void noticed(char *msg);

void dump_vmem_structure();


/* Misc */

#define MY_RANDOM_MOD 123

#define SEED_PF 123456        /**< Get reproducable pseudo-random numbers for
                           init_pagefile */

#define VOID_IDX -1


#endif /* MMANAGE_H */
