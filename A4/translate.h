#ifndef TRANSLATE_H_
#define TRANSLATE_H_

// aus dem scull-main
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>       // printk()
#include <linux/slab.h>         // kmalloc()
#include <linux/fs.h>           // vieles...
#include <linux/errno.h>        // error codes
#include <linux/types.h>        // size_t
#include <linux/fcntl.h>	// file operations
#include <linux/cdev.h>         // cdev
#include <linux/kdev_t.h>	// dev_t
#include <asm/uaccess.h>	// copy_from_user()
#include <linux/string.h>       // strchr(), strlen()

// Translate DEFINE
// standardmaessig ist der Buffer 40
#define STD_BUFFER_SIZE 40

// der default translate substring
#define STD_TRANSLATE_SUBSTR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"    //standard subst
#define SUBSTR_SIZE (strlen(translate_subst))

// die minor nummern beginnen ab 0
#define MINOR_BEGINNING 0

// wir haben die devices translate0 und translate1
#define NO_OF_DEVICES 2

// offset for lower case characters
#define LOWER_A_ASCII 'a'
#define UPPER_A_ASCII 'A'

#define UPPER_SUBSTR_OFFSET (SUBSTR_SIZE/2)

// misc
#define EXIT_SUCCESS 0
#define VOID_CHAR_IDX (-1)

// each of our devices have this
struct translate_dev {
	char *buffer;	// pointer to the buffer
	int items;	// # of items
	char *read_pos;	// current reading position
	char *write_pos;// current writing position
	struct cdev cdev;// char device
	struct semaphore writer_open_lock;	// mutex for writing
	struct semaphore reader_open_lock;	// mutex for reading
};


// Translate fileoperations (auch aus scull ueberneommen)
int translate_open(struct inode *inode, struct file *filp);
int translate_release(struct inode *inode, struct file *filp);
ssize_t translate_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
ssize_t translate_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos);


// Install/Uninstall (oriented on scull)
static int translate_init(void);
static void translate_cleanup(void);
static void translate_setup_cdev(struct translate_dev *dev, int index);
static void cleanup_single_translate_dev(int i);  // cleanup the device of the given index

// Echte Anwendungsfunktionen
int substr_index_from_char(char c);
void encode_char(char *write_pos);
char substr_char_from_index(int index);
void decode_char(char *read_pos);
int is_lower_case(char c);
int is_upper_case(char c);
int is_in_lower_case_substr(int idx);


// file_operations interface implementieren
struct file_operations translate_ops = {
    .owner = THIS_MODULE,
    .open  = translate_open,
    .release = translate_release,
    .write = translate_write,
    .read  = translate_read
};


#endif
