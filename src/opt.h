
#ifndef OPT_H
#define OPT_H

#include "define.h"
#include "bit.h"
#include "structs_opt.h"
#include "util_buffer.h"
#include "util_alloc.h"

int opt_process(Opts* opt, OptHandler* handlers, int argc, const char** argv);
void opt_free(Opts* opt);

#define opt_flag(opt, bit) (((opt)->flags & (1 << (bit))) != 0)

#endif/*OPT_H*/
