#include "translate.h"

// Modulinformationen
MODULE_AUTHOR("Kai Brusch and Matthias Nitsche");
MODULE_LICENSE("MIT");

// Die initialisierung des Modules wurde aus scull uebernommen
module_init(translate_init);
module_exit(translate_cleanup);

// Globale Variablen 
int translate_major;    			// Mayor Numbers
struct translate_dev *translate_devs;   	// Uebersetzungs Device

// Uebersetzungsparameter und Defaultwerte
static char *translate_subst = STD_TRANSLATE_SUBSTR;
static int translate_bufsize = STD_BUFFER_SIZE;

// Die eben gesetzten Variablen werden dem Module uebergeben
module_param(translate_subst, charp, S_IRUGO);
module_param(translate_bufsize, int, S_IRUGO);


// encode_char encodiert ein Char array
void encode_char(char *write_pos) {
    int idx = substr_index_from_char(*write_pos);
    if (idx != VOID_CHAR_IDX) {
        *write_pos = translate_subst[idx];
    }
}

void decode_char(char *read_pos) {
    char * pchar = strchr(translate_subst, *read_pos);
    
	//	falls der Char in dem substr gefunden wurde
    if (pchar != NULL) {
	//	dann hole den originalen Char ueber die 
	// 	position des gefundenen, berechne danach
	// 	die neue position
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


// Device Transaktionen
// Open Operation (Aus Scull)
int translate_open(struct inode *inode, struct file *filp) {
    struct translate_dev *dev = container_of(inode->i_cdev, struct translate_dev, cdev);
 
	// erzeugt einen Pointer zu dem Device von einem 'complicated' inode
    filp->private_data = dev;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_open()\n");
#endif   

	// Wir ueberpruefen ob der User im write oder read Modus ist
	// Falls das Device zur verfuegung steht greifen wir auf das Device zu 
    if( (filp->f_mode & FMODE_WRITE) == FMODE_WRITE) {
	//Wir verringern den Semaphore und ueberpruefen ob wir nun auf das Device zugreifen koennen , falls dies nicht erfolgreich ist, wissen wir, dass das Device busy ist
        if (down_trylock(&dev->writer_open_lock) != 0) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_open: device already being written onto.\n");
#endif
            return -EBUSY;
        }
    } else { 
        if (down_trylock(&dev->reader_open_lock) != 0) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_open: device already being read from.\n");
#endif

            return -EBUSY;
        }
    }
    
    return EXIT_SUCCESS;
}


// Wir schließen nun die File Operation
// Wir ueberpruefen den Mode des Users und verringern die jeweiligen Semaphore,danach ist ein read/write mögliche
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

// Hier wird aus dem Buffer der Char uebersetzt und geschrieben 
ssize_t translate_write(struct file *filp, const char __user *buf,
			size_t count, loff_t *f_pos) {
    struct translate_dev *dev = filp->private_data;
    
	//Position des Writers im Buffer, erlaubt uns vom Char wegzuabstrahieren 
    int p_writer = (dev->write_pos - dev->buffer) / sizeof(char);
    
    // Speichert die Anzahl der bisher kopierten Elemente
	int num_copied_items = 0;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_write()\n");
#endif   

    while (num_copied_items < count) {
	// Reduziert den Semaphore, falls der Buffer voll ist, schlaegt dies fehl
	if (dev->items == translate_bufsize) {

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_write: buffer is full. copied %d items \n",num_copied_items);
#endif

	    return num_copied_items;
	}
	
	// Zu diesem Zeitpunkt ist der Buffer nicht voll, 
	//jedoch sind noch nicht alle Elemente kopiert worden
        
        // Nun wird ein Char von dem User kopiert
        if (copy_from_user(dev->write_pos, buf, 1)){

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_write: copy_from_user failed \n");
#endif
		// Befreie den Semaphore
                return -EFAULT;
        }
        
        
	// Falls wir das translate0 Device sind
	// dann encodierung wir waehrend des schreibens
        if (MINOR(dev->cdev.dev) == MINOR_START_IDX) {
            encode_char(dev->write_pos);
        }
        
	// Nachdem wir einen Char erfolgreich kopiert haben, 
	//inkrementiren wir alle Schleifenvariablen und fahren fort
        
        // Aktualisiere die Pointer
        dev->write_pos = dev->buffer + ((p_writer + 1) % translate_bufsize)* sizeof(char);
        p_writer = (dev->write_pos - dev->buffer) / sizeof(char);
        buf += sizeof(char);

        // Counter aktualisieren
        num_copied_items++;
        dev->items++;
    }

    return num_copied_items;
}


