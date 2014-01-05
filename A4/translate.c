#include "translate.h"

// Module things
// Metainformation
MODULE_AUTHOR("Kai Brusch and Matthias Nitsche");
MODULE_LICENSE("MIT");

// module init and module exit procedures taken from scull
module_init(translate_init);
module_exit(translate_cleanup);

// global variables 
int translate_major;    // recieved major number
struct translate_dev *translate_devs;    // translate device

// translate parameters (and default values)
static char *translate_subst = STD_TRANSLATE_SUBSTR;
static int translate_bufsize = STD_BUFFER_SIZE;

// make variables into module params
module_param(translate_subst, charp, S_IRUGO);
module_param(translate_bufsize, int, S_IRUGO);


// Char manipulation
void encode_char(char *write_pos) {
    int idx = substr_index_from_char(*write_pos);
    if (idx != VOID_CHAR_IDX) {
        *write_pos = translate_subst[idx];
    }
}

void decode_char(char *read_pos) {
    char * pchar = strchr(translate_subst, *read_pos);
    // if the char was found in substr (aka, had been encoded)
    if (pchar != NULL) {
        // then get the original char according to
        // the position of the finding
        // calc the index using both pointers
        int index = pchar - translate_subst;
        *read_pos = substr_char_from_index(index);
    }
}

char substr_char_from_index(int idx) {
    if( is_in_lower_case_substr(idx) ) {
	   return (LOWER_A_ASCII + idx);
    } else {
	   return (UPPER_A_ASCII + (idx - UPPER_SUBSTR_OFFSET));
    }
}

int substr_index_from_char(char c) {
    int result = VOID_CHAR_IDX;
    if( is_lower_case(c) ){
        return c - LOWER_A_ASCII;
    } else if( is_upper_case(c) ){
        return c - UPPER_A_ASCII + UPPER_SUBSTR_OFFSET;
    }
    return result;
}

int is_lower_case(char c){
    return (c >= 'a') && (c <= 'z');
}

int is_upper_case(char c){
    return (c >= 'A') && (c <= 'Z');
}

int is_in_lower_case_substr(int idx){
    return (idx >= 0) && (idx < UPPER_SUBSTR_OFFSET);
}


// Device Transactions
// open operation (taken from scull)
int translate_open(struct inode *inode, struct file *filp) {
    struct translate_dev *dev = container_of(inode->i_cdev, struct translate_dev, cdev);
    // makes a pointer to the device from a 'complicated' inode
    filp->private_data = dev;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_open()\n");
#endif   

    // we check whether the user is in write or read mode.
    // then we try to access the corresponding part of the device
    // if the device is free for writing/reading
    if( (filp->f_mode & FMODE_WRITE) == FMODE_WRITE) {
	// try to access the device. for that we try to decrease the
	// semaphore value. If we succeed, we may use it.
	// else we say, that the device is busy.
        if (down_trylock(&dev->writer_open_lock) != 0) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_open: device already being written onto.\n");
#endif
            return -EBUSY;
        }
    } else {
	// same goes for reading
        if (down_trylock(&dev->reader_open_lock) != 0) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_open: device already being read from.\n");
#endif

            return -EBUSY;
        }
    }
    
    return EXIT_SUCCESS;
}

// close file operation.
// simply check which mode the user was in and decrease the corresponding
// semaphore. this then allows others to read/write
int translate_release(struct inode *inode, struct file *filp) {
    struct translate_dev *dev = filp->private_data;

#ifdef DEBUG_MESSAGES    
    printk(KERN_NOTICE "translate_close()\n");
#endif
    
    if ((filp->f_mode & FMODE_WRITE) == FMODE_WRITE) {
        up(&dev->writer_open_lock);
    } else {
        up(&dev->reader_open_lock);
    }
    return EXIT_SUCCESS;
}

// implementation of what happens when someone now wants to write
// into our device. we got much help form others groups here.
ssize_t translate_write(struct file *filp, const char __user *buf,
			size_t count, loff_t *f_pos) {
    struct translate_dev *dev = filp->private_data;
    
    // position of the writer on the buffer
    // it abstracts from the type char
    int p_writer = (dev->write_pos - dev->buffer) / sizeof(char);
    
    int num_copied_items = 0;	// tracks the progress of the # of copied items

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_write()\n");
#endif   

    while (num_copied_items < count) {
	// decrease the semaphore
	// if the buffer is full, this fails.
	if (dev->items == translate_bufsize) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_write: buffer is full. copied %d items \n",num_copied_items);
#endif

	    return num_copied_items;
	}
	
        // at this point, the buffer isnt full and
        // there are still items to be copied.
        
        // now copy a single character from the user
        if (copy_from_user(dev->write_pos, buf, 1)){

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_write: copy_from_user failed \n");
#endif
		// free semaphore again and end
                return -EFAULT;
        }
        
        // if we're the translate0 device
        // then encode during writing from user into device
        if (MINOR(dev->cdev.dev) == MINOR_BEGINNING) {
            encode_char(dev->write_pos);
        }
        
        // now that we've succesfully copied (encoded)
        // a char, we increment all loop-dependent-variables.
        
        // update pointers
        dev->write_pos = dev->buffer + ((p_writer + 1) % translate_bufsize)* sizeof(char);
        p_writer = (dev->write_pos - dev->buffer) / sizeof(char);
        buf += sizeof(char);

        // update counters
        num_copied_items++;
        dev->items++;
    }

    return num_copied_items;
}

