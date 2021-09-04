#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/mm.h> 
#include <asm/io.h>
#include <linux/slab.h> 
#include <linux/kernel.h> 
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include <linux/interrupt.h>

#include "memDev.h"


#define DEV_ID_COUNT 10

dev_t g_mem_dev;
struct cdev * g_pdev;
void *g_pBuffer = NULL;

unsigned int g_size = 0;
unsigned char *mem_msg_buf = NULL;

MsgEvtProcessFunc msgEvtProcess = NULL;

wait_queue_head_t    mem_poll_wait;




static ssize_t char_read(struct file *filp,char __user *buffer,size_t,loff_t *);
static int char_open(struct inode *,struct file *);
static ssize_t char_write(struct file *filp,const char __user *buffer,size_t ,loff_t*);
static int char_release(struct inode *,struct file *); 
static int mem_mmap(struct file *filp, struct vm_area_struct *vma);
static long mem_ioctl(struct file *pFile, unsigned int cmd, unsigned long add);
static unsigned int mem_poll(struct file* file, poll_table* wait);


static unsigned char * malloc_reserved_mem(unsigned int size);

struct file_operations Fops = {
    .read = char_read,
    .write = char_write,
    .open = char_open,
    .release = char_release,/* a.k.a. close */
    .mmap = mem_mmap,
    .compat_ioctl = mem_ioctl,
    .unlocked_ioctl = mem_ioctl,
    .poll = mem_poll,
};

/*------------ Kernel Hook --------------------*/
typedef void (* SoftIrqHook )(char cInOut, struct softirq_action* pCur, struct softirq_action* pVec);
/*extern*/ SoftIrqHook gFunc_softIrqHook;
typedef void (*TaskletHook )(char cInOut, struct tasklet_struct *pList);
/*extern*/ TaskletHook gFunc_TaskletHook;

void softIrqHook(char cInOut, struct softirq_action* pCur, struct softirq_action* pVec);
void taskletHook(char cInOut, struct tasklet_struct *pList);

/*
    HI_SOFTIRQ=0,
    TIMER_SOFTIRQ,
    NET_TX_SOFTIRQ,
    NET_RX_SOFTIRQ,
    BLOCK_SOFTIRQ,
    BLOCK_IOPOLL_SOFTIRQ,
    TASKLET_SOFTIRQ,
    SCHED_SOFTIRQ,
    HRTIMER_SOFTIRQ,
    RCU_SOFTIRQ,
    NR_SOFTIRQS
*/
unsigned long long  g_iCurrentSoftirq = 33;
unsigned long long g_ullSoftirqTimes[10];
unsigned long long  g_ullTaskletTimes;

/*--------------------------------------*/

int memDev_init(unsigned int size, unsigned char **ppBuf, MsgEvtProcessFunc process){
    int iRes, devno;
    printk(KERN_DEBUG "memDev_init() enter. \n");
    printk(KERN_DEBUG "memDev_init() KMALLOC_MAX_SIZE=%u\n", KMALLOC_MAX_SIZE);

    iRes = alloc_chrdev_region(&g_mem_dev,0,DEV_ID_COUNT,"memMsgDev");
    if (likely(0 == iRes)){
        printk(KERN_DEBUG "[memMsgDev] alloc_chrdev_region succeful, Major=%d, minor=%d.\n",
               MAJOR(g_mem_dev),
               MINOR(g_mem_dev));
    }
    else{
        printk(KERN_DEBUG "[memMsgDev] alloc_chrdev_region fail, iRes=%d.\n",
               iRes);
        return -1;
    }

    g_pdev = cdev_alloc();
    cdev_init(g_pdev, &Fops);
    iRes = cdev_add(g_pdev, g_mem_dev, 1);
    if (likely(0 == iRes)){
        printk(KERN_DEBUG "[memMsgDev] cdev_add succeful.\n");
    }
    else{
        printk(KERN_DEBUG "[memMsgDev] cdev_add fail, iRes-%d.\n", iRes);
        return -2;
    }
    if (NULL == mem_msg_buf){
        printk(KERN_DEBUG "[memMsgDev] need to init msg dev. \n");
        g_size = size;
        if (size % 4096 > 0){
            g_size += 4096 - size % 4096;
        }
        mem_msg_buf = malloc_reserved_mem(g_size);
        if (NULL == mem_msg_buf){
            printk(KERN_DEBUG "[memMsgDev] malloc_reserved_mem fail.\n");
            return -3;
        }
        *ppBuf = mem_msg_buf;
    }
    msgEvtProcess = process;

    init_waitqueue_head(&mem_poll_wait);

    printk(KERN_DEBUG "memDev init successfully.\n");

/*------------huawei--------------------*/
    gFunc_TaskletHook = taskletHook;
    gFunc_softIrqHook = softIrqHook;
/*--------------------------------*/

    return 0;
}


int memDev_exit(){
    cdev_del(g_pdev);
    printk(KERN_DEBUG "[memMsgDev] cdev_del successful.\n");
    kfree(mem_msg_buf);
    unregister_chrdev_region(g_mem_dev,DEV_ID_COUNT);
    printk(KERN_DEBUG "[memMsgDev] unregister_chrdev_region successful.\n");
    printk("memDev exit successfully.\n");

/*------------hook--------------------*/
    gFunc_TaskletHook = NULL;
    gFunc_softIrqHook = NULL;
/*--------------------------------*/
    return 0;
}


