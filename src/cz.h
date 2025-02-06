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

    cz_inst_Index,

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
    cz_type_type_Array,
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
        unsigned array_index;
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
    cz_type_t type;
    unsigned  length;
} cz_type_array_t;

typedef UTILS_STRETCHY_T (cz_type_array_t, unsigned) cz_type_arrays_t;

typedef struct
{
    unsigned code_offset;
    unsigned line_position;
} cz_debug_line_t;

typedef UTILS_STRETCHY_T (cz_debug_line_t, unsigned) cz_debug_lines_t;

typedef struct
{
    const char *file_name;
    unsigned code_offset;
    cz_func_t func;
    unsigned line_offset;
    unsigned line_count;
} cz_debug_function_t;

typedef UTILS_STRETCHY_T (cz_debug_function_t, unsigned) cz_debug_functions_t;

typedef struct
{
    cz_debug_lines_t     lines;
    cz_debug_functions_t functions;
} cz_debug_info_t;

typedef struct
{
    cz_code_t      code;
    cz_variables_t variables;
    cz_inputs_t    inputs;
    cz_results_t   results;
    cz_functions_t functions;

    cz_struct_entries_t struct_entries;
    cz_type_structs_t   structs;
    cz_type_arrays_t    arrays;

    cz_debug_info_t *debug_info;

    cz_label_t last_label; // TODO: Rethink this.
} cz_t;

static inline void
cz_emit_inst(cz_t *cz, cz_inst_t inst)
{
    UTILS_STRETCHY_PUSH(cz->code, inst);
}

static inline cz_function_t *
cz_get_top_func(cz_t *cz)
{
    UTILS_ASSERT(cz->functions.count > 0);
    return cz->functions.data + cz->functions.count - 1;
}

static inline cz_var_t
cz_add_variable(cz_t *cz, cz_type_t type)
{
    cz_var_t var = cz->variables.count - cz_get_top_func(cz)->variable_offset;

    UTILS_STRETCHY_PUSH(cz->variables, (cz_variable_t) {
        .var  = var,
        .type = type,
    });

    return var;
}

static inline cz_in_t
cz_add_input(cz_t *cz, cz_type_t type)
{
    cz_in_t in = cz->inputs.count - cz_get_top_func(cz)->input_offset;

    UTILS_STRETCHY_PUSH(cz->inputs, (cz_input_t) {
        .in   = in,
        .type = type,
    });

    return in;
}

static inline void
cz_add_result(cz_t *cz, cz_type_t type)
{
    UTILS_STRETCHY_PUSH(cz->results, (cz_result_t) {
        .type = type,
    });
}

static inline cz_func_t
cz_function_begin(cz_t *cz)
{
    cz_func_t func = cz->functions.count;

    UTILS_STRETCHY_PUSH(cz->functions, (cz_function_t) {
        .func            = func,
        .code_offset     = cz->code.count,
        .variable_offset = cz->variables.count,
        .input_offset    = cz->inputs.count,
        .result_offset   = cz->results.count,
    });

    return func;
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

static inline cz_type_t
cz_array(cz_t *cz, cz_type_t type, unsigned length)
{
    cz_type_t res = {
        .type        = cz_type_type_Array,
        .array_index = cz->arrays.count,
    };

    UTILS_STRETCHY_PUSH(cz->arrays, (cz_type_array_t) {
        .type   = type,
        .length = length,
    });

    return res;
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
        case cz_inst_Index:      return "index";
        case cz_inst_Print:      return "print";
        case cz_inst_Cow:        return "cow";
    }

    return "<unknown_inst_type>";
}

static inline void
cz_debug_function(cz_t *cz, cz_func_t func, const char *file_name)
{
    if (!cz->debug_info)
        return;

    UTILS_STRETCHY_PUSH(cz->debug_info->functions, (cz_debug_function_t) {
        .file_name   = file_name,
        .code_offset = cz->code.count,
        .func        = func,
        .line_offset = cz->debug_info->lines.count,
    });
}

static inline void
cz_debug_line(cz_t *cz, unsigned line_position)
{
    if (!cz->debug_info)
        return;

    UTILS_ASSERT(cz->debug_info->functions.count > 0);

    cz_debug_function_t *func = cz->debug_info->functions.data + cz->debug_info->functions.count - 1;

    if (func->line_count == 0
     || cz->debug_info->lines.data[cz->debug_info->lines.count - 1].line_position != line_position)
    {
        UTILS_STRETCHY_PUSH(cz->debug_info->lines, (cz_debug_line_t) {
            .line_position = line_position,
            .code_offset   = cz->code.count,
        });

        func->line_count++;
    }
}

#endif // CZ_H_

#ifdef CZ_IMPL
#undef CZ_IMPL

// Nothing?

#endif // CZ_IMPL
