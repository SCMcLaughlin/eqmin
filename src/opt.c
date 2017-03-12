
#include "opt.h"

static int opt_handle(Opts* opt, OptHandler* handlers, const char* arg)
{
    for (;;)
    {
        const char* name = handlers->name;
        
        if (name == NULL)
            break;
        
        if (strcmp(arg, name) == 0)
        {
            opt->flags |= (1 << handlers->bit);
            return ERR_None;
        }
        
        handlers++;
    }
    
    fprintf(stderr, "Error: unknown option '%s%s'\n\n", (arg[1] == 0) ? "-" : "--", arg);
    return ERR_UnknownArg;
}

int opt_process(Opts* opt, OptHandler* handlers, int argc, const char** argv)
{
    char shortOpt[2] = {0, 0};
    int i, rc;
    Buffer* buf;
    
    memset(opt, 0, sizeof(Opts));
    
    for (i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        
        if (!arg) break;
        
        if (arg[0] == '-')
        {
            /* Long option? */
            if (arg[1] == '-')
            {
                rc = opt_handle(opt, handlers, &arg[2]);
                if (rc) return rc;
                
                continue;
            }
            
            /* Short option(s) */
            for (;;)
            {
                arg++;
                
                if (*arg == 0)
                    break;
                
                shortOpt[0] = *arg;
                
                rc = opt_handle(opt, handlers, shortOpt);
                if (rc) return rc;
            }
            
            continue;
        }
        
        /* Arguments */
        if (bit_is_pow2_or_zero(opt->argCount))
        {
            uint32_t cap = (opt->argCount == 0) ? 1 : (opt->argCount * 2);
            Buffer** args = realloc_array_type(opt->args, cap, Buffer*);
            
            if (!args) return ERR_OutOfMemory;
            
            opt->args = args;
        }
        
        buf = buf_create(arg, strlen(arg));
        
        if (!buf) return ERR_OutOfMemory;
        
        opt->args[opt->argCount++] = buf;
    }
    
    return ERR_None;
}

void opt_free(Opts* opt)
{
    if (opt->args)
    {
        uint32_t i;
        
        for (i = 0; i < opt->argCount; i++)
        {
            buf_drop(opt->args[i]);
        }
        
        free(opt->args);
    }
}
