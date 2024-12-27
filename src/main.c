#define VM_IMPL
#include "vm.h"

typedef enum
{
    cz_inst_Halt,
    cz_inst_Imm,
    cz_inst_Op,
    cz_inst_ScopeBegin,
    cz_inst_ScopeEnd,
    cz_inst_Label,
    cz_inst_Jmp,
    cz_inst_Brk,
    cz_inst_Load,
    cz_inst_Store,
    cz_inst_Call,
    cz_inst_Print,
    cz_inst_Cow,
} cz_inst_type_t;

typedef enum
{
    cz_type_Int,
    cz_type_Float,
    cz_type_Bool,
    cz_type_Char,
} cz_type_t;

typedef enum
{
    cz_inst_op_Add,
    cz_inst_op_Sub,
    cz_inst_op_Mul,
    cz_inst_op_Div,
    cz_inst_op_Mod,

    cz_inst_op_LT,
    cz_inst_op_LE,
    cz_inst_op_GT,
    cz_inst_op_GE,
    cz_inst_op_EQ,
    cz_inst_op_NE,
} cz_inst_op_type_t;

typedef unsigned cz_scope_t;
typedef unsigned cz_label_t;
typedef unsigned cz_var_t;
typedef unsigned cz_func_t;

typedef struct
{
    cz_inst_type_t type;

    union {
        struct {
            cz_type_t type;
            union {
                int   as_int;
                float as_float;
                bool  as_bool;
                char  as_char;
            };
        } imm;

        struct {
            cz_inst_op_type_t type;
        } op;

        cz_scope_t scope;
        cz_label_t label;

        struct {
            cz_label_t label;
        } jmp;

        struct {
            cz_scope_t scope;
        } brk;

        struct {
            cz_var_t var;
            cz_type_t type;
        } var;

        struct {
            cz_var_t var;
        } load;

        struct {
            cz_var_t var;
        } store;

        struct {
            cz_func_t func;
        } call;
    };
} cz_inst_t;

typedef UTILS_STRETCHY_T (cz_inst_t, unsigned) cz_code_t;

typedef struct
{
    cz_var_t  var;
    cz_type_t type;
} cz_variable_t;

typedef UTILS_STRETCHY_T (cz_variable_t, unsigned) cz_variables_t;

typedef struct
{
    cz_func_t func;

    unsigned code_offset;
    unsigned code_count;

    unsigned variable_offset;
    unsigned variable_count;
} cz_function_t;

typedef UTILS_STRETCHY_T (cz_function_t, unsigned) cz_functions_t;

typedef struct
{
    cz_code_t      code;
    cz_variables_t variables;
    cz_functions_t functions;

    cz_scope_t last_scope;
    cz_label_t last_label;
    cz_var_t   last_var;
    cz_func_t  last_func;
} cz_t;

void
cz_emit_inst(cz_t *cz, cz_inst_t inst)
{
    UTILS_STRETCHY_PUSH(cz->code, inst);
}

void
cz_add_variable(cz_t *cz, cz_var_t var, cz_type_t type)
{
    UTILS_STRETCHY_PUSH(cz->variables, (cz_variable_t) {
        .var  = var,
        .type = type,
    });
}

void
cz_function_begin(cz_t *cz, cz_func_t func)
{
    UTILS_STRETCHY_PUSH(cz->functions, (cz_function_t) {
        .func            = func,
        .code_offset     = cz->code.count,
        .variable_offset = cz->variables.count,
    });
}

void
cz_function_end(cz_t *cz, cz_func_t func)
{
    UTILS_ASSERT(cz->functions.count > 0);

    cz_function_t *function = cz->functions.data + cz->functions.count - 1;

    UTILS_ASSERT(function->func == func);

    function->code_count     = cz->code.count      - function->code_offset;
    function->variable_count = cz->variables.count - function->variable_offset;
}

#define CZ_FUNC(cz, func_ident) \
    cz_func_t func_ident = ++((cz)->last_func); \
    for (bool __done_##__LINE__ = (cz_function_begin((cz), func_ident), false); \
         !__done_##__LINE__; \
         cz_function_end((cz), func_ident), __done_##__LINE__ = true)

#define CZ_HALT(cz) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Halt, \
    })

#define CZ_IMM_INT(cz, val) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Imm, \
        .imm  = { \
            .type   = cz_type_Int, \
            .as_int = val, \
        }, \
    })

#define CZ_IMM_FLOAT(cz, val) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Imm, \
        .imm  = { \
            .type     = cz_type_Float, \
            .as_float = val, \
        }, \
    })

