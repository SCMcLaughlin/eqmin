
#include "actions.h"

static int wep_rm_handle_wld(Opts* opt, Wld* wld, Pfs* pfs, Buffer* path, Buffer* wldName)
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
    }
    
    if (!opt_flag(opt, OPT_Quiet))
        printf("Removed %u weapon models from '%s' internal file '%s'\n", rmCount, buf_str(path), buf_str(wldName));
    
    return ERR_None;
}

static int wep_rm_handle_s3d(Opts* opt, Buffer* path)
{
    Pfs pfs;
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
        
        rc = wep_rm_handle_wld(opt, &wld, &pfs, path, name);
        wld_close(&wld);
        
        if (rc) goto finish;
    }
    
finish:
    pfs_close(&pfs);
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
