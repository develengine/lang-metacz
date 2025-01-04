#ifndef CZ2VM_H_
#define CZ2VM_H_

#include "utils.h"
#include "vm.h"
#include "cz.h"

void
cz2vm_compile(cz_t *cz, cz_func_t func, vm_mem_buf_t *code);

#endif // CZ2VM_H_

#ifdef CZ2VM_IMPL
#undef CZ2VM_IMPL

#define CZ2VM_PROC_ALIGNMENT (sizeof(ptrdiff_t))

typedef struct
{
    cz_type_t type;
    ptrdiff_t prev_sp;
    ptrdiff_t size;
    ptrdiff_t offset;
} cz2vm_object_t;

typedef UTILS_STRETCHY_T (cz2vm_object_t, unsigned) cz2vm_objects_t;

typedef struct
{
    cz_scope_t id;
    unsigned eval_stack_bottom;
    unsigned label_offset;
    ptrdiff_t sp_bottom;

    bool is_result_set;
    unsigned result_offset;
    unsigned result_count;
    unsigned result_sp;
} cz2vm_scope_t;

typedef UTILS_STRETCHY_T (cz2vm_scope_t, unsigned) cz2vm_scopes_t;

typedef struct
{
    cz_label_t id;
    ptrdiff_t position; 
} cz2vm_label_t;

typedef UTILS_STRETCHY_T (cz2vm_label_t, unsigned) cz2vm_labels_t;

typedef struct
{
    cz_scope_t scope_id;
    cz_label_t label_id;
    ptrdiff_t jmp_position;
} cz2vm_label_patch_t;

typedef UTILS_STRETCHY_T (cz2vm_label_patch_t, unsigned) cz2vm_label_patches_t;

typedef struct
{
    cz_scope_t scope_id;
    ptrdiff_t jmp_position;
} cz2vm_scope_patch_t;

typedef UTILS_STRETCHY_T (cz2vm_scope_patch_t, unsigned) cz2vm_scope_patches_t;

typedef struct
{
    cz_var_t id;
    cz_type_t type;
    ptrdiff_t offset;
    ptrdiff_t size;
} cz2vm_var_t;

typedef UTILS_STRETCHY_T (cz2vm_var_t, unsigned) cz2vm_vars_t;

typedef struct
{
    cz_var_t id;
    cz_type_t type;
    ptrdiff_t offset;
    ptrdiff_t size;
} cz2vm_in_t;

typedef UTILS_STRETCHY_T (cz2vm_in_t, unsigned) cz2vm_ins_t;

typedef struct
{
    cz_type_t type;
    ptrdiff_t offset;
    ptrdiff_t size;
} cz2vm_res_t;

typedef UTILS_STRETCHY_T (cz2vm_res_t, unsigned) cz2vm_reses_t;

typedef struct
{
    cz_func_t id;
    ptrdiff_t start_address;
} cz2vm_func_t;

typedef UTILS_STRETCHY_T (cz2vm_func_t, unsigned) cz2vm_funcs_t;

typedef struct
{
    cz_func_t func_id;

    unsigned  in_offset;
    unsigned  in_count;
    ptrdiff_t in_size;

    ptrdiff_t meta_size;

    unsigned  var_offset;
    unsigned  var_count;
    ptrdiff_t var_size;

    unsigned  res_offset;
    unsigned  res_count;
    ptrdiff_t res_size;
} cz2vm_func_def_t;

typedef UTILS_STRETCHY_T (cz2vm_func_def_t, unsigned) cz2vm_func_defs_t;

typedef struct
{
    cz_func_t func_id;
    ptrdiff_t address_position;
} cz2vm_func_patch_t;

typedef UTILS_STRETCHY_T (cz2vm_func_patch_t, unsigned) cz2vm_func_patches_t;

typedef struct
{
    cz2vm_objects_t eval_stack;
    ptrdiff_t sp;

    cz2vm_scopes_t  scopes;
    cz2vm_objects_t scope_results; // TODO: Have result segments fold when they are no longer needed
    cz2vm_labels_t  labels;
    cz2vm_label_patches_t label_patches;
    cz2vm_scope_patches_t scope_patches;

    cz2vm_vars_t      vars;
    cz2vm_ins_t       ins;
    cz2vm_reses_t     reses;
    cz2vm_func_defs_t func_defs;

    cz2vm_funcs_t funcs;
    UTILS_STRETCHY_T (cz_func_t, unsigned) todo_funcs;
    cz2vm_func_patches_t func_patches;
} cz2vm_t;

