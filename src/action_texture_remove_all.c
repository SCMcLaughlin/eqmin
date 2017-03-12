
#include "actions.h"

#define make_ext(a, b, c) ((a) | (b) << 8 | (c) << 16)

static int tex_rm_transfer_file(Pfs* src, Pfs* dst, uint32_t index, Buffer* name)
{
    const byte* data;
    uint32_t inflatedLen;
    uint32_t deflatedLen;
    int rc;
    
    data = pfs_get_compressed_raw_by_index(src, index, &inflatedLen, &deflatedLen);
    
    if (!data)
    {
        fprintf(stderr, "Error: could not retrieve internal PFS file '%s'\n", buf_str(name));
        return ERR_CouldNotOpen;
    }
    
    rc = pfs_put_compressed(dst, buf_str(name), buf_length(name), data, inflatedLen, deflatedLen);
    
    if (rc)
    {
        fprintf(stderr, "Error: failed to copy internal PFS file '%s', errcode %i\n", buf_str(name), rc);
        return rc;
    }
    
    return ERR_None;
}

static int tex_rm_handle(Opts* opt, Buffer* path)
{
    Pfs src, dst;
    uint32_t i = 0;
    uint32_t rmCount = 0;
    int rc;
    
    pfs_init(&dst);
    rc = pfs_open(&src, path);
    
    if (rc)
    {
        fprintf(stderr, "Error: could not open '%s', errcode %i\n", buf_str(path), rc);
        goto finish;
    }
    
    for (;;)
    {
        Buffer* name = pfs_get_name(&src, i++);
        const char* str;
        uint32_t ext;
        uint32_t len;
        
        if (!name) break;
        
        len = buf_length(name);
        str = buf_str(name);
        
        /* Must be at least 4 characters for the dot and 3-letter extension */
        if (len < 4)
            continue;
        
        str += len - 3;
        ext = make_ext(str[0], str[1], str[2]);
        
        /* Blacklist of extensions */
        switch (ext)
        {
        case make_ext('b', 'm', 'p'):
        case make_ext('d', 'd', 's'):
        case make_ext('t', 'g', 'a'):
        case make_ext('p', 'n', 'g'):
        case make_ext('j', 'p', 'g'):
            rmCount++;
            continue;
        
        default:
            break;
        }
        
        rc = tex_rm_transfer_file(&src, &dst, i - 1, name);
        
        if (rc)
        {
            fprintf(stderr, "Error: failed to copy internal PFS file '%s', errcode %i\n", buf_str(name), rc);
            goto finish;
        }
    }
    
    if (rmCount > 0)
    {
        rc = pfs_save_as(&dst, pfs_path(&src));
        
        if (rc)
        {
            fprintf(stderr, "Error: failed to save '%s', errcode %i\n", pfs_path(&src), rc);
            goto finish;
        }
    }
    
    if (!opt_flag(opt, OPT_Quiet))
        printf("Removed %u textures from '%s'\n", rmCount, buf_str(path));
    
finish:
    pfs_close(&src);
    pfs_close(&dst);
    return rc;
}

#undef make_ext

int action_texture_remove_all(Opts* opt)
{
    uint32_t i, n;
    
    n = opt->argCount;
    
    if (n == 0)
    {
        fprintf(stderr, "Error: no target files provided to texture-remove-all, aborting\n");
        return ERR_Generic;
    }
    
    for (i = 0; i < n; i++)
    {
        Buffer* path = opt->args[i];
        int rc = tex_rm_handle(opt, path);
        
        if (rc) return rc;
    }
    
    return ERR_None;
}
