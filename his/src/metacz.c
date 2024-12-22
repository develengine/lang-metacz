#include "metacz.h"

/* [ ] Have the scopes persist, have their offset be ~0, and trash the labels.
 * [ ] Scope jump instructions will encode relative nesting distance.
 * [ ] 
 */

cz_scope_ref_t
cz_scope_begin(cz_t *cz)
{
    ASSERT(cz->rec_funcs.count > 0);

    unsigned rec_func_i = cz->rec_funcs.count - 1;
    cz_rec_func_t *rec_func_p = cz->rec_funcs.data + rec_func_i;

    unsigned scope_n = cz->scopes.count - rec_func_p->scope_offset;

    DCK_STRETCHY_PUSH(cz->scopes, (cz_scope_t) {
        .label_offset = cz->labels.count,
        .stack_base   = cz->stack_size,
        .stack_diff   = (unsigned)-1,
    });

    return (cz_scope_ref_t) {
        .func_index = rec_func_p->func_index,
        .number     = scope_n,
    };
}

cz_label_ref_t
cz_label(cz_t *cz)
{
    ASSERT(cz->rec_funcs.count > 0);

    unsigned rec_func_i = cz->rec_funcs.count - 1;
    cz_rec_func_t *rec_func_p = cz->rec_funcs.data + rec_func_i;

    ASSERT(cz->scopes.count >= rec_func_p->scope_offset);

    unsigned scope_i = cz->scopes.count - 1;
    cz_scope_t *scope_p = cz->scopes.data + scope_i;

    unsigned label_n = cz->labels.count - scope_p->label_offset;

    DCK_STRETCHY_PUSH(cz->labels, (cz_label_t) {0});

    return (cz_label_ref_t) {
        .func_index   = rec_func_p->func_index,
        .scope_number = scope_i - rec_func_p->scope_offset,
        .number       = label_n,
    };
}

void
cz_label_link(cz_t *cz, cz_label_ref_t label_ref)
{
    ASSERT(cz->rec_funcs.count > 0);

    unsigned rec_func_i = cz->rec_funcs.count - 1;
    cz_rec_func_t *rec_func_p = cz->rec_funcs.data + rec_func_i;

    ASSERT(label_ref.func_index == rec_func_p->func_index);

    ASSERT(cz->scopes.count >= rec_func_p->scope_offset);

    unsigned scope_i = cz->scopes.count - 1;
    cz_scope_t *scope_p = cz->scopes.data + scope_i;

    ASSERT(scope_p->label_offset <= cz->labels.count);

    unsigned label_count = cz->labels.count - scope_p->label_offset;

    ASSERT(label_ref.number < label_count);

    unsigned label_i = scope_p->label_offset + label_ref.number;
    cz_label_t *label_p = cz->labels.data + label_i;

    ASSERT(label_p->is_linked == false);

    label_p->is_linked = true;
}

void
cz_scope_end(cz_t *cz)
{
    ASSERT(cz->scopes.count > 0);

    unsigned scope_i = cz->scopes.count - 1;
    cz_scope_t scope = cz->scopes.data[scope_i];

    for (unsigned label_i = scope.label_offset; label_i < cz->labels.count; ++label_i) {
        cz_label_t label = cz->labels.data[label_i];
        ASSERT(label.is_linked == true);
    }

    cz->scopes.count = scope_i;
}

cz_func_ref_t
cz_func_begin(cz_t *cz)
{
    unsigned func_i = cz->funcs.count;

    unsigned parent_i = 0;
    if (cz->rec_funcs.count != 0) {
        parent_i = cz->rec_funcs.data[cz->rec_funcs.count - 1].func_index;
    }

    DCK_STRETCHY_PUSH(cz->funcs, (cz_func_t) {
        .parent_index = parent_i,
    });

    unsigned scope_i = cz->scopes.count;
    cz_scope_begin(cz);

    DCK_STRETCHY_PUSH(cz->rec_funcs, (cz_rec_func_t) {
        .code_offset   = cz->rec.code.count,
        .input_offset  = cz->rec.inputs.count,
        .local_offset  = cz->rec.locals.count,
        .output_offset = cz->rec.outputs.count,
        .func_index    = func_i,
        .scope_offset  = scope_i,
    });

    return (cz_func_ref_t) { .index = func_i };
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

