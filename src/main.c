
#include "define.h"
#include "opt.h"
#include "actions.h"

int main(int argc, const char** argv)
{
    Opts opts;
    int rc;
    
    static OptHandler optHandlers[] = {
        { "t",                  OPT_TextureRemoveAll },
        { "texture-remove-all", OPT_TextureRemoveAll },
        { "w",                  OPT_WeaponRemoveAll  },
        { "weapon-remove-all",  OPT_WeaponRemoveAll  },
        { "q",                  OPT_Quiet            },
        { "quiet",              OPT_Quiet            },
        { "h",                  OPT_Help             },
        { "help",               OPT_Help             },
        { NULL,                 0                    }
    };
    
    rc = opt_process(&opts, optHandlers, argc, argv);
    
    if (rc && rc != ERR_UnknownArg)
    {
        fprintf(stderr, "Error: could not process program arguments, errcode %i\n", rc);
        goto finish;
    }
    
    if (rc == ERR_UnknownArg || opts.flags == 0 || opt_flag(&opts, OPT_Help))
    {
        printf(
          /*                              *                                                  */
            "Usage: eqmin [options] [target files]\n\n"
            "Options:\n"
            "    -t, --texture-remove-all Removes all textures from the target PFS files. Any\n"
            "                             zones, objects, weapons or mob models contained\n"
            "                             within will be rendered as pure-white blobs.\n\n"
            "    -w, --weapon-remove-all  Removes all weapon models from the target PFS\n"
            "                             files.\n\n"
            "    -q, --quiet              Avoids sending status messages to stdout.\n\n"
            "    -h, --help               Displays this message.\n\n"
        );
        goto finish;
    }
    
    if (opt_flag(&opts, OPT_TextureRemoveAll))
    {
        rc = action_texture_remove_all(&opts);
        if (rc) goto finish;
    }
    
    if (opt_flag(&opts, OPT_WeaponRemoveAll))
    {
        rc = action_weapon_remove_all(&opts);
        if (rc) goto finish;
    }
    
finish:
    opt_free(&opts);
    return rc;
}
