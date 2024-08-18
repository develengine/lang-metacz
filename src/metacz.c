#include "metacz.h"

cz_func_ref_t
cz_func_begin(cz_t *cz)
{
    cz_func_ref_t func_ref = { .index = cz->funcs.count };

    cz_func_ref_t parent_ref = {0};
    if (cz->rec_funcs.count != 0) {
        parent_ref = cz->rec_funcs.data[cz->rec_funcs.count - 1].func_ref;
    }

    DCK_STRETCHY_PUSH(cz->funcs, (cz_func_t) {
        .parent_ref = parent_ref,
    });

    DCK_STRETCHY_PUSH(cz->rec_funcs, (cz_rec_func_t) {
        .code_offset   = cz->rec.code.count,
        .input_offset  = cz->rec.inputs.count,
        .local_offset  = cz->rec.locals.count,
        .output_offset = cz->rec.outputs.count,
        .func_ref      = func_ref,
    });
}

void
cz_func_end(cz_t *cz)
{
}

void
cz_abort_if_error(cz_t *cz)
{
    if (cz->error[0]) {
        fprintf(stderr, "%s\nABORTING...\n", cz->error);
        exit(1);
    }
}

