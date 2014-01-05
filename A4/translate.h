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

// standard größe des Buffers 40
#define STD_BUFFER_SIZE 40

#define STD_TRANSLATE_SUBSTR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define SUBSTR_SIZE (strlen(translate_subst))

// offset für Alphabet Grenzen
#define LOWER_A_ASCII 'a'
#define UPPER_A_ASCII 'A'
#define UPPER_SUBSTR_OFFSET (SUBSTR_SIZE/2)

// Minor hat bei uns den channel 0 und 1
#define MINOR_START_IDX 0
// translate0 und translate1 = 2 devices
#define NO_OF_DEVICES 2

// Sonstiges
#define EXIT_SUCCESS 0
#define VOID_CHAR_IDX (-1)

// Charakter Manipulation und Codierung
int substr_index_from_char(char c);
void encode_char(char *write_pos);
char substr_char_from_index(int index);
void decode_char(char *read_pos);
int is_lower_case(char c);
int is_upper_case(char c);
int is_in_lower_case_substr(int idx);


// alle devices haben dieses standard interface
struct translate_dev {
	char *buffer;	// pointer zu dem buffer
	int items;
	char *read_pos;	// aktueller lese pointer
	char *write_pos;// aktueller schreibe pointer
	struct cdev cdev;// Charakter device
	struct semaphore writer_open_lock;
	struct semaphore reader_open_lock;
};

// Translate Operationen nach Scull
int translate_open(struct inode *inode, struct file *filp);
int translate_release(struct inode *inode, struct file *filp);
ssize_t translate_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
ssize_t translate_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos);


// Translate Install/Uninstall nach Scull
static int translate_init(void);
static void translate_cleanup(void);
static void translate_setup_cdev(struct translate_dev *dev, int index);


// Dateioperationen als Interface
struct file_operations translate_ops = {
    .owner = THIS_MODULE,
    .open  = translate_open,
    .release = translate_release,
    .write = translate_write,
    .read  = translate_read
};


#endif
