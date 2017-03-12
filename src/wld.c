
#include "wld.h"
#include "pfs.h"

#define WLD_SIGNATURE   0x54503d02
#define WLD_VERSION1    0x00015500
#define WLD_VERSION2    0x1000C800

static void wld_init(Wld* wld, Buffer* file)
{
    array_init(&wld->fragsByIndex, Frag*);
    tbl_init(&wld->fragsByNameRef, Frag*);
    
    wld->strings = NULL;
    wld->stringsLength = 0;
    wld->data = file;
}

void wld_process_string(void* str, uint32_t len)
{
    static const byte hash[] = {0x95, 0x3A, 0xC5, 0x2A, 0x95, 0x7A, 0x95, 0x6A};
    byte* data = (byte*)str;
    uint32_t i;
    
    for (i = 0; i < len; i++)
    {
        data[i] = data[i] ^ hash[i & 7];
    }
}

int wld_open(Wld* wld, Buffer* file)
{
    byte* data = buf_writable(file);
    uint32_t len = buf_length(file);
    WldHeader* h = (WldHeader*)data;
    uint32_t p = sizeof(WldHeader);
    uint32_t ver;
    int stringsLength;
    int nameRef;
    Frag* frag;
    uint32_t n;
    uint32_t i;
    
    wld_init(wld, file);
    
    if (p > len)
        goto oob;
    
    if (h->signature != WLD_SIGNATURE)
        return ERR_Invalid;
    
    ver = h->version & 0xfffffffe;
    
    if (ver != WLD_VERSION1 && ver != WLD_VERSION2)
        return ERR_Invalid;
    
    wld->version = (ver == WLD_VERSION1) ? 1 : 2;
    
    stringsLength = -((int)h->stringsLength);
    wld->strings = (char*)&data[p];
    wld->stringsLength = stringsLength;
    
    p += h->stringsLength;
    
    if (p > len)
        goto oob;
    
    wld_process_string(wld->strings, -stringsLength);
    
    frag = NULL;
    if (!array_push_back(&wld->fragsByIndex, (void*)&frag))
        return ERR_OutOfMemory;
    
    n = h->fragCount;
    
    for (i = 0; i < n; i++)
    {
        frag = (Frag*)&data[p];
        
        p += sizeof(Frag);
        
        if (p > len)
            goto oob;
        
        p += frag->length - sizeof(uint32_t);
        
        if (p > len)
            goto oob;
        
        if (!array_push_back(&wld->fragsByIndex, (void*)&frag))
            return ERR_OutOfMemory;
        
        nameRef = frag->nameRef;
        
        if (nameRef < 0 && nameRef > stringsLength)
        {
            int rc = tbl_set_int(&wld->fragsByNameRef, nameRef, (void*)&frag);
            
            if (rc && rc != ERR_Again)
                return rc;
        }
    }
    
    return ERR_None;
    
oob:
    return ERR_OutOfBounds;
}

void wld_close(Wld* wld)
{
    array_deinit(&wld->fragsByIndex, NULL);
    tbl_deinit(&wld->fragsByNameRef, NULL);
    
    if (wld->data)
    {
        buf_drop(wld->data);
        wld->data = NULL;
    }
}

const char* wld_frag_name(Wld* wld, Frag* frag)
{
    return wld_name_by_ref(wld, frag->nameRef);
}

const char* wld_name_by_ref(Wld* wld, int nameRef)
{
    if (nameRef < 0 && nameRef > wld->stringsLength)
        return wld->strings - nameRef;
    
    return NULL;
}

Frag* wld_frag_by_ref(Wld* wld, int ref)
{
    Frag** ptr = NULL;
    
    if (ref > 0)
    {
        if (ref < (int)array_count(&wld->fragsByIndex))
            ptr = array_get(&wld->fragsByIndex, ref, Frag*);
    }
    else
    {
        ptr = tbl_get_int(&wld->fragsByNameRef, ref, Frag*);
    }
    
    return (ptr) ? *ptr : NULL;
}

Frag* wld_frag_by_index(Wld* wld, uint32_t index)
{
    return array_get(&wld->fragsByIndex, index + 1, Frag);
}

int wld_save_into(Wld* wld, Pfs* pfs, const char* name, uint32_t namelen)
{
    Buffer* data = wld->data;
    
    wld_process_string(wld->strings, -wld->stringsLength);
    
    return pfs_put(pfs, name, namelen, buf_data(data), buf_length(data));
}
