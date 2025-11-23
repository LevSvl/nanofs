/*
 * This header only contains set of informational macros.
 * It should be directly included into the all modules
 * using macros defined below
 */
#ifndef NANOFS_INFO_H
#define NANOFS_INFO_H

/*
 * This macro is used for low level functions
 * that work with storage device to read and
 * write filesystem's blocks
 */
#define fs_low_level_storage

/*
 * This macro is used for low level functions
 * that work with inmem inodes that is why
 * such functions must care about state 
 * of inmem inodes they use
 */
#define fs_low_level_inode

#endif // NANOFS_INFO_H
