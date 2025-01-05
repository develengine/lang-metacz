#ifndef CZ_H_
#define CZ_H_

#include "utils.h"

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

    cz_inst_Ref,
    cz_inst_Select,
    cz_inst_Deref,
    cz_inst_Set,

    cz_inst_Print,
    cz_inst_Cow,
} cz_inst_type_t;

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

typedef enum
{
    cz_mem_Var,
    cz_mem_In,
} cz_mem_type_t;

typedef enum
{
    cz_type_Int,
    cz_type_Char,
    cz_type_Bool,
    cz_type_Float,
} cz_type_leaf_t;

typedef enum
{
    cz_type_type_Leaf,
    cz_type_type_Struct,
} cz_type_type_t;

typedef unsigned cz_scope_t;
typedef unsigned cz_label_t;
typedef unsigned cz_var_t;
typedef unsigned cz_func_t;
typedef unsigned cz_in_t;
// typedef unsigned cz_type_id_t;
typedef unsigned cz_entry_id_t;

typedef struct
{
    cz_inst_type_t type;

    union {
        struct {
            cz_type_leaf_t type;
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
            cz_mem_type_t mem_type;
            union {
                cz_var_t var;
                cz_in_t  in;
            };
        } load;

        struct {
            cz_mem_type_t mem_type;
            union {
                cz_var_t var;
                cz_in_t  in;
            };
        } store;

        struct {
            cz_func_t func;
        } call;

        struct {
            cz_mem_type_t mem_type;
            union {
                cz_var_t var;
                cz_in_t  in;
            };
        } ref;

        struct {
            cz_entry_id_t entry_id;
        } select;
    };
} cz_inst_t;

typedef UTILS_STRETCHY_T (cz_inst_t, unsigned) cz_code_t;

typedef struct
{
    cz_type_type_t type;
    union {
        cz_type_leaf_t leaf;
        unsigned struct_index;
    };
} cz_type_t;

typedef struct
{
    cz_var_t  var;
    cz_type_t type;
} cz_variable_t;

typedef UTILS_STRETCHY_T (cz_variable_t, unsigned) cz_variables_t;

typedef struct
{
    cz_in_t   in;
    cz_type_t type;
} cz_input_t;

typedef UTILS_STRETCHY_T (cz_input_t, unsigned) cz_inputs_t;

typedef struct
{
    cz_type_t type;
} cz_result_t;

typedef UTILS_STRETCHY_T (cz_result_t, unsigned) cz_results_t;

typedef struct
{
    cz_func_t func;

    unsigned code_offset;
    unsigned code_count;

    unsigned variable_offset;
    unsigned variable_count;

    unsigned input_offset;
    unsigned input_count;

    unsigned result_offset;
    unsigned result_count;
} cz_function_t;

typedef UTILS_STRETCHY_T (cz_function_t, unsigned) cz_functions_t;

typedef struct
{
    unsigned entry_offset;
    unsigned entry_count;
} cz_type_struct_t;

typedef UTILS_STRETCHY_T (cz_type_struct_t, unsigned) cz_type_structs_t;

typedef struct
{
    cz_type_t type;
} cz_struct_entry_t;

typedef UTILS_STRETCHY_T (cz_struct_entry_t, unsigned) cz_struct_entries_t;

typedef struct
{
    cz_code_t      code;
    cz_variables_t variables;
    cz_inputs_t    inputs;
    cz_results_t   results;
    cz_functions_t functions;

    cz_struct_entries_t struct_entries;
    cz_type_structs_t   structs;

    // TODO: Rethink the identifier situation.
    cz_scope_t last_scope;
    cz_label_t last_label;
    cz_var_t   last_var;
    cz_func_t  last_func;
    cz_in_t    last_in;
} cz_t;

static inline void
cz_emit_inst(cz_t *cz, cz_inst_t inst)
{
    UTILS_STRETCHY_PUSH(cz->code, inst);
}

static inline void
cz_add_variable(cz_t *cz, cz_var_t var, cz_type_t type)
{
    UTILS_STRETCHY_PUSH(cz->variables, (cz_variable_t) {
        .var  = var,
        .type = type,
    });
}

static inline void
cz_add_input(cz_t *cz, cz_in_t in, cz_type_t type)
{
    UTILS_STRETCHY_PUSH(cz->inputs, (cz_input_t) {
        .in   = in,
        .type = type,
    });
}

static inline void
cz_add_result(cz_t *cz, cz_type_t type)
{
    UTILS_STRETCHY_PUSH(cz->results, (cz_result_t) {
        .type = type,
    });
}

static inline void
cz_function_begin(cz_t *cz, cz_func_t func)
{
    UTILS_STRETCHY_PUSH(cz->functions, (cz_function_t) {
        .func            = func,
        .code_offset     = cz->code.count,
        .variable_offset = cz->variables.count,
        .input_offset    = cz->inputs.count,
        .result_offset   = cz->results.count,
    });
}

static inline void
cz_function_end(cz_t *cz, cz_func_t func)
{
    UTILS_ASSERT(cz->functions.count > 0);

    cz_function_t *function = cz->functions.data + cz->functions.count - 1;

    UTILS_ASSERT(function->func == func);

    function->code_count     = cz->code.count      - function->code_offset;
    function->variable_count = cz->variables.count - function->variable_offset;
    function->input_count    = cz->inputs.count    - function->input_offset;
    function->result_count   = cz->results.count   - function->result_offset;
}

static inline cz_entry_id_t
cz_add_entry(cz_t *cz, cz_type_t type)
{
    UTILS_ASSERT(cz->structs.count > 0);

    cz_type_struct_t *type_struct = cz->structs.data + cz->structs.count - 1;

    cz_entry_id_t entry_id = type_struct->entry_count++;

    UTILS_STRETCHY_PUSH(cz->struct_entries, (cz_struct_entry_t) {
        .type = type,
    });

    return entry_id;
}

static inline cz_type_t
cz_struct_begin(cz_t *cz)
{
    cz_type_t type = {
        .type = cz_type_type_Struct,
        .struct_index = cz->structs.count,
    };

    UTILS_STRETCHY_PUSH(cz->structs, (cz_type_struct_t) {
        .entry_offset = cz->struct_entries.count,
    });

    return type;
}

static inline void
cz_struct_end(cz_t *cz, cz_type_t type)
{
    UTILS_ASSERT(type.type         == cz_type_type_Struct);
    UTILS_ASSERT(cz->structs.count > 0);
    UTILS_ASSERT(type.struct_index == cz->structs.count - 1);
}

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
        case cz_inst_Ref:        return "ref";
        case cz_inst_Select:     return "select";
        case cz_inst_Deref:      return "deref";
        case cz_inst_Set:        return "set";
        case cz_inst_Print:      return "print";
        case cz_inst_Cow:        return "cow";
    }

    return "<unknown_inst_type>";
}

#endif // CZ_H_

#ifdef CZ_IMPL
#undef CZ_IMPL

// Nothing?

#endif // CZ_IMPL
