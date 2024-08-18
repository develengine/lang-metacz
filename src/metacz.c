#include "metacz.h"

cz_scope_ref_t
cz_scope_begin(cz_t *cz)
{
    unsigned scope_index = cz->scopes.count;

    DCK_STRETCHY_PUSH(cz->scopes, (cz_scope_t) {
        .label_offset = cz->scope_labels.count,
        .stack_base   = cz->stack_size,
        .stack_diff   = (unsigned)-1,
    });

    return (cz_scope_ref_t) { .index = scope_index };
}

cz_label_ref_t
cz_label(cz_t *cz, cz_scope_ref_t scope_ref)
{
    // We want to be able to free the unnecesarry labels as son as posible.
    // But this can cause reuse bugs, maybe a flat bump thing is better in this case.
    // Should it go beyond function scopes though?
    // In fact, labels only really make sense when dealing with the scopes of the generator code.
    // This may be the case with all the other object referecnces as well.
}

void
cz_scope_end(cz_t *cz)
{
    
}

cz_func_ref_t
cz_func_begin(cz_t *cz)
{
    unsigned func_index = cz->funcs.count;

    unsigned parent_index = {0};
    if (cz->rec_funcs.count != 0) {
        parent_index = cz->rec_funcs.data[cz->rec_funcs.count - 1].func_index;
    }

    DCK_STRETCHY_PUSH(cz->funcs, (cz_func_t) {
        .parent_index = parent_index,
    });

    DCK_STRETCHY_PUSH(cz->rec_funcs, (cz_rec_func_t) {
        .code_offset   = cz->rec.code.count,
        .input_offset  = cz->rec.inputs.count,
        .local_offset  = cz->rec.locals.count,
        .output_offset = cz->rec.outputs.count,
        .func_index    = func_index,
        .scope_index   = cz_scope_begin(cz).index,
    });

    return (cz_func_ref_t) { .index = func_index };
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