#define CZ_IMM_BOOL(cz, val) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Imm, \
        .imm  = { \
            .type    = cz_type_Bool, \
            .as_bool = val, \
        }, \
    })

#define CZ_IMM_CHAR(cz, val) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Imm, \
        .imm  = { \
            .type    = cz_type_Char, \
            .as_char = val, \
        }, \
    })

#define CZ_OP(cz, op_sf) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Op, \
        .op   = { \
            .type = cz_inst_op_##op_sf, \
        }, \
    })

#define CZ_PRINT(cz) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Print, \
    })

#define CZ_COW(cz) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Cow, \
    })

#define CZ_SCOPE_BEGIN(cz) \
( \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type  = cz_inst_ScopeBegin, \
        .scope = ++((cz)->last_label), \
    }), \
    (cz)->last_label \
)

#define CZ_SCOPE_END(cz, scope_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type  = cz_inst_ScopeEnd, \
        .scope = (scope_id), \
    })

#define CZ_SCOPE(cz, scope_ident) \
    for (cz_scope_t scope_ident = CZ_SCOPE_BEGIN(cz), __done_##__LINE__ = false; \
         !__done_##__LINE__; \
         CZ_SCOPE_END(cz, scope_ident), __done_##__LINE__ = true)

#define CZ_JMP(cz, label_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Jmp, \
        .jmp  = { \
            .label = (label_id), \
        }, \
    })

#define CZ_BRK(cz, scope_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Brk, \
        .brk  = { \
            .scope = (scope_id), \
        }, \
    })

#define CZ_LABEL_MAKE(cz) (++((cz)->last_label))

#define CZ_LABEL_SET(cz, label_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type  = cz_inst_Label, \
        .label = (label_id), \
    })

#define CZ_VAR(cz, type_sf) \
( \
    cz_add_variable((cz), ++((cz)->last_var), cz_type_##type_sf), \
    (cz)->last_var \
)

#define CZ_LOAD(cz, var_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Load, \
        .load = { \
            .var = (var_id), \
        }, \
    })

#define CZ_STORE(cz, var_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Store, \
        .load = { \
            .var = (var_id), \
        }, \
    })

#define CZ_CALL(cz, func_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Call, \
        .call = { \
            .func = func_id, \
        }, \
    })


static inline const char *
cz_inst_type_name(cz_inst_type_t inst_type)
{
    switch (inst_type) {
        case cz_inst_Halt:       return "halt";
        case cz_inst_Imm:        return "imm";
        case cz_inst_Op:         return "op";
        case cz_inst_ScopeBegin: return "scope_begin";
        case cz_inst_ScopeEnd:   return "scope_end";
        case cz_inst_Label:      return "label";
        case cz_inst_Jmp:        return "jmp";
        case cz_inst_Brk:        return "brk";
        case cz_inst_Load:       return "load";
        case cz_inst_Store:      return "store";
        case cz_inst_Call:       return "call";
        case cz_inst_Print:      return "print";
        case cz_inst_Cow:        return "cow";
    }

    return "<unknown_inst_type>";
}

typedef struct
{
    cz_type_t type;
    ptrdiff_t prev_sp;
    ptrdiff_t size;
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
    cz_func_t id;
    ptrdiff_t start_address;
} cz2vm_func_t;

typedef UTILS_STRETCHY_T (cz2vm_func_t, unsigned) cz2vm_funcs_t;

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
    cz2vm_objects_t scope_results;
    cz2vm_labels_t  labels;
    cz2vm_label_patches_t label_patches;
    cz2vm_scope_patches_t scope_patches;
    cz2vm_objects_t results; // TODO: Have result segments fold when they are no longer needed

    cz2vm_vars_t vars;

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
        cz2vm_object_t set_result = cz2vm->results.data[scope->result_offset + i];

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

