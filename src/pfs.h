
#ifndef PFS_H
#define PFS_H

#include "define.h"
#include "structs_pfs.h"
#include "util_container.h"
#include "util_alloc.h"
#include "crc.h"
#include <zlib.h>

void pfs_init(Pfs* pfs);
int pfs_open(Pfs* pfs, Buffer* path);
void pfs_close(Pfs* pfs);

#define pfs_path(pfs) (buf_str((pfs)->path))

#define pfs_is_open(pfs) ((pfs->raw) != NULL)

int pfs_save(Pfs* pfs);
int pfs_save_as(Pfs* pfs, const char* path);

Buffer* pfs_get(Pfs* pfs, const char* name, uint32_t len);
Buffer* pfs_get_by_index(Pfs* pfs, uint32_t index);
int pfs_put(Pfs* pfs, const char* name, uint32_t namelen, const void* data, uint32_t datalen);

const byte* pfs_get_compressed_raw_by_index(Pfs* pfs, uint32_t index, uint32_t* inflatedLen, uint32_t* deflatedLen);
const byte* pfs_get_compressed_raw(Pfs* pfs, const char* name, uint32_t len, uint32_t* inflatedLen, uint32_t* deflatedLen);
int pfs_put_compressed(Pfs* pfs, const char* name, uint32_t namelen, const void* data, uint32_t inflatedLen, uint32_t deflatedLen);

Buffer* pfs_get_name(Pfs* pfs, uint32_t index);

#endif/*PFS_H*/