static inline ptrdiff_t
cz2vm_size(vm_type_type_t type)
{
    switch (type) {
        case vm_type_Int:   return sizeof(int);
        case vm_type_Float: return sizeof(float);
        case vm_type_Ptr:   return sizeof(ptrdiff_t);
        case vm_type_Char:  return sizeof(char);
    }

    fprintf(stderr, "cz2vm_size: Unknown type '%d'!\n", type);
    exit(1);
}

static inline ptrdiff_t
cz2vm_alignment(vm_type_type_t type)
{
    switch (type) {
        case vm_type_Int:   return _Alignof(int);
        case vm_type_Float: return _Alignof(float);
        case vm_type_Ptr:   return _Alignof(ptrdiff_t);
        case vm_type_Char:  return _Alignof(char);
    }

    fprintf(stderr, "cz2vm_alignment: Unknown type '%d'!\n", type);
    exit(1);
}

static inline vm_type_type_t
cz2vm_vm_type(cz_type_t type)
{
    switch (type) {
        case cz_type_Int:   return vm_type_Int;
        case cz_type_Float: return vm_type_Float;
        case cz_type_Bool:  return vm_type_Int;
        case cz_type_Char:  return vm_type_Char;
    }

    fprintf(stderr, "cz2vm_vm_type: Unknown type '%d'!\n", type);
    exit(1);
}

static inline vm_reg_type_t
cz2vm_vm_reg(cz_type_t type)
{
    switch (type) {
        case cz_type_Int:   return vm_reg_Int;
        case cz_type_Float: return vm_reg_Float;
        case cz_type_Bool:  return vm_reg_Int;
        case cz_type_Char:  return vm_reg_Char;
    }

    fprintf(stderr, "cz2vm_vm_reg: Unknown type '%d'!\n", type);
    exit(1);
}

static inline vm_op_type_t
cz2vm_vm_op(cz_inst_op_type_t type)
{
    switch (type) {
        case cz_inst_op_Add: return vm_op_Add;
        case cz_inst_op_Sub: return vm_op_Sub;
        case cz_inst_op_Mul: return vm_op_Mul;
        case cz_inst_op_Div: return vm_op_Div;
        case cz_inst_op_Mod: return vm_op_Mod;

        case cz_inst_op_LT: return vm_op_LT;
        case cz_inst_op_LE: return vm_op_LE;
        case cz_inst_op_GT: return vm_op_GT;
        case cz_inst_op_GE: return vm_op_GE;
        case cz_inst_op_EQ: return vm_op_EQ;
        case cz_inst_op_NE: return vm_op_NE;
    }

    fprintf(stderr, "cz2vm_vm_op: Unknown type '%d'!\n", type);
    exit(1);
}

static inline void
cz2vm_push(cz2vm_t *cz2vm, vm_mem_buf_t *code,
           cz_type_t type, vm_type_type_t vm_type, vm_reg_type_t vm_reg, unsigned vm_reg_ind)
{
    int off = 0;
    if (cz2vm->eval_stack.count != 0) {
        off += cz2vm->eval_stack.data[cz2vm->eval_stack.count - 1].size;
    }
    off += (cz2vm->sp + off) % cz2vm_alignment(vm_type);
    UTILS_STRETCHY_PUSH(cz2vm->eval_stack, (cz2vm_object_t) {
        .type    = type,
        .prev_sp = cz2vm->sp,
        .size    = cz2vm_size(vm_type),
        .offset  = cz2vm->sp + off,
    });
    cz2vm->sp += off;

    vm_inst_push(code, vm_reg, vm_reg_ind, off);
}

static inline unsigned
cz2vm_stack_count(cz2vm_t *cz2vm)
{
    unsigned count = cz2vm->eval_stack.count;

    if (cz2vm->scopes.count > 0) {
        count -= cz2vm->scopes.data[cz2vm->scopes.count - 1].eval_stack_bottom;
    }

    return count;
}

