
#ifndef UTIL_HASH_TBL_H
#define UTIL_HASH_TBL_H

#include "define.h"
#include "hash.h"
#include "util_alloc.h"
#include "util_buffer.h"
#include "structs_container.h"

void tbl_init_size(HashTbl* tbl, uint32_t elemSize);
#define tbl_init(tbl, type) tbl_init_size((tbl), sizeof(type))
void tbl_deinit(HashTbl* tbl, ElemCallback dtor);

int tbl_set_str(HashTbl* tbl, const char* key, uint32_t len, const void* value);
int tbl_set_buf(HashTbl* tbl, Buffer* key, const void* value);
int tbl_set_int(HashTbl* tbl, int64_t key, const void* value);
#define tbl_set_ptr(tbl, ptr, val) tbl_set_int((tbl), (intptr_t)(ptr), (val))

int tbl_update_str(HashTbl* tbl, const char* key, uint32_t len, const void* value);
int tbl_update_buf(HashTbl* tbl, Buffer* key, const void* value);
int tbl_update_int(HashTbl* tbl, int64_t key, const void* value);
#define tbl_update_ptr(tbl, ptr, val) tbl_update_int((tbl), (intptr_t)(ptr), (val))

void* tbl_get_str_raw(HashTbl* tbl, const char* key, uint32_t len);
#define tbl_get_str(tbl, key, len, type) (type*)tbl_get_str_raw((tbl), (key), (len))
#define tbl_get_str_literal(tbl, key, type) tbl_get_str(tbl, key, sizeof(key) - 1, type)
void* tbl_get_int_raw(HashTbl* tbl, int64_t key);
#define tbl_get_int(tbl, key, type) (type*)tbl_get_int_raw((tbl), (key))
#define tbl_get_ptr(tbl, ptr, type) tbl_get_int(tbl, (intptr_t)(ptr), type)
#define tbl_has_int(tbl, key) (tbl_get_int_raw((tbl), (key)) != NULL)

Buffer* tbl_get_key_str(HashTbl* tbl, const char* key, uint32_t len);

int tbl_remove_str(HashTbl* tbl, const char* key, uint32_t len);
int tbl_remove_int(HashTbl* ptr, int64_t key);
#define tbl_remove_ptr(tbl, ptr) tbl_remove_int((tbl), (intptr_t)(ptr))

void tbl_for_each(HashTbl* tbl, ElemCallback func);
void tbl_for_each_with_tbl(HashTbl* tbl, ContainerElemCallback func);

#endif/*UTIL_HASH_TBL_H*/