// translate_read() ist die inverse Operation zu translate_write, wir moechten Informationen von einem Device lesen, muessen die jedoch vorher dekodieren 
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
	
        // Zu diesem Zeitpunk haben wir die Informationen
	// und der User moechte diese lesen, wir dekodieren den Buffer
	// und uebergeben dann die Informationen dem User
	// Falls das Device translate1 ist, dann dekodiere
        if (MINOR(dev->cdev.dev) == (MINOR_START_IDX + 1)) {
            decode_char(dev->read_pos);
        }
        
        if (copy_to_user(buf, dev->read_pos, 1)) {
            return -EFAULT;
        }

        // Pointer aktualisieren
        dev->read_pos = dev->buffer + ((p_reader + 1) % translate_bufsize)* sizeof(char);
        p_reader = (dev->read_pos - dev->buffer) / sizeof(char);
        buf += sizeof(char);
	
	// Counter aktualisieren
        num_copied_items++;
        dev->items--;
    }
    
    return num_copied_items;
}

// Kernel initialisierung des Translate Modules. Aus Scull kopiert
static int translate_init(void) {
    int result = EXIT_SUCCESS, i;
    dev_t dev;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_init()\n");
    printk(KERN_NOTICE "translate_init: param subst = %s \n",translate_subst);
    printk(KERN_NOTICE "translate_init: param bufsize = %d \n",translate_bufsize);
#endif

    result = alloc_chrdev_region(&dev, MINOR_START_IDX, NO_OF_DEVICES,"translate\n");
    translate_major = MAJOR(dev);

    if (result != EXIT_SUCCESS) {

#ifdef DEBUG_MESSAGES
    printk(KERN_ALERT "translate_init: error(%d) getting major %d \n",
		result, translate_major);
#endif

	return result;
    }

    // Speicher fuer die Devices allozieren
    translate_devs = kmalloc(NO_OF_DEVICES * sizeof(struct translate_dev),GFP_KERNEL);
    if (!translate_devs) {
        result = -ENOMEM;
	goto fail;
    }
    
    // Inhalt der Device zuruecksetzen
    memset(translate_devs, 0, NO_OF_DEVICES * sizeof(struct translate_dev));

	// Initialisieren der Devices
    for (i = 0; i < NO_OF_DEVICES; i++) {
        struct translate_dev *dev = &translate_devs[i];
        // Allocate Buffer
        dev->buffer = kmalloc(translate_bufsize, GFP_KERNEL);
    	if (!(dev->buffer)) {
    	    result = -ENOMEM;
    	    goto fail;
    	}
    	
    	dev->items = 0;
    	dev->read_pos = dev->buffer;
    	dev->write_pos = dev->buffer;
    	
    	// Initialisieren der Semaphore
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

// Setup Char Device (Aus Scull kopiert)
static void translate_setup_cdev(struct translate_dev *dev, int index) {
    int result = EXIT_SUCCESS, devno = MKDEV(translate_major, MINOR_START_IDX + index);
    
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

// Cleanup Procedure. Aus Scull kopiert
static void translate_cleanup(void) {
    dev_t dev = MKDEV(translate_major, MINOR_START_IDX);
    int i;

#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_cleanup()\n");
#endif

    if (translate_devs != NULL) {
        for (i = 0; i < NO_OF_DEVICES; i++) {

            struct translate_dev *dev = &(translate_devs[i]);
            // Buffer free
            kfree(dev->buffer);
            // Pointer zuruecksetzen
            dev->read_pos = NULL;
            dev->write_pos = NULL;
            dev->buffer = NULL;
            // Char dev loeschen
            cdev_del(&dev->cdev);
    
#ifdef DEBUG_MESSAGES
    printk(KERN_NOTICE "translate_cleanup: kfree'd translate dev %d\n", i);
#endif

        }
        // Device Memory freigeben
        kfree(translate_devs);
    }
    unregister_chrdev_region(dev, NO_OF_DEVICES);
}