static inline bool
cz2vm_check_result(cz2vm_t *cz2vm, cz2vm_scope_t *scope)
{
    unsigned result_count = cz2vm->eval_stack.count - scope->eval_stack_bottom;

    UTILS_ASSERT(scope->result_count == result_count);
    UTILS_ASSERT(scope->result_sp    == cz2vm->sp);

    for (unsigned i = 0; i < result_count; ++i) {
        cz2vm_object_t new_result = cz2vm->eval_stack.data[scope->eval_stack_bottom + i];
        cz2vm_object_t set_result = cz2vm->scope_results.data[scope->result_offset + i];

        UTILS_ASSERT(new_result.type    == set_result.type);
        UTILS_ASSERT(new_result.prev_sp == set_result.prev_sp);
        UTILS_ASSERT(new_result.size    == set_result.size);
    }

    return true; // TODO: WTF IS THIS
}

static inline cz2vm_var_t *
cz2vm_find_var(cz2vm_t *cz2vm, cz_var_t var_id)
{
    unsigned var_i = 0;
    cz2vm_var_t *var = NULL;

    for (; var_i < cz2vm->vars.count; ++var_i) {
        var = cz2vm->vars.data + var_i;

        if (var->id == var_id)
            break;
    }

    UTILS_ASSERT(var_i != cz2vm->vars.count);

    return var;
}

static bool
cz2vm_get_func_def(cz2vm_t *cz2vm, cz_t *cz, cz_func_t func, cz2vm_func_def_t *func_def_res)
{
    cz2vm_func_def_t *func_def_found = UTILS_STRETCHY_HOSE(cz2vm->func_defs, func_id, func);
    if (func_def_found) {
        *func_def_res = *func_def_found;
        return true;
    }

    cz_function_t *function = UTILS_STRETCHY_HOSE(cz->functions, func, func);
    if (!function) {
        return false;
    }

    cz2vm_func_def_t func_def = {
        .func_id    = func,
        .in_offset  = cz2vm->ins.count,
        .var_offset = cz2vm->vars.count,
        .res_offset = cz2vm->reses.count,
    };

    {
        for (unsigned in_i = 0; in_i < function->input_count; ++in_i) {
            cz_input_t input = cz->inputs.data[function->input_offset + in_i];

            vm_type_type_t vm_type = cz2vm_vm_type(input.type);

            vm_align(&func_def.in_size, cz2vm_alignment(vm_type));

            cz2vm_in_t in = {
                .id     = input.in,
                .type   = input.type,
                .offset = func_def.in_size,
                .size   = cz2vm_size(vm_type),
            };

            func_def.in_size += in.size;

            UTILS_STRETCHY_PUSH(cz2vm->ins, in);
        }

        vm_align(&func_def.in_size, CZ2VM_PROC_ALIGNMENT);
        func_def.in_count = cz2vm->ins.count - func_def.in_offset;
    }
    {
        func_def.meta_size = sizeof(ptrdiff_t) * 2;
        vm_align(&func_def.meta_size, CZ2VM_PROC_ALIGNMENT);
    }
    {
        for (unsigned var_i = 0; var_i < function->variable_count; ++var_i) {
            cz_variable_t variable = cz->variables.data[function->variable_offset + var_i];

            vm_type_type_t vm_type = cz2vm_vm_type(variable.type);

            vm_align(&func_def.var_size, cz2vm_alignment(vm_type));

            cz2vm_var_t var = {
                .id     = variable.var,
                .type   = variable.type,
                .offset = func_def.var_size,
                .size   = cz2vm_size(vm_type),
            };

            func_def.var_size += var.size;

            UTILS_STRETCHY_PUSH(cz2vm->vars, var);
        }

        vm_align(&func_def.var_size, CZ2VM_PROC_ALIGNMENT);
        func_def.var_count = cz2vm->vars.count - func_def.var_offset;
    }
    {
        for (unsigned res_i = 0; res_i < function->result_count; ++res_i) {
            cz_result_t result = cz->results.data[function->result_offset + res_i];

            vm_type_type_t vm_type = cz2vm_vm_type(result.type);

            vm_align(&func_def.res_size, cz2vm_alignment(vm_type));

            cz2vm_res_t res = {
                .type   = result.type,
                .offset = func_def.res_size,
                .size   = cz2vm_size(vm_type),
            };

            func_def.res_size += res.size;

            UTILS_STRETCHY_PUSH(cz2vm->reses, res);
        }

        func_def.res_count = cz2vm->reses.count - func_def.res_offset;
    }

    UTILS_STRETCHY_PUSH(cz2vm->func_defs, func_def);

    *func_def_res = func_def;
    return true;
}