static void
cz2vm_compile(cz2vm_t *cz2vm, cz_t *cz, cz_func_t func, vm_mem_buf_t *code)
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
    cz2vm->results.count = 0;
    cz2vm->vars.count = 0;

    UTILS_STRETCHY_PUSH(cz2vm->funcs, (cz2vm_func_t) {
        .id            = func,
        .start_address = code->count,
    });

    cz2vm->sp = sizeof(ptrdiff_t) * 2; // TODO: Maybe alignment too?

    ptrdiff_t variable_offset = 0;

    for (unsigned var_i = 0; var_i < function->variable_count; ++var_i) {
        cz_variable_t variable = cz->variables.data[function->variable_offset + var_i];

        vm_type_type_t vm_type = cz2vm_vm_type(variable.type);

        vm_align(&variable_offset, cz2vm_alignment(vm_type));

        cz2vm_var_t var = {
            .id     = variable.var,
            .type   = variable.type,
            .offset = variable_offset,
            .size   = cz2vm_size(vm_type),
        };

        UTILS_STRETCHY_PUSH(cz2vm->vars, var);

        variable_offset += var.size;
    }

    vm_align(&variable_offset, 16); // TODO: Don't do it like this. Allow for arbitrary offset.

    printf("variable_offset = %d\n", (int)variable_offset);

    VM_PUSH(code, None, 0, variable_offset);
    cz2vm->sp += variable_offset;

    for (unsigned inst_i = 0; inst_i < function->code_count; ++inst_i) {
        cz_inst_t *inst = cz->code.data + function->code_offset + inst_i;

        printf("inst type: '%s'\n", cz_inst_type_name(inst->type));

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
                    scope->result_offset = cz2vm->results.count;
                    scope->result_count  = result_count;
                    scope->result_sp     = cz2vm->sp;

                    UTILS_STRETCHY_RESERVE(cz2vm->results, result_count);

                    for (unsigned i = 0; i < result_count; ++i) {
                        cz2vm->results.data[scope->result_offset + i] = cz2vm->eval_stack.data[scope->eval_stack_bottom + i];
                    }

                    cz2vm->results.count += result_count;
                }

                cz2vm->eval_stack.count = scope->eval_stack_bottom;
                cz2vm->sp               = scope->sp_bottom;

                UTILS_STRETCHY_PUSH(cz2vm->scope_patches, (cz2vm_scope_patch_t) {
                    .scope_id     = scope->id,
                    .jmp_position = jmp_position,
                });
            } break;

            case cz_inst_Load: {
                cz2vm_var_t *var = UTILS_STRETCHY_HOSE(cz2vm->vars, id, inst->load.var);
                UTILS_ASSERT(var);

                vm_reg_type_t  vm_reg  = cz2vm_vm_reg(var->type);
                vm_type_type_t vm_type = cz2vm_vm_type(var->type);

                VM_IMM_PTR(code, 0, var->offset);
                vm_inst_load(code, vm_reg, 0);
                cz2vm_push(cz2vm, code, var->type, vm_type, vm_reg, 0);
            } break;

            case cz_inst_Store: {
                UTILS_ASSERT(cz2vm_stack_count(cz2vm) >= 1);

                cz2vm_object_t object = cz2vm->eval_stack.data[--(cz2vm->eval_stack.count)];

                cz2vm_var_t *var = UTILS_STRETCHY_HOSE(cz2vm->vars, id, inst->load.var);
                UTILS_ASSERT(var);

                UTILS_ASSERT(var->type == object.type);

                vm_reg_type_t vm_reg = cz2vm_vm_reg(var->type);

                int off = cz2vm->sp - object.prev_sp;
                vm_inst_pop(code, vm_reg, 0, off);
                cz2vm->sp -= off;

                VM_IMM_PTR(code, 0, var->offset);
                vm_inst_store(code, vm_reg, 0);
            } break;

            case cz_inst_Call: {
                ptrdiff_t prev_sp = cz2vm->sp;

                if (cz2vm->eval_stack.count > 0) {
                    cz2vm->sp += cz2vm->eval_stack.data[cz2vm->eval_stack.count - 1].size;
                }

                vm_align(&(cz2vm->sp), 16); // TODO: Not like this. Use a constant def.

                if (prev_sp != cz2vm->sp) {
                    VM_PUSH(code, None, 0, cz2vm->sp - prev_sp);
                }

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

                if (prev_sp != cz2vm->sp) {
                    VM_POP(code, None, 0, cz2vm->sp - prev_sp);
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

    vm_inst_ret(code);
}

void
cz_compile_to_vm(cz_t *cz, cz_func_t func, vm_mem_buf_t *code)
{
    static cz2vm_t cz2vm_val = {0};
    cz2vm_t *cz2vm = &cz2vm_val;

    cz2vm->todo_funcs.count = 0;
    cz2vm->func_patches.count = 0;

    if (!UTILS_STRETCHY_HOSE(cz2vm->funcs, id, func)) {
        UTILS_STRETCHY_PUSH(cz2vm->todo_funcs, func);
    }

    vm_inst_call(code, 0xDEADC0DE);
    UTILS_STRETCHY_PUSH(cz2vm->func_patches, (cz2vm_func_patch_t) {
        .func_id          = func,
        .address_position = code->count - sizeof(ptrdiff_t),
    });

    VM_HALT(code);

    while (cz2vm->todo_funcs.count != 0) {
        cz_func_t func = cz2vm->todo_funcs.data[--(cz2vm->todo_funcs.count)];
        cz2vm_compile(cz2vm, cz, func, code);
    }

    for (unsigned i = 0; i < cz2vm->func_patches.count; ++i) {
        cz2vm_func_patch_t patch = cz2vm->func_patches.data[i];

        cz2vm_func_t *cz2vm_func = UTILS_STRETCHY_HOSE(cz2vm->funcs, id, patch.func_id);
        UTILS_ASSERT(cz2vm_func);

        *(ptrdiff_t *)(code->data + patch.address_position) = cz2vm_func->start_address;
    }
}

int
main(void)
{
    vm_mem_buf_t code = {0};
    vm_mem_buf_t *c = &code;

#if 0
    VM_IMM_INT(c, 0, 1);
    VM_PUSH(c, Int, 0, 0);

    unsigned label_1 = c->count;
        VM_POP(c, Int, 0, 0);
        VM_IMM_INT(c, 1, 2);
        VM_OP(c, Int, Mul);

        VM_PRINT(c, Int, 0);
        VM_PUSH(c, Int, 0, 0);

        VM_IMM_INT(c, 1, 64);
        VM_OP(c, Int, NE);
    vm_link(c, VM_JMP_IF(c), label_1);

    VM_COW(c);
    VM_HALT(c);
#elif 0
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, test_func) {
        CZ_IMM_INT(cz, 1);
        CZ_IMM_INT(cz, 665);
        CZ_OP(cz, Add);

        CZ_PRINT(cz);
        CZ_COW(cz);
    }

    cz_compile_to_vm(cz, test_func, c);
#elif 0
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, test_func) {
        CZ_IMM_INT(cz, 1);
        CZ_IMM_INT(cz, 2);
        CZ_OP(cz, LT);
        CZ_PRINT(cz);

        CZ_COW(cz);
    }

    cz_compile_to_vm(cz, test_func, c);