ssize_t char_read(struct file *filp,char __user *buffer,size_t size, loff_t * offp){
    printk( "[memMsgDev] enter char_read().\n");
    char str[1000];
    sprintf(str," HI_SOFTIRQ = %llu\n TIMER_SOFTIRQ = %llu\n NET_TX_SOFTIRQ = %llu\n NET_RX_SOFTIRQ = %llu \n BLOCK_SOFTIRQ = %llu \n BLOCK_IOPOLL_SOFTIRQ = %llu \n TASKLET_SOFTIRQ = %llu \n SCHED_SOFTIRQ = %llu \n HRTIMER_SOFTIRQ = %llu\n RCU_SOFTIRQ = %llu \n g_iCurrentSoftirq = %llu \n", 
            g_ullSoftirqTimes[0],
            g_ullSoftirqTimes[1],
            g_ullSoftirqTimes[2],
            g_ullSoftirqTimes[3],
            g_ullSoftirqTimes[4],
            g_ullSoftirqTimes[5],
            g_ullSoftirqTimes[6],
            g_ullSoftirqTimes[7],
            g_ullSoftirqTimes[8],
            g_ullSoftirqTimes[9],
            g_iCurrentSoftirq);
    copy_to_user(buffer, str, strlen(str));
    *offp = strlen(str);
    return strlen(str) ;
}

int char_open(struct inode * nodep,struct file * filep){
    printk( "char_open() enter char_read().\n");
    return 0;
}

ssize_t char_write(struct file *filep,const char __user *buffer,size_t size,loff_t * offp){
    return 0;
}

int char_release(struct inode * nodep,struct file * filep){

    return 0;
}


/* ------------------ The MMap  ------------------ */
static unsigned char * malloc_reserved_mem(unsigned int size){
    unsigned char *p = kmalloc(size, GFP_KERNEL);
    unsigned char *tmp = p;
    unsigned int i, n;
    if (NULL == p){
        printk("Error : malloc_reserved_mem kmalloc failed!\n");
        return NULL;
    }
    
    n = size / 4096 + 1;
    if (0 == size % 4096 ){
        --n;
    }
    for (i = 0; i < n; ++i){
        SetPageReserved(virt_to_page(tmp));
        tmp += 4096;
    }
    
    return p;
}
 
int mem_mmap(struct file *filp, struct vm_area_struct *vma){
    printk("in mem_mmap\n");
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT; 
    /*unsigned long  physics = ((unsigned long )mem_msg_buf)-PAGE_OFFSET; */
    unsigned long physics =
      (unsigned long)(virt_to_phys((unsigned long)mem_msg_buf));
    unsigned long mypfn = physics >> PAGE_SHIFT + vma->vm_pgoff;
    unsigned long vmsize = vma->vm_end - vma->vm_start;
    unsigned long psize = g_size - offset;
    printk(KERN_DEBUG
           "INFO:mem_mmap() The magicnum of mem_msg_buf for mmap is %s\n",
           mem_msg_buf);        
    printk(KERN_DEBUG "INFO: mem_mmap() vm_pgoff = %u, vmsize= %u\n", vma->vm_pgoff, vmsize);
    if(vmsize > psize){
        printk(KERN_DEBUG "Error : mem_mmap() vmsize[%d] > g_size[%d] ! \n", vmsize, g_size);
        return -ENXIO;
    }
    if(remap_pfn_range(vma,vma->vm_start,mypfn,vmsize,vma->vm_page_prot))
        return -EAGAIN;
    return 0;
}

/* ---------------------------  ioctl ---------------------- */
static long mem_ioctl(struct file *pFile, unsigned int cmd, unsigned long add){
    printk(KERN_DEBUG "mem_ioctl() : enter . \n");
    switch(cmd){
    case 0:
        return msgEvtProcess();
    case 1:
    default:
        printk(KERN_DEBUG "Error: mem_ioctl() this is a unknow cmd[%d].\n", cmd);
        return -1;
    }    
}



/* --------------------------  poll --------------------- */
unsigned char mem_poll_flag = 0;

int mem_wakeup_poll(){
    printk(KERN_DEBUG "info: mem_wakeup_poll enter. \n");
    mem_poll_flag = 1;
    wake_up_interruptible(&mem_poll_wait);
    return 0;
}

static unsigned int mem_poll(struct file* file, poll_table* wait)

{
    unsigned int    mask = 0;
    printk(KERN_DEBUG "info: mem_poll enter. \n");
    poll_wait(file, &mem_poll_wait, wait);
    if (mem_poll_flag){
        mask |= POLLIN;
        mem_poll_flag = 0;
    }
    return mask;
}





/*------------huawei--------------------*/


void softIrqHook(char cInOut, struct softirq_action* pCur, struct softirq_action* pVec){
    static int iInitFlag = 0;
    if (0 == iInitFlag ){
        iInitFlag = 1;
        memset(g_ullSoftirqTimes , 0, sizeof(g_ullTaskletTimes));
    }
    if (0 == cInOut){
        g_iCurrentSoftirq = pCur - pVec;
    }
    if (1 == cInOut){
        g_iCurrentSoftirq = pCur - pVec;
        ++g_ullSoftirqTimes[g_iCurrentSoftirq];
        g_iCurrentSoftirq = 100;
    }
}
void taskletHook(char cInOut, struct tasklet_struct *pList){
}
/*---------------------------------------------------------------------------------*/