static void
cz2vm_compile_func(cz2vm_t *cz2vm, cz_t *cz, cz_func_t func, vm_mem_buf_t *code)
{
    cz_function_t *function = UTILS_STRETCHY_HOSE(cz->functions, func, func);
    UTILS_ASSERT(function);

    // TODO: Some of these should be persistent and referenced from the functions.
    cz2vm->eval_stack.count = 0;
    cz2vm->scopes.count = 0;
    cz2vm->scope_results.count = 0;
    cz2vm->labels.count = 0;
    cz2vm->label_patches.count = 0;
    cz2vm->scope_patches.count = 0;
    cz2vm->scope_results.count = 0;

    cz2vm_func_def_t func_def = {0};
    bool success = cz2vm_get_func_def(cz2vm, cz, func, &func_def);
    UTILS_ASSERT(success);

    UTILS_STRETCHY_PUSH(cz2vm->funcs, (cz2vm_func_t) {
        .id            = func,
        .start_address = code->count,
    });

    if (func_def.var_size != 0) {
        VM_PUSH(code, None, 0, func_def.var_size);
    }
    cz2vm->sp = func_def.meta_size + func_def.var_size;

    for (unsigned inst_i = 0; inst_i < function->code_count; ++inst_i) {
        cz_inst_t *inst = cz->code.data + function->code_offset + inst_i;

        switch (inst->type) {
            case cz_inst_Halt: {
                VM_HALT(code);
            } break;

            case cz_inst_Imm: {
                switch (inst->imm.type) {
                    case cz_type_Int:   VM_IMM_INT(   code, 0, inst->imm.as_int   ); break;
                    case cz_type_Float: VM_IMM_FLOAT( code, 0, inst->imm.as_float ); break;
                    case cz_type_Bool:  VM_IMM_INT(   code, 0, inst->imm.as_int   ); break;
                    case cz_type_Char:  VM_IMM_CHAR(  code, 0, inst->imm.as_char  ); break;
                }

                vm_type_type_t vm_type = cz2vm_vm_type(inst->imm.type);
                vm_reg_type_t  vm_reg  = cz2vm_vm_reg(inst->imm.type);

                cz2vm_push(cz2vm, code, inst->imm.type, vm_type, vm_reg, 0);
            } break;

            case cz_inst_Op: {
                UTILS_ASSERT(cz2vm_stack_count(cz2vm) >= 2);

                cz2vm_object_t object_r = cz2vm->eval_stack.data[--(cz2vm->eval_stack.count)];
                cz2vm_object_t object_l = cz2vm->eval_stack.data[--(cz2vm->eval_stack.count)];

                UTILS_ASSERT(object_r.type == object_l.type);
                vm_reg_type_t  vm_reg  = cz2vm_vm_reg(object_r.type);
                vm_type_type_t vm_type = cz2vm_vm_type(object_r.type);

                int off = cz2vm->sp - object_r.prev_sp;
                vm_inst_pop(code, vm_reg, 1, off);
                cz2vm->sp -= off;

                off = cz2vm->sp - object_l.prev_sp;
                vm_inst_pop(code, vm_reg, 0, off);
                cz2vm->sp -= off;

                vm_op_type_t vm_op_type = cz2vm_vm_op(inst->op.type);

                vm_inst_op(code, vm_type, vm_op_type);

                if (vm_op_type == vm_op_LT
                 || vm_op_type == vm_op_LE
                 || vm_op_type == vm_op_GT
                 || vm_op_type == vm_op_GE
                 || vm_op_type == vm_op_EQ
                 || vm_op_type == vm_op_NE
                ) {
                    cz2vm_push(cz2vm, code, cz_type_Bool, vm_type_Int, vm_reg_Int, 0);
                }
                else {
                    cz2vm_push(cz2vm, code, object_r.type, vm_type, vm_reg, 0);
                }
            } break;

            case cz_inst_ScopeBegin: {
                UTILS_STRETCHY_PUSH(cz2vm->scopes, (cz2vm_scope_t) {
                    .id                = inst->scope,
                    .eval_stack_bottom = cz2vm->eval_stack.count,
                    .label_offset      = cz2vm->labels.count,
                    .sp_bottom         = cz2vm->sp,
                });
            } break;

            case cz_inst_ScopeEnd: {
                UTILS_ASSERT(cz2vm->scopes.count > 0);

                cz2vm_scope_t scope = cz2vm->scopes.data[cz2vm->scopes.count - 1];
                UTILS_ASSERT(scope.id == inst->scope);

                if (scope.is_result_set) {
                    UTILS_ASSERT(cz2vm_check_result(cz2vm, &scope) == true);
                }

                unsigned patch_dst_i = 0;

                ptrdiff_t code_position = code->count;

                for (unsigned patch_i = 0; patch_i < cz2vm->scope_patches.count; ++patch_i) {
                    cz2vm_scope_patch_t *patch = cz2vm->scope_patches.data + patch_i;

                    if (patch->scope_id == scope.id) {
                        vm_link(code, patch->jmp_position, code_position);
                        continue;
                    }

                    if (patch_i != patch_dst_i) {
                        cz2vm->scope_patches.data[patch_dst_i] = *patch;
                    }
                    ++patch_dst_i;
                }
                cz2vm->scope_patches.count = patch_dst_i;

                cz2vm->scopes.count--;
                cz2vm->labels.count = scope.label_offset;
            } break;

            case cz_inst_Label: {
                UTILS_ASSERT(cz2vm->scopes.count > 0);
                UTILS_ASSERT(cz2vm_stack_count(cz2vm) == 0);

                cz2vm_scope_t scope = cz2vm->scopes.data[cz2vm->scopes.count - 1];

                cz2vm_label_t label = {
                    .id       = inst->label,
                    .position = code->count,
                };

                UTILS_STRETCHY_PUSH(cz2vm->labels, label);

                unsigned patch_dst_i = 0;

                for (unsigned patch_i = 0; patch_i < cz2vm->label_patches.count; ++patch_i) {
                    cz2vm_label_patch_t *patch = cz2vm->label_patches.data + patch_i;

                    if (patch->label_id == label.id) {
                        UTILS_ASSERT(patch->scope_id == scope.id);

                        vm_link(code, patch->jmp_position, label.position);
                        continue;
                    }

                    if (patch_i != patch_dst_i) {
                        cz2vm->label_patches.data[patch_dst_i] = *patch;
                    }
                    ++patch_dst_i;
                }
                cz2vm->label_patches.count = patch_dst_i;
            } break;

            case cz_inst_Jmp: {
                UTILS_ASSERT(cz2vm->scopes.count > 0);
                UTILS_ASSERT(cz2vm_stack_count(cz2vm) >= 1);

                cz2vm_object_t object = cz2vm->eval_stack.data[--(cz2vm->eval_stack.count)];
                UTILS_ASSERT(object.type == cz_type_Bool);

                int off = cz2vm->sp - object.prev_sp;
                vm_inst_pop(code, vm_reg_Int, 0, off);
                cz2vm->sp -= off;

                ptrdiff_t jmp_position = VM_JMP_IF(code);

                UTILS_ASSERT(cz2vm_stack_count(cz2vm) == 0);

                int label_i = (int)(cz2vm->labels.count) - 1;
                int scope_i = (int)(cz2vm->scopes.count) - 1;
                cz2vm_scope_t *scope = NULL;
                cz2vm_label_t *label = NULL;

                for (; scope_i >= 0; --scope_i) {
                    scope = cz2vm->scopes.data + scope_i;

                    for (; label_i >= 0 && label_i >= (int)scope->label_offset; --label_i) {
                        label = cz2vm->labels.data + label_i;

                        if (label->id == inst->label)
                            goto found_label;
                    }

                    if (label_i == -1)
                        break;
                }

                UTILS_STRETCHY_PUSH(cz2vm->label_patches, (cz2vm_label_patch_t) {
                    .scope_id     = cz2vm->scopes.data[cz2vm->scopes.count - 1].id,
                    .label_id     = inst->jmp.label,
                    .jmp_position = jmp_position,
                });

                break;

            found_label:
                UTILS_ASSERT(scope->eval_stack_bottom == cz2vm->eval_stack.count);
                UTILS_ASSERT(scope->sp_bottom         == cz2vm->sp);

                vm_link(code, jmp_position, label->position);
            } break;

            case cz_inst_Brk: {
                UTILS_ASSERT(cz2vm->scopes.count > 0);

                VM_IMM_INT(code, 0, 1);
                ptrdiff_t jmp_position = VM_JMP_IF(code);

                unsigned scope_i = 0;
                cz2vm_scope_t *scope = NULL;

                for (; scope_i < cz2vm->scopes.count; ++scope_i) {
                    scope = cz2vm->scopes.data + scope_i;

                    if (scope->id == inst->brk.scope)
                        break;
                }

                UTILS_ASSERT(scope_i != cz2vm->scopes.count);

                if (scope->is_result_set) {
                    UTILS_ASSERT(cz2vm_check_result(cz2vm, scope) == true);
                }
                else {
                    unsigned result_count = cz2vm->eval_stack.count - scope->eval_stack_bottom;

                    scope->is_result_set = true;
                    scope->result_offset = cz2vm->scope_results.count;
                    scope->result_count  = result_count;
                    scope->result_sp     = cz2vm->sp;

                    UTILS_STRETCHY_RESERVE(cz2vm->scope_results, result_count);

                    for (unsigned i = 0; i < result_count; ++i) {
                        cz2vm->scope_results.data[scope->result_offset + i] = cz2vm->eval_stack.data[scope->eval_stack_bottom + i];
                    }

                    cz2vm->scope_results.count += result_count;
                }

                cz2vm->eval_stack.count = scope->eval_stack_bottom;
                cz2vm->sp               = scope->sp_bottom;

                UTILS_STRETCHY_PUSH(cz2vm->scope_patches, (cz2vm_scope_patch_t) {
                    .scope_id     = scope->id,
                    .jmp_position = jmp_position,
                });
            } break;

            case cz_inst_Load: {
                if (inst->load.mem_type == cz_mem_Var) {
                    cz2vm_var_t *var = UTILS_STRETCHY_HOSE(cz2vm->vars, id, inst->load.var);
                    UTILS_ASSERT(var);

                    vm_reg_type_t  vm_reg  = cz2vm_vm_reg(var->type);
                    vm_type_type_t vm_type = cz2vm_vm_type(var->type);

                    VM_IMM_PTR(code, 0, func_def.meta_size + var->offset);
                    vm_inst_load(code, vm_reg, 0);
                    cz2vm_push(cz2vm, code, var->type, vm_type, vm_reg, 0);
                }
                else {
                    cz2vm_in_t *in = UTILS_STRETCHY_HOSE(cz2vm->ins, id, inst->load.in);
                    UTILS_ASSERT(in);

                    vm_reg_type_t  vm_reg  = cz2vm_vm_reg(in->type);
                    vm_type_type_t vm_type = cz2vm_vm_type(in->type);

                    VM_IMM_PTR(code, 0, in->offset - func_def.in_size);
                    vm_inst_load(code, vm_reg, 0);
                    cz2vm_push(cz2vm, code, in->type, vm_type, vm_reg, 0);
                }
            } break;

            case cz_inst_Store: {
                UTILS_ASSERT(cz2vm_stack_count(cz2vm) >= 1);

                cz2vm_object_t object = cz2vm->eval_stack.data[--(cz2vm->eval_stack.count)];

                vm_reg_type_t vm_reg = cz2vm_vm_reg(object.type);

                int off = cz2vm->sp - object.prev_sp;
                vm_inst_pop(code, vm_reg, 0, off);
                cz2vm->sp -= off;

                if (inst->store.mem_type == cz_mem_Var) {
                    cz2vm_var_t *var = UTILS_STRETCHY_HOSE(cz2vm->vars, id, inst->load.var);
                    UTILS_ASSERT(var);

                    UTILS_ASSERT(var->type == object.type);

                    VM_IMM_PTR(code, 0, func_def.meta_size + var->offset);
                    vm_inst_store(code, vm_reg, 0);
                }
                else {
                    cz2vm_in_t *in = UTILS_STRETCHY_HOSE(cz2vm->ins, id, inst->load.in);
                    UTILS_ASSERT(in);

                    UTILS_ASSERT(in->type == object.type);

                    VM_IMM_PTR(code, 0, in->offset - func_def.in_size);
                    vm_inst_store(code, vm_reg, 0);
                }
            } break;

            case cz_inst_Call: {
                cz2vm_func_def_t callee_def = {0};
                success = cz2vm_get_func_def(cz2vm, cz, inst->call.func, &callee_def);
                UTILS_ASSERT(success);

                UTILS_ASSERT(cz2vm_stack_count(cz2vm) >= callee_def.in_count);

                ptrdiff_t input_offset;

                if (callee_def.in_count == 0) {
                    input_offset = cz2vm->sp;

                    if (cz2vm->eval_stack.count > 0) {
                        input_offset += cz2vm->eval_stack.data[cz2vm->eval_stack.count - 1].size;
                    }
                }
                else {
                    unsigned first_input_i = cz2vm->eval_stack.count - callee_def.in_count;
                    input_offset = cz2vm->eval_stack.data[first_input_i].offset;
                }
                
                vm_align(&input_offset, CZ2VM_PROC_ALIGNMENT);

                ptrdiff_t next_sp = input_offset + callee_def.in_size;

                if (cz2vm->sp != next_sp) {
                    VM_PUSH(code, None, 0, next_sp - cz2vm->sp);
                    cz2vm->sp = next_sp;
                }

                if (callee_def.in_count > 0) {
                    unsigned first_input_i = cz2vm->eval_stack.count - callee_def.in_count;

                    unsigned in_i = callee_def.in_count;

                    do {
                        --in_i;

                        cz2vm_in_t     in  = cz2vm->ins.data[callee_def.in_offset + in_i];
                        cz2vm_object_t obj = cz2vm->eval_stack.data[first_input_i + in_i];

                        UTILS_ASSERT(in.type == obj.type);
                        UTILS_ASSERT(in.size == obj.size);

                        ptrdiff_t dst = input_offset + in.offset;
                        ptrdiff_t src = obj.offset;

                        // TODO: Merge moves if possible.
                        if (dst != src) {
                            vm_inst_memmove(code, dst, src, in.size);
                        }
                    } while (in_i != 0);
                }

                cz2vm->eval_stack.count -= callee_def.in_count;

                cz2vm_func_t *cz2vm_func = UTILS_STRETCHY_HOSE(cz2vm->funcs, id, inst->call.func);
                if (cz2vm_func) {
                    vm_inst_call(code, cz2vm_func->start_address);
                }
                else {
                    vm_inst_call(code, 0xDEADC0DE);
                    UTILS_STRETCHY_PUSH(cz2vm->func_patches, (cz2vm_func_patch_t) {
                        .func_id          = inst->call.func,
                        .address_position = code->count - sizeof(ptrdiff_t),
                    });

                    unsigned todo_func_i = 0;
                    for (; todo_func_i < cz2vm->todo_funcs.count; ++todo_func_i) {
                        if (cz2vm->todo_funcs.data[todo_func_i] == inst->call.func)
                            break;
                    }

                    if (todo_func_i == cz2vm->todo_funcs.count) {
                        UTILS_STRETCHY_PUSH(cz2vm->todo_funcs, inst->call.func);
                    }
                }

                {
                    ptrdiff_t dst     = func_def.meta_size + func_def.var_size;
                    ptrdiff_t prev_sp = func_def.meta_size + func_def.var_size;

                    if (cz2vm->eval_stack.count > 0) {
                        cz2vm_object_t *object = cz2vm->eval_stack.data + cz2vm->eval_stack.count - 1;
                        dst     = object->offset + object->size;
                        prev_sp = object->offset;
                    }

                    ptrdiff_t src_base = input_offset + callee_def.in_size + callee_def.meta_size + callee_def.var_size;
                    
                    for (unsigned res_i = 0; res_i < callee_def.res_count; ++res_i) {
                        cz2vm_res_t res = cz2vm->reses.data[callee_def.res_offset + res_i];

                        vm_type_type_t vm_type = cz2vm_vm_type(res.type);

                        vm_align(&dst, cz2vm_alignment(vm_type));

                        ptrdiff_t src = src_base + res.offset;

                        // TODO: Merge moves if possible.
                        if (dst != src) {
                            vm_inst_memmove(code, dst, src, res.size);
                        }

                        UTILS_STRETCHY_PUSH(cz2vm->eval_stack, (cz2vm_object_t) {
                            .type    = res.type,
                            .prev_sp = prev_sp,
                            .size    = res.size,
                            .offset  = dst,
                        });

                        prev_sp = dst;
                        dst += res.size;
                    }

                    ptrdiff_t callee_sp = src_base;
                    if (callee_def.res_count > 0) {
                        callee_sp += cz2vm->reses.data[callee_def.res_offset + callee_def.res_count - 1].offset;
                    }

                    if (prev_sp != callee_sp) {
                        VM_POP(code, None, 0, callee_sp - prev_sp);
                    }
                }
            } break;

            case cz_inst_Print: {
                UTILS_ASSERT(cz2vm_stack_count(cz2vm) >= 1);

                cz2vm_object_t object = cz2vm->eval_stack.data[--(cz2vm->eval_stack.count)];
                vm_reg_type_t  vm_reg = cz2vm_vm_reg(object.type);

                int off = cz2vm->sp - object.prev_sp;
                vm_inst_pop(code, vm_reg, 0, off);
                cz2vm->sp -= off;

                vm_inst_print(code, vm_reg, 0);
            } break;

            case cz_inst_Cow: {
                VM_COW(code);
            } break;
        }
    }

    UTILS_ASSERT(cz2vm->eval_stack.count == func_def.res_count);

    for (unsigned res_i = 0; res_i < func_def.res_count; ++res_i) {
        cz2vm_object_t obj = cz2vm->eval_stack.data[res_i];
        cz2vm_res_t    res = cz2vm->reses.data[func_def.res_offset + res_i];

        UTILS_ASSERT(obj.type   == res.type);
        UTILS_ASSERT(obj.size   == res.size);
        UTILS_ASSERT(obj.offset == res.offset + func_def.meta_size + func_def.var_size);
    }

    vm_inst_ret(code);
}

