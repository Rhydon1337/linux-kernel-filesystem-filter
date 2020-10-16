# linux-kernel-filesystem-filter
## TL;DR
Linux kernel filesystem filter driver.

Depends on kallsyms which means it will work on most of Linux distros except embedded systems which probably compiled without it.

Tested on Linux kernel version: 4.19.91.

## How it works
Windows uses a driver stack model that able to use layers of drivers and therfore it utlizes this mechanism for the concept of filter drivers.
The usage of filter drivers are needed for a lot of reasons, some of them related to the need of monitoring\blocking to calls.
However, linux doesn't have this mechanism.
Then I have chosen to implement filesysten filter, because in my opinion its the most important one.

Based on "UnderStanding The Linux Kernel 3rd Edition" and some kernel browsing:

Let’s describe the operation of the sys_open() function. It performs the following steps:

* Invokes getname() to read the file pathname from the process address space.
* Invokes get_unused_fd() to find an empty slot in current->files->fd. The corresponding index (the new file descriptor) is stored in the fd local variable.
* Invokes the filp_open() function, passing as parameters the pathname, the access mode flags, and the permission bit mask.
    * Invokes open_namei(), passing to it the pathname, the modified access mode flags, and the address of a local nameidata data structure.
        * Invokes the dentry_open() function, passing to it the addresses of the dentry object and the mounted filesystem object located by the lookup operation, and the access mode flags.

Lets dig into dentry_open():

1. Allocates a new file object.
2. Initializes the f_flags and f_mode fields of the file object according to the access mode flags passed to the open() system call.
3. Initializes the f_dentry and f_vfsmnt fields of the file object according to the addresses of the dentry object and the mounted filesystem object passed as parameters.
4. Sets the f_op field to the contents of the i_fop field of the corresponding inode object. This sets up all the methods for future file operations.
5. Inserts the file object into the list of opened files pointed to by the s_files field of the filesystem’s superblock.
6. If the open method of the file operations is defined, the function invokes it.
7. Invokes file_ra_state_init() to initialize the read-ahead data structures.
8. If the O_DIRECT flag is set, it checks whether direct I/O operations can be performed on the file.
9. Returns the address of the file object.

In dentry_open the file object is allocated and initialized, dentry reiecve as parameter the dentry object and the mounted filesystem object 
and sets the f_op field to the contents of the i_fop field of the corresponding inode object.

the f_op field is using for all file operations which are interesting us.

f->f_op = fops_get(inode->i_fop);
f_op initialized by fops_get(inode->i_fop);

Every filesystem must supplies for each inode a file_operations(i_fop), this is part a of the abstraction of Linux VFS.

All we need to do is to set our callback in the matching filesystem's file_operations struct.

## Limitations
Everyone inside the kernel is able to lookup and edit the inode itself and change the pointer to the filesystem's file_operations struct.

## Usage
Although that its just a template for filter drivers, current master branch prints every file name which opens in the system.

cd linux-kernel-filesystem-filter/
make
insmod filesystem_filter_driver.ko

DONE!!!