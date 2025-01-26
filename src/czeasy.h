#ifndef CZEASY_H_
#define CZEASY_H_

#include "cz.h"

#define CZ_FUNC(cz, func_ident) \
    cz_func_t func_ident = cz_function_begin(cz); \
    for (bool __done_##__LINE__ = false; \
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
    cz_add_variable((cz), (cz_type_t) { \
        .type = cz_type_type_Leaf, \
        .leaf = cz_type_##type_sf, \
    })

#define CZ_VAR_TYPE(cz, type) \
    cz_add_variable((cz), type)

#define CZ_IN(cz, type_sf) \
    cz_add_input((cz), (cz_type_t) { \
        .type = cz_type_type_Leaf, \
        .leaf = cz_type_##type_sf, \
    })

#define CZ_IN_TYPE(cz, type) \
    cz_add_input((cz), type)

#define CZ_RES(cz, type_sf) \
    cz_add_result((cz), (cz_type_t) { \
        .type = cz_type_type_Leaf, \
        .leaf = cz_type_##type_sf, \
    })

#define CZ_RES_TYPE(cz, type) \
    cz_add_result((cz), type)

#define CZ_LOAD(cz, mem_sf, var_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Load, \
        .load = { \
            .mem_type = cz_mem_##mem_sf, \
            .var      = (var_id), /* TODO: this should be done differently */ \
        }, \
    })

#define CZ_STORE(cz, mem_sf, var_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Store, \
        .store = { \
            .mem_type = cz_mem_##mem_sf, \
            .var      = (var_id), /* TODO: this should be done differently */ \
        }, \
    })

#define CZ_REF(cz, mem_sf, var_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Ref, \
        .ref = { \
            .mem_type = cz_mem_##mem_sf, \
            .var      = (var_id), /* TODO: this should be done differently */ \
        }, \
    })

#define CZ_SELECT(cz, ent_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Select, \
        .select = { \
            .entry_id = (ent_id), \
        }, \
    })

#define CZ_DEREF(cz) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Deref, \
    })

#define CZ_SET(cz) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Set, \
    })

#define CZ_INDEX(cz) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Index, \
    })

#define CZ_CALL(cz, func_id) \
    cz_emit_inst((cz), (cz_inst_t) { \
        .type = cz_inst_Call, \
        .call = { \
            .func = func_id, \
        }, \
    })

#define CZ_LEAF(type_sf) ((cz_type_t) { .type = cz_type_type_Leaf, .leaf = cz_type_##type_sf })

#endif // CZEASY_H_

#ifdef CZEASY_IMPL
#undef CZEASY_IMPL
#endif // CZEASY_IMPL