void
cz2vm_compile(cz_t *cz, cz_func_t func, vm_mem_buf_t *code)
{
    static cz2vm_t cz2vm_val = {0};
    cz2vm_t *cz2vm = &cz2vm_val;

    cz2vm->todo_funcs.count = 0;
    cz2vm->func_patches.count = 0;

    cz2vm_func_def_t func_def = {0};
    bool success = cz2vm_get_func_def(cz2vm, cz, func, &func_def);
    UTILS_ASSERT(success);

    if (func_def.in_size != 0) {
        VM_PUSH(code, None, 0, func_def.in_size);
    }

    cz2vm_func_t *cz2vm_func = UTILS_STRETCHY_HOSE(cz2vm->funcs, id, func);
    if (cz2vm_func) {
        vm_inst_call(code, cz2vm_func->start_address);
    }
    else {
        vm_inst_call(code, 0xDEADC0DE);
        UTILS_STRETCHY_PUSH(cz2vm->func_patches, (cz2vm_func_patch_t) {
            .func_id          = func,
            .address_position = code->count - sizeof(ptrdiff_t),
        });
        UTILS_STRETCHY_PUSH(cz2vm->todo_funcs, func);
    }

    VM_HALT(code);

    while (cz2vm->todo_funcs.count != 0) {
        cz_func_t func = cz2vm->todo_funcs.data[--(cz2vm->todo_funcs.count)];
        cz2vm_compile_func(cz2vm, cz, func, code);
    }

    for (unsigned i = 0; i < cz2vm->func_patches.count; ++i) {
        cz2vm_func_patch_t patch = cz2vm->func_patches.data[i];

        cz2vm_func_t *cz2vm_func = UTILS_STRETCHY_HOSE(cz2vm->funcs, id, patch.func_id);
        UTILS_ASSERT(cz2vm_func);

        *(ptrdiff_t *)(code->data + patch.address_position) = cz2vm_func->start_address;
    }
}

#endif // CZ2VM_IMPL