// implementation of what happens when someone now wants to read
// into our device. we got much help form others groups here.
// this is very similar to the write process
ssize_t translate_read(struct file *filp, char __user *buf,
		       size_t count,loff_t *f_pos) {
    struct translate_dev *dev = filp->private_data;
    int num_copied_items = 0;
    int p_reader = (dev->read_pos - dev->buffer) / sizeof(char);

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_read()\n");
#endif

    while (num_copied_items < count) {
        if (dev->items == 0) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_read: buffer empty, read %d chars \n",num_copied_items);
#endif

	    return num_copied_items;
	}
	
	// because we're got the information now and the user want to read them
	// we'll decode our buffer (if translate1) and THEN hand it over to the user.
        
        // if the device is translate1, then decode
        if (MINOR(dev->cdev.dev) == (MINOR_BEGINNING + 1)) {
            decode_char(dev->read_pos);
        }
        
        if (copy_to_user(buf, dev->read_pos, 1)) {
            return -EFAULT;
        }

        // update pointers
        dev->read_pos = dev->buffer + ((p_reader + 1) % translate_bufsize)* sizeof(char);
        p_reader = (dev->read_pos - dev->buffer) / sizeof(char);
        buf += sizeof(char);
	
	// update counters
        num_copied_items++;
        dev->items--;
    }
    
    return num_copied_items;
}

// called from kernel to initialize translate module. taken from scull
static int translate_init(void) {
    int result = EXIT_SUCCESS, i;
    dev_t dev;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_init()\n");
    printk(KERN_NOTICE "translate_init: param subst = %s \n",translate_subst);
    printk(KERN_NOTICE "translate_init: param bufsize = %d \n",translate_bufsize);
#endif

    result = alloc_chrdev_region(&dev, MINOR_BEGINNING, NO_OF_DEVICES,"translate\n");
    translate_major = MAJOR(dev);

    if (result != EXIT_SUCCESS) {

#ifdef DEBUG_MESSAGES
    printk(KERN_ALERT "translate_init: error(%d) getting major %d \n",
		result, translate_major);
#endif

	return result;
    }

    // allocate memory for the devices
    translate_devs = kmalloc(NO_OF_DEVICES * sizeof(struct translate_dev),GFP_KERNEL);
    if (!translate_devs) {
        result = -ENOMEM;
	goto fail;
    }
    
    // reset contents of device
    memset(translate_devs, 0, NO_OF_DEVICES * sizeof(struct translate_dev));

    // initialize each device (in translate its only two)
    for (i = 0; i < NO_OF_DEVICES; i++) {
        struct translate_dev *dev = &translate_devs[i];
        // allocate buffer (just like with the device memory)
        dev->buffer = kmalloc(translate_bufsize, GFP_KERNEL);
    	if (!(dev->buffer)) {
    	    result = -ENOMEM;
    	    goto fail;
    	}
    	
    	dev->items = 0;
    	dev->read_pos = dev->buffer;
    	dev->write_pos = dev->buffer;
    	
    	// init semaphores
    	sema_init(&dev->reader_open_lock, 1);
    	sema_init(&dev->writer_open_lock, 1);
    	
    	translate_setup_cdev(&translate_devs[i], i);

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_init: translate dev %d initialized", i);
#endif

    }

#ifdef DEBUG_MESSAGES    
    printk(KERN_NOTICE "translate_init: translate initialized");
#endif

    return EXIT_SUCCESS;

    fail: translate_cleanup();
    return result;
}

// setup char device (taken form scull)
static void translate_setup_cdev(struct translate_dev *dev, int index) {
    int result = EXIT_SUCCESS, devno = MKDEV(translate_major, MINOR_BEGINNING + index);
    
#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_setup_cdev()\n");
#endif

    cdev_init(&dev->cdev, &translate_ops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &translate_ops;
    
    result = cdev_add(&dev->cdev, devno, 1);

    if (result) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "Error(%d): adding translate dev %d \n", result, index);
#endif

    }
}

// cleanup procedure. taken from scull
static void translate_cleanup(void) {
    dev_t dev = MKDEV(translate_major, MINOR_BEGINNING);
    int i;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_cleanup()\n");
#endif

    if (translate_devs != NULL) {
        for (i = 0; i < NO_OF_DEVICES; i++) {

            struct translate_dev *dev = &(translate_devs[i]);
            // free Buffer
            kfree(dev->buffer);
            // reset pointers
            dev->read_pos = NULL;
            dev->write_pos = NULL;
            dev->buffer = NULL;
            // delete char dev
            cdev_del(&dev->cdev);
    
#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_cleanup: kfree'd translate dev %d\n", i);
#endif

        }
        // free device memory
        kfree(translate_devs);
    }
    unregister_chrdev_region(dev, NO_OF_DEVICES);
}

