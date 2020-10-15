#include "filesystem_filter.h"

#include <linux/kallsyms.h>
#include <linux/slab.h>

struct file_operations g_original_fops;

void initialize_filter(char* filesystem_file_operation_name) {
    unsigned long prev_cr0 = read_cr0();
    struct file_operations* original_filesystem_fops = (struct file_operations*)kallsyms_lookup_name(filesystem_file_operation_name);
    if (NULL == original_filesystem_fops) {
        printk("Unable to find the original filsystem fops\n");
        return;
    }

    g_original_fops.open = (void*)original_filesystem_fops->open;
    g_original_fops.llseek = (void*)original_filesystem_fops->llseek;
    g_original_fops.read_iter = (void*)original_filesystem_fops->read_iter;
    g_original_fops.write_iter = (void*)original_filesystem_fops->write_iter;
    g_original_fops.release = (void*)original_filesystem_fops->release;
    g_original_fops.fsync = (void*)original_filesystem_fops->fsync;

    write_cr0(prev_cr0 & (~ 0x10000));

    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->open, (unsigned long)file_open);
    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->llseek, (unsigned long)llseek);
    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->read_iter, (unsigned long)file_read_iter);
    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->write_iter, (unsigned long)file_write_iter);
    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->release, (unsigned long)release_file);
    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->fsync, (unsigned long)sync_file);
    write_cr0(prev_cr0);
    __sync_synchronize();
}

loff_t llseek(struct file *file, loff_t offset, int whence) {
    return g_original_fops.llseek(file, offset, whence);
}

int file_open(struct inode * inode, struct file * filp) {
    char* temp_buffer = NULL;
    char* buffer = kmalloc(PATH_MAX, GFP_KERNEL);
    if (NULL != buffer) {
        temp_buffer = dentry_path_raw(filp->f_path.dentry, buffer, PATH_MAX);
        if (!IS_ERR(temp_buffer)) {
            printk(KERN_INFO "Open file: %s\n", temp_buffer);
        }
        kfree(buffer);
    }
    return g_original_fops.open(inode, filp);
}

ssize_t file_read_iter(struct kiocb *iocb, struct iov_iter *to) {
    return g_original_fops.read_iter(iocb, to);
}

ssize_t file_write_iter(struct kiocb *iocb, struct iov_iter *from) {
    return g_original_fops.write_iter(iocb, from);
}

int release_file(struct inode *inode, struct file *filp) {
    return g_original_fops.release(inode, filp);
}

int sync_file(struct file *file, loff_t start, loff_t end, int datasync) {
    return g_original_fops.fsync(file, start, end, datasync);
}