
#include "actions.h"

static int wep_rm_handle_wld(Wld* wld, Pfs* pfs, Buffer* path, Buffer* wldName, uint32_t* outCount)
{
    char* strings = wld->strings;
    const char* stringsEnd = strings + (-wld->stringsLength);
    uint32_t rmCount = 0;
    
    while (strings < stringsEnd)
    {
        uint32_t len = strlen(strings);
        
        if (len > 3 && strings[0] == 'I' && strings[1] == 'T' && isdigit(strings[2]) && strcmp(strings + len - (sizeof("_ACTORDEF") - 1), "_ACTORDEF") == 0)
        {
            strings[0] = 'Z';
            strings[1] = 'Z';
            rmCount++;
        }
        
        strings += len + 1;
    }
    
    if (rmCount > 0)
    {
        int rc = wld_save_into(wld, pfs, buf_str(wldName), buf_length(wldName));
        
        if (rc)
        {
            fprintf(stderr, "Error: could not save edited '%s' into '%s', errcode %i\n", buf_str(wldName), buf_str(path), rc);
            return rc;
        }
        
        *outCount += rmCount;
    }
    
    return ERR_None;
}

static int wep_rm_handle_s3d(Opts* opt, Buffer* path)
{
    Pfs pfs;
    uint32_t rmCount = 0;
    uint32_t i = 0;
    int rc;
    
    rc = pfs_open(&pfs, path);
    
    if (rc)
    {
        fprintf(stderr, "Error: could not open '%s', errcode %i\n", buf_str(path), rc);
        goto finish;
    }
    
    for (;;)
    {
        Buffer* name = pfs_get_name(&pfs, i++);
        Buffer* file;
        const char* str;
        uint32_t len;
        Wld wld;
        
        if (!name) break;
        
        len = buf_length(name);
        str = buf_str(name);
        
        /* Must be at least 4 characters for the dot and 3-letter extension */
        if (len < 4)
            continue;
        
        str += len - 3;
        
        if (strcmp(str, "wld") != 0)
            continue;
        
        file = pfs_get(&pfs, buf_str(name), buf_length(name));
        
        if (!file)
        {
            fprintf(stderr, "Error: could not retrieve internal PFS file '%s'\n", buf_str(name));
            rc = ERR_CouldNotOpen;
            goto finish;
        }
        
        rc = wld_open(&wld, file);
        
        if (rc)
        {
            fprintf(stderr, "Error: could not open internal WLD file '%s' from '%s', errcode %i\n", buf_str(name), buf_str(path), rc);
            goto finish;
        }
        
        rc = wep_rm_handle_wld(&wld, &pfs, path, name, &rmCount);
        wld_close(&wld);
        
        if (rc) goto finish;
    }
    
    if (rmCount > 0)
    {
        rc = pfs_save(&pfs);
        
        if (rc)
        {
            fprintf(stderr, "Error: failed to save '%s', errcode %i\n", pfs_path(&pfs), rc);
            goto finish;
        }
    }
    
    if (!opt_flag(opt, OPT_Quiet))
        printf("Removed %u weapon models from '%s'\n", rmCount, buf_str(path));
    
finish:
    pfs_close(&pfs);
    return rc;
}

static int wep_rm_eqg_blacklist(Pfs* pfs, Buffer* name)
{
    const char* str = buf_str(name);
    uint32_t len = buf_length(name);
    
    (void)pfs;
    
    if (len < 6)
        return false;
    
    return !(str[0] == 'i' && str[1] == 't' && isdigit(str[2]) && (strcmp(str + len - 3, "mod") == 0 || strcmp(str + len - 3, "mds") == 0));
}

static int wep_rm_handle_eqg(Opts* opt, Buffer* path)
{
    Pfs src, dst;
    uint32_t rmCount;
    int rc;
    
    pfs_init(&dst);
    rc = pfs_open(&src, path);
    
    if (rc)
    {
        fprintf(stderr, "Error: could not open '%s', errcode %i\n", buf_str(path), rc);
        goto finish;
    }
    
    rc = pfs_conditional_file_transfer(&src, &dst, wep_rm_eqg_blacklist, NULL, &rmCount);
    if (rc) goto finish;
    
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
        printf("Removed %u weapon models from '%s'\n", rmCount, buf_str(path));
    
finish:
    pfs_close(&src);
    pfs_close(&dst);
    return rc;
}

int wep_rm_handle(Opts* opt, Buffer* path)
{
    const char* str = buf_str(path);
    uint32_t len = buf_length(path);
    int rc;
    
    if (len < 4) return ERR_None;
    
    str += len - 3;
    
    if (strcmp(str, "s3d") == 0)
    {
        rc = wep_rm_handle_s3d(opt, path);
    }
    else if (strcmp(str, "eqg") == 0)
    {
        rc = wep_rm_handle_eqg(opt, path);
    }
    else
    {
        fprintf(stderr, "Warning: expected target file with a '.s3d' or '.eqg' extension, got '%s'\n", buf_str(path));
        rc = ERR_None;
    }
    
    return rc;
}

int action_weapon_remove_all(Opts* opt)
{
    uint32_t i, n;
    
    n = opt->argCount;
    
    if (n == 0)
    {
        fprintf(stderr, "Error: no target files provided to weapon-remove-all, aborting\n");
        return ERR_Generic;
    }
    
    for (i = 0; i < n; i++)
    {
        Buffer* path = opt->args[i];
        int rc = wep_rm_handle(opt, path);
        
        if (rc) return rc;
    }
    
    return ERR_None;
}
