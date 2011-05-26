/*
 *  include/linux/bfs_fs.h - BFS data structures on disk.
 *  Copyright (C) 1999 Tigran Aivazian <tigran@veritas.com>
 */

#ifndef _FILESYSTEM_BFS_STRUCTS_H_
#define _FILESYSTEM_BFS_STRUCTS_H_

#define B_OS_NAME_LENGTH 32
#define SUPER_BLOCK_MAGIC1 0x42465331
#define SUPER_BLOCK_MAGIC2 0xDD121031
#define SUPER_BLOCK_MAGIC3 0x15B6830E

#define BFS_CLEAN 0x434C454E
#define BFS_DIRTY 0x44495254
#define BEFS_BTREE_MAGIC 0x69F6C2E8

#define off_t uint64_t
#define inode_addr bfs_block_run
#define bigtime_t uint64_t

#define INODE_MAGIC 0x3BBE0AD9

#define NUM_DIRECT_BLOCKS 12

#define SHORT_SYMLINK_NAME_LENGTH 144

#define SD_DATA(sd) (void *)((char *)sd + sizeof(*sd) + (sd->name_size-sizeof(sd->name)))

/*
Inode addresses have len = 1
*/
struct bfs_block_run
{
    uint32_t allocation_group;
    uint16_t start;
    uint16_t len;
} __attribute__ ((packed));

struct bfs_sblock
{
    char name[B_OS_NAME_LENGTH];
    uint32_t magic1;
    uint32_t fs_byte_order;

    uint32_t block_size;
    uint32_t block_shift;
    
    off_t num_blocks;
    off_t used_blocks;

    uint32_t inode_size;

    uint32_t magic2;
    uint32_t blocks_per_ag;
    uint32_t ag_shift;
    uint32_t num_ags;

    uint32_t flags;

    bfs_block_run log_blocks;
    off_t log_start;
    off_t log_end;

    uint32_t magic3;
    inode_addr root_dir;
    inode_addr indices;

    uint32_t pad[8];
}  __attribute__ ((packed));

struct bfs_data_stream
{
    bfs_block_run direct[NUM_DIRECT_BLOCKS];
    off_t max_direct_range;
    bfs_block_run indirect;
    off_t max_indirect_range;
    bfs_block_run double_indirect;
    off_t max_double_indirect_range;
    off_t size;
}  __attribute__ ((packed));

struct bfs_inode
{
    uint32_t magic1;
    inode_addr inode_num;
    uint32_t uid;
    uint32_t gid;
    uint32_t mode;
    uint32_t flags;
    bigtime_t create_time;
    bigtime_t last_modified_time;
    inode_addr parent;
    inode_addr attributes;
    uint32_t type;

    uint32_t inode_size;
    uint32_t etc;

    bfs_data_stream data;
    uint32_t pad[4];
    uint32_t small_data[1];
} __attribute__ ((packed));

struct bfs_btree_root
{
    uint32_t magic;
    uint32_t node_size;
    uint32_t max_depth;
    uint32_t data_type;
    off_t root_node_pointer;
    off_t free_node_pointer;
    off_t max_size;
} __attribute__ ((packed));

struct bfs_btree_node
{
    off_t left;
    off_t right;
    off_t overflow;
    uint16_t all_key_count;
    uint16_t all_key_length;
} __attribute__ ((packed));

struct bfs_small_data
{
    uint32_t type;
    uint16_t name_size;
    uint16_t data_size;
    char name[1];
} __attribute__ ((packed));

enum inode_flags {
    BEFS_INODE_IN_USE = 0x00000001,
    BEFS_ATTR_INODE = 0x00000004,
    BEFS_INODE_LOGGED = 0x00000008,
    BEFS_INODE_DELETED = 0x00000010,
    BEFS_LONG_SYMLINK = 0x00000040,
    BEFS_PERMANENT_FLAG = 0x0000ffff,
    BEFS_INODE_NO_CREATE = 0x00010000,
    BEFS_INODE_WAS_WRITTEN = 0x00020000,
    BEFS_NO_TRANSACTION = 0x00040000
};

enum btree_types {
    BTREE_STRING_TYPE = 0,
    BTREE_INT32_TYPE = 1,
    BTREE_UINT32_TYPE = 2,
    BTREE_INT64_TYPE = 3,
    BTREE_UINT64_TYPE = 4,
    BTREE_FLOAT_TYPE = 5,
    BTREE_DOUBLE_TYPE = 6
};

#endif