#elif 0
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, test_func) {
        CZ_SCOPE(cz, scope) {
            cz_label_t label = CZ_LABEL_MAKE(cz);

            CZ_IMM_INT(cz, 1);
            CZ_IMM_INT(cz, 2);
            CZ_OP(cz, Mul);

            CZ_IMM_INT(cz, 65);
            CZ_OP(cz, LT);

            CZ_JMP(cz, label);
                CZ_IMM_CHAR(cz, 'A');
                CZ_PRINT(cz);

                CZ_IMM_FLOAT(cz, 6.66f);

                CZ_BRK(cz, scope);

            CZ_LABEL_SET(cz, label);
                CZ_IMM_CHAR(cz, 'B');
                CZ_PRINT(cz);

                CZ_IMM_FLOAT(cz, 123.4f);
        }

        CZ_PRINT(cz);

        CZ_COW(cz);
    }

    cz_compile_to_vm(cz, test_func, c);
#elif 0
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, test_func) {
        cz_var_t var_1 = CZ_VAR(cz, Int);
        CZ_IMM_INT(cz, 1);
        CZ_STORE(cz, var_1);

        CZ_SCOPE(cz, scope) {
            cz_label_t l_start = CZ_LABEL_MAKE(cz);
            CZ_LABEL_SET(cz, l_start);

            CZ_LOAD(cz, var_1);
            CZ_PRINT(cz);

            CZ_LOAD(cz, var_1);
            CZ_IMM_INT(cz, 123);

            CZ_OP(cz, GT);

            cz_label_t l_end = CZ_LABEL_MAKE(cz);
            CZ_JMP(cz, l_end);

            CZ_LOAD(cz, var_1);
            CZ_IMM_INT(cz, 2);
            CZ_OP(cz, Mul);

            CZ_STORE(cz, var_1);

            CZ_IMM_BOOL(cz, true);
            CZ_JMP(cz, l_start);

            CZ_LABEL_SET(cz, l_end);

            CZ_BRK(cz, scope);
        }

        CZ_COW(cz);
        CZ_HALT(cz);
    }

    cz_compile_to_vm(cz, test_func, c);
#else
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, print_a_func) {
        CZ_IMM_CHAR(cz, 'A');
        CZ_PRINT(cz);
    }

    CZ_FUNC(cz, main_func) {

        CZ_CALL(cz, print_a_func);

        CZ_COW(cz);

        // CZ_CALL(cz, main_func);
    }

    cz_compile_to_vm(cz, main_func, c);
#endif

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    vm_run(c, data, data_size);

    return 0;
}

