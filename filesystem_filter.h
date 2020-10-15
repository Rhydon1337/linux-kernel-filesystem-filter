#pragma once

#include <linux/fs.h>

void initialize_filter(char* filesystem_file_operation_name);

loff_t llseek(struct file *file, loff_t offset, int whence);

int file_open(struct inode * inode, struct file * filp);

ssize_t file_read_iter(struct kiocb *iocb, struct iov_iter *to);

ssize_t file_write_iter(struct kiocb *iocb, struct iov_iter *from);

int release_file(struct inode *inode, struct file *filp);

int sync_file(struct file *file, loff_t start, loff_t end, int datasync);