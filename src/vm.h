#ifndef VM_H_
#define VM_H_

#include "utils.h"

typedef enum
{
    vm_inst_Halt,     //
    vm_inst_Cow,      //
    vm_inst_Print,    // [reg]

    vm_inst_LoadImm,  // [reg] ...
    vm_inst_StoreImm, // [mem] ...

    vm_inst_Load,     // [reg]
    vm_inst_Store,    // [reg]

    vm_inst_Pop,      // [reg] [off]
    vm_inst_Push,     // [reg] [off]

    vm_inst_Op,       // [op]
    vm_inst_UnOp,     // [uop]
    vm_inst_ShiftL,   // [typ]
    vm_inst_ShiftR,   // [typ]

    vm_inst_Jmp,      // [off]
    vm_inst_JmpIf,    // [off]
} vm_inst_t;

typedef enum
{
    vm_reg_None,
    vm_reg_Int,
    vm_reg_Float,
    vm_reg_Ptr,
    vm_reg_Char,
    vm_reg_DP,
    vm_reg_SP,
    vm_reg_IP,
} vm_reg_type_t;

typedef enum
{
    vm_type_Int,
    vm_type_Float,
    vm_type_Ptr,
    vm_type_Char,
} vm_type_type_t;

typedef enum
{
    vm_op_Add,
    vm_op_Sub,
    vm_op_Mul,
    vm_op_Div,
    vm_op_Mod,

    vm_op_Or,
    vm_op_And,

    vm_op_BitOr,
    vm_op_BitAnd,
    vm_op_BitXor,

    vm_op_LT,
    vm_op_LE,
    vm_op_GT,
    vm_op_GE,
    vm_op_EQ,
    vm_op_NE,
} vm_op_type_t;

typedef enum
{
    vm_un_op_Not,
    vm_un_op_BitNot,
    vm_un_op_Negate,
} vm_un_op_type_t;

typedef struct
{
    vm_reg_type_t type;
    unsigned index;
} vm_inst_reg_t;

typedef struct
{
    size_t size;
    size_t alignment;
} vm_inst_mem_t;

typedef struct
{
    vm_type_type_t type_type;
    vm_un_op_type_t type;
    unsigned index;
} vm_int_un_op_t;

typedef struct
{
    vm_op_type_t type;
    vm_type_type_t type_type;
} vm_inst_op_t;

typedef struct
{
    int    ints  [2];
    char   chars [2];
    size_t ptrs  [2];
    float  floats[2];

    size_t dp;
    size_t sp;
    size_t ip;
} vm_regs_t;

typedef UTILS_STRETCHY_T (unsigned char, size_t) vm_mem_buf_t;


void
vm_run(vm_mem_buf_t *code, unsigned char *data, size_t data_size);

static inline void
vm_link(vm_mem_buf_t *code, unsigned from, unsigned to)
{
    *((int*)(code->data + from)) = (int)to - ((int)from + sizeof(int));
}

static inline void
vm_mem_buf_reserve(vm_mem_buf_t *mem_buf, size_t size)
{
    UTILS_STRETCHY_RESERVE(*mem_buf, size);
}

static inline void
vm_align(size_t *ptr, size_t alignment)
{
    size_t rem = (*ptr) % alignment;
    if (rem) {
        *ptr += alignment - rem;
    }
}


#define VM_MEM_GET(data, ptr, type) ( \
    vm_align(ptr, _Alignof(type)), \
    *(ptr) += sizeof(type), \
    *((type*)(data + *(ptr) - sizeof(type))) \
)

#define VM_MEM_BUF_PUSH(mem_buf, type, ...) ( \
    vm_align(&((mem_buf)->count), _Alignof(type)), \
    vm_mem_buf_reserve(mem_buf, sizeof(type)), \
    *((type*)((mem_buf)->data + (mem_buf)->count)) = __VA_ARGS__, \
    (mem_buf)->count += sizeof(type), \
    (mem_buf)->count - sizeof(type) \
)

#define VM_IMM_INT(code, ind, ...) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_LoadImm); \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) { \
        .type  = vm_reg_Int, \
        .index = ind, \
    }); \
    (void)VM_MEM_BUF_PUSH(code, int, __VA_ARGS__); \
} while (0)

#define VM_IMM_FLOAT(code, ind, ...) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_LoadImm); \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) { \
        .type  = vm_reg_Float, \
        .index = ind, \
    }); \
    (void)VM_MEM_BUF_PUSH(code, float, __VA_ARGS__); \
} while (0)

#define VM_IMM_PTR(code, ind, ...) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_LoadImm); \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) { \
        .type  = vm_reg_Ptr, \
        .index = ind, \
    }); \
    (void)VM_MEM_BUF_PUSH(code, size_t, __VA_ARGS__); \
} while (0)

#define VM_IMM_CHAR(code, ind, ...) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_LoadImm); \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) { \
        .type  = vm_reg_Char, \
        .index = ind, \
    }); \
    (void)VM_MEM_BUF_PUSH(code, char, __VA_ARGS__); \
} while (0)

#define VM_IMM_DP(code, ...) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_LoadImm); \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) { \
        .type  = vm_reg_DP, \
    }); \
    (void)VM_MEM_BUF_PUSH(code, size_t, __VA_ARGS__); \
} while (0)

static inline void
vm_inst_op(vm_mem_buf_t *code, vm_type_type_t type, vm_op_type_t op)
{
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Op);
    (void)VM_MEM_BUF_PUSH(code, vm_inst_op_t, (vm_inst_op_t) {
        .type      = op,
        .type_type = type,
    });
}

#define VM_OP(code, type_sf, op_sf) \
    vm_inst_op(code, vm_type_##type_sf, vm_op_##op_sf)

#define VM_JMP_IF(code) ( \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_JmpIf), \
    VM_MEM_BUF_PUSH(code, int, 0xDEADC0DE) \
)

static inline void
vm_inst_print(vm_mem_buf_t *code, vm_reg_type_t reg_type, unsigned ind)
{
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Print);
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) {
        .type  = reg_type,
        .index = ind,
    });
}

#define VM_PRINT(code, reg_sf, ind) \
    vm_inst_print(code, vm_reg_##reg_sf, ind)

#define VM_COW(code) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Cow); \
} while (0)

#define VM_HALT(code) \
do { \
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Halt); \
} while (0)

static inline void
vm_inst_push(vm_mem_buf_t *code, vm_reg_type_t reg_type, unsigned ind, int off)
{
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Push);
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) {
        .type = reg_type,
        .index = ind,
    });
    (void)VM_MEM_BUF_PUSH(code, int, off);
}

static inline void
vm_inst_pop(vm_mem_buf_t *code, vm_reg_type_t reg_type, unsigned ind, int off)
{
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Pop);
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) {
        .type = reg_type,
        .index = ind,
    });
    (void)VM_MEM_BUF_PUSH(code, int, off);
}

#define VM_PUSH(code, reg_sf, ind, ...) \
    vm_inst_push(code, vm_reg_##reg_sf, ind, __VA_ARGS__)

#define VM_POP(code, reg_sf, ind, ...) \
    vm_inst_pop(code, vm_reg_##reg_sf, ind, __VA_ARGS__)

static inline void
vm_inst_store(vm_mem_buf_t *code, vm_reg_type_t reg_type, unsigned ind)
{
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Store);
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) {
        .type = reg_type,
        .index = ind,
    });
}

static inline void
vm_inst_load(vm_mem_buf_t *code, vm_reg_type_t reg_type, unsigned ind)
{
    (void)VM_MEM_BUF_PUSH(code, vm_inst_t, vm_inst_Load);
    (void)VM_MEM_BUF_PUSH(code, vm_inst_reg_t, (vm_inst_reg_t) {
        .type = reg_type,
        .index = ind,
    });
}

#define VM_STORE(code, reg_sf, ind) \
    vm_inst_store(code, vm_reg_##reg_sf, ind)

#define VM_LOAD(code, reg_sf, ind) \
    vm_inst_load(code, vm_reg_##reg_sf, ind)


#endif // VM_H_

#ifdef VM_IMPL

void
vm_run(vm_mem_buf_t *code, unsigned char *data, size_t data_size)
{
    (void)data_size;

    vm_regs_t regs = {0};

    for (;;) {
        vm_inst_t inst = VM_MEM_GET(code->data, &regs.ip, vm_inst_t);

        switch (inst) {
            case vm_inst_Halt:
                return;

            case vm_inst_Cow:
                printf("\\_/\n V\n");
                break;

            case vm_inst_LoadImm: {
                vm_inst_reg_t reg = VM_MEM_GET(code->data, &regs.ip, vm_inst_reg_t);

                switch (reg.type) {
                    case vm_reg_None:
                        break;
                    case vm_reg_Int:
                        regs.ints[reg.index] = VM_MEM_GET(code->data, &regs.ip, int);
                        break;
                    case vm_reg_Float:
                        regs.floats[reg.index] = VM_MEM_GET(code->data, &regs.ip, float);
                        break;
                    case vm_reg_Ptr:
                        regs.ptrs[reg.index] = VM_MEM_GET(code->data, &regs.ip, size_t);
                        break;
                    case vm_reg_Char:
                        regs.chars[reg.index] = VM_MEM_GET(code->data, &regs.ip, char);
                        break;
                    case vm_reg_DP:
                        regs.dp = VM_MEM_GET(code->data, &regs.ip, size_t);
                        break;
                    case vm_reg_SP:
                        regs.sp = VM_MEM_GET(code->data, &regs.ip, size_t);
                        break;
                    case vm_reg_IP:
                        regs.ip = VM_MEM_GET(code->data, &regs.ip, size_t);
                        break;
                }
            } break;

            case vm_inst_StoreImm: {
                vm_inst_mem_t mem = VM_MEM_GET(code->data, &regs.ip, vm_inst_mem_t);

                vm_align(&regs.ip, mem.alignment);
                memcpy(data + regs.dp, code->data + regs.ip, mem.size);
                regs.ip += mem.size;
            } break;

            case vm_inst_Load: {
                vm_inst_reg_t reg = VM_MEM_GET(code->data, &regs.ip, vm_inst_reg_t);

                switch (reg.type) {
                    case vm_reg_None:
                        break;
                    case vm_reg_Int:
                        regs.ints[reg.index] = *(int*)(data + regs.dp);
                        break;
                    case vm_reg_Float:
                        regs.floats[reg.index] = *(float*)(data + regs.dp);
                        break;
                    case vm_reg_Ptr:
                        regs.ptrs[reg.index] = *(size_t*)(data + regs.dp);
                        break;
                    case vm_reg_Char:
                        regs.chars[reg.index] = *(char*)(data + regs.dp);
                        break;
                    case vm_reg_DP:
                        regs.dp = *(size_t*)(data + regs.dp);
                        break;
                    case vm_reg_SP:
                        regs.sp = *(size_t*)(data + regs.dp);
                        break;
                    case vm_reg_IP:
                        regs.ip = *(size_t*)(data + regs.dp);
                        break;
                }
            } break;

            case vm_inst_Store: {
                vm_inst_reg_t reg = VM_MEM_GET(code->data, &regs.ip, vm_inst_reg_t);

                switch (reg.type) {
                    case vm_reg_None:
                        break;
                    case vm_reg_Int:
                        *(int*)(data + regs.dp) = regs.ints[reg.index];
                        break;
                    case vm_reg_Float:
                        *(float*)(data + regs.dp) = regs.floats[reg.index];
                        break;
                    case vm_reg_Ptr:
                        *(size_t*)(data + regs.dp) = regs.ptrs[reg.index];
                        break;
                    case vm_reg_Char:
                        *(char*)(data + regs.dp) = regs.chars[reg.index];
                        break;
                    case vm_reg_DP:
                        *(size_t*)(data + regs.dp) = regs.dp;
                        break;
                    case vm_reg_SP:
                        *(size_t*)(data + regs.dp) = regs.sp;
                        break;
                    case vm_reg_IP:
                        *(size_t*)(data + regs.dp) = regs.ip;
                        break;
                }
            } break;

            case vm_inst_Pop: {
                vm_inst_reg_t reg = VM_MEM_GET(code->data, &regs.ip, vm_inst_reg_t);
                size_t off = (size_t)VM_MEM_GET(code->data, &regs.ip, int);

                switch (reg.type) {
                    case vm_reg_None:
                        break;
                    case vm_reg_Int:
                        regs.ints[reg.index] = *(int*)(data + regs.sp);
                        break;
                    case vm_reg_Float:
                        regs.floats[reg.index] = *(float*)(data + regs.sp);
                        break;
                    case vm_reg_Ptr:
                        regs.ptrs[reg.index] = *(size_t*)(data + regs.sp);
                        break;
                    case vm_reg_Char:
                        regs.chars[reg.index] = *(char*)(data + regs.sp);
                        break;
                    case vm_reg_DP:
                        regs.dp = *(size_t*)(data + regs.sp);
                        break;
                    case vm_reg_SP:
                        regs.sp = *(size_t*)(data + regs.sp);
                        break;
                    case vm_reg_IP:
                        regs.ip = *(size_t*)(data + regs.sp);
                        break;
                }

                regs.sp -= off;
            } break;

            case vm_inst_Push: {
                vm_inst_reg_t reg = VM_MEM_GET(code->data, &regs.ip, vm_inst_reg_t);
                size_t off = (size_t)VM_MEM_GET(code->data, &regs.ip, int);

                regs.sp += off;

                switch (reg.type) {
                    case vm_reg_None:
                        break;
                    case vm_reg_Int:
                        *(int*)(data + regs.sp) = regs.ints[reg.index];
                        break;
                    case vm_reg_Float:
                        *(float*)(data + regs.sp) = regs.floats[reg.index];
                        break;
                    case vm_reg_Ptr:
                        *(size_t*)(data + regs.sp) = regs.ptrs[reg.index];
                        break;
                    case vm_reg_Char:
                        *(char*)(data + regs.sp) = regs.chars[reg.index];
                        break;
                    case vm_reg_DP:
                        *(size_t*)(data + regs.sp) = regs.dp;
                        break;
                    case vm_reg_SP:
                        *(size_t*)(data + regs.sp) = regs.sp;
                        break;
                    case vm_reg_IP:
                        *(size_t*)(data + regs.sp) = regs.ip;
                        break;
                }
            } break;

            case vm_inst_Op: {
                vm_inst_op_t op = VM_MEM_GET(code->data, &regs.ip, vm_inst_op_t);

                switch (op.type_type) {
                    case vm_type_Int: {
                        switch (op.type) {
                            case vm_op_Add: regs.ints[0] += regs.ints[1]; break;
                            case vm_op_Sub: regs.ints[0] -= regs.ints[1]; break;
                            case vm_op_Mul: regs.ints[0] *= regs.ints[1]; break;
                            case vm_op_Div: regs.ints[0] /= regs.ints[1]; break;
                            case vm_op_Mod: regs.ints[0] %= regs.ints[1]; break;

                            case vm_op_Or:  regs.ints[0] = regs.ints[0] || regs.ints[1]; break;
                            case vm_op_And: regs.ints[0] = regs.ints[0] && regs.ints[1]; break;

                            case vm_op_BitOr:  regs.ints[0] |= regs.ints[1]; break;
                            case vm_op_BitAnd: regs.ints[0] &= regs.ints[1]; break;
                            case vm_op_BitXor: regs.ints[0] ^= regs.ints[1]; break;

                            case vm_op_LT: regs.ints[0] = regs.ints[0] <  regs.ints[1]; break;
                            case vm_op_LE: regs.ints[0] = regs.ints[0] <= regs.ints[1]; break;
                            case vm_op_GT: regs.ints[0] = regs.ints[0] >  regs.ints[1]; break;
                            case vm_op_GE: regs.ints[0] = regs.ints[0] >= regs.ints[1]; break;
                            case vm_op_EQ: regs.ints[0] = regs.ints[0] == regs.ints[1]; break;
                            case vm_op_NE: regs.ints[0] = regs.ints[0] != regs.ints[1]; break;
                        }
                    } break;
                    case vm_type_Float: {
                        switch (op.type) {
                            case vm_op_Add: regs.floats[0] += regs.floats[1]; break;
                            case vm_op_Sub: regs.floats[0] -= regs.floats[1]; break;
                            case vm_op_Mul: regs.floats[0] *= regs.floats[1]; break;
                            case vm_op_Div: regs.floats[0] /= regs.floats[1]; break;
                            case vm_op_Mod:
                                fprintf(stderr, "Modulo is not supported for floating point registers!\n"); 
                                exit(1);

                            case vm_op_Or: /* fallthrough */
                            case vm_op_And:
                                fprintf(stderr, "Logical operations are not supported for floating point registers!\n"); 
                                exit(1);

                            case vm_op_BitOr:  /* fallthrough */
                            case vm_op_BitAnd: /* fallthrough */
                            case vm_op_BitXor:
                                fprintf(stderr, "Bit operations are not supported for floating point registers!\n"); 
                                exit(1);

                            case vm_op_LT: regs.ints[0] = regs.floats[0] <  regs.floats[1]; break;
                            case vm_op_LE: regs.ints[0] = regs.floats[0] <= regs.floats[1]; break;
                            case vm_op_GT: regs.ints[0] = regs.floats[0] >  regs.floats[1]; break;
                            case vm_op_GE: regs.ints[0] = regs.floats[0] >= regs.floats[1]; break;
                            case vm_op_EQ: regs.ints[0] = regs.floats[0] == regs.floats[1]; break;
                            case vm_op_NE: regs.ints[0] = regs.floats[0] != regs.floats[1]; break;
                        }
                    } break;
                    case vm_type_Ptr: {
                        switch (op.type) {
                            case vm_op_Add: regs.ptrs[0] += regs.ptrs[1]; break;
                            case vm_op_Sub: regs.ptrs[0] -= regs.ptrs[1]; break;
                            case vm_op_Mul: regs.ptrs[0] *= regs.ptrs[1]; break;
                            case vm_op_Div: regs.ptrs[0] /= regs.ptrs[1]; break;
                            case vm_op_Mod: regs.ptrs[0] %= regs.ptrs[1]; break;

                            case vm_op_Or: /* fallthrough */
                            case vm_op_And:
                                fprintf(stderr, "Logical operations are not supported for pointer registers!\n"); 
                                exit(1);

                            case vm_op_BitOr:  regs.ptrs[0] |= regs.ptrs[1]; break;
                            case vm_op_BitAnd: regs.ptrs[0] &= regs.ptrs[1]; break;
                            case vm_op_BitXor: regs.ptrs[0] ^= regs.ptrs[1]; break;

                            case vm_op_LT: regs.ints[0] = regs.ptrs[0] <  regs.ptrs[1]; break;
                            case vm_op_LE: regs.ints[0] = regs.ptrs[0] <= regs.ptrs[1]; break;
                            case vm_op_GT: regs.ints[0] = regs.ptrs[0] >  regs.ptrs[1]; break;
                            case vm_op_GE: regs.ints[0] = regs.ptrs[0] >= regs.ptrs[1]; break;
                            case vm_op_EQ: regs.ints[0] = regs.ptrs[0] == regs.ptrs[1]; break;
                            case vm_op_NE: regs.ints[0] = regs.ptrs[0] != regs.ptrs[1]; break;
                        }
                    } break;
                    case vm_type_Char: {
                        switch (op.type) {
                            case vm_op_Add: regs.chars[0] += regs.chars[1]; break;
                            case vm_op_Sub: regs.chars[0] -= regs.chars[1]; break;
                            case vm_op_Mul: regs.chars[0] *= regs.chars[1]; break;
                            case vm_op_Div: regs.chars[0] /= regs.chars[1]; break;
                            case vm_op_Mod: regs.chars[0] %= regs.chars[1]; break;

                            case vm_op_Or: /* fallthrough */
                            case vm_op_And:
                                fprintf(stderr, "Logical operations are not supported for character registers!\n"); 
                                exit(1);

                            case vm_op_BitOr:  regs.chars[0] |= regs.chars[1]; break;
                            case vm_op_BitAnd: regs.chars[0] &= regs.chars[1]; break;
                            case vm_op_BitXor: regs.chars[0] ^= regs.chars[1]; break;

                            case vm_op_LT: regs.ints[0] = regs.chars[0] <  regs.chars[1]; break;
                            case vm_op_LE: regs.ints[0] = regs.chars[0] <= regs.chars[1]; break;
                            case vm_op_GT: regs.ints[0] = regs.chars[0] >  regs.chars[1]; break;
                            case vm_op_GE: regs.ints[0] = regs.chars[0] >= regs.chars[1]; break;
                            case vm_op_EQ: regs.ints[0] = regs.chars[0] == regs.chars[1]; break;
                            case vm_op_NE: regs.ints[0] = regs.chars[0] != regs.chars[1]; break;
                        }
                    } break;
                    default:
                        fprintf(stderr, "Unsupported type for operations (%d)\n", op.type_type);
                        exit(1);
                }
            } break;

            case vm_inst_ShiftL: {
                vm_type_type_t type = VM_MEM_GET(code->data, &regs.ip, vm_type_type_t);

                switch (type) {
                    case vm_type_Int:  regs.ints [0] <<= regs.ints[1]; break;
                    case vm_type_Char: regs.chars[0] <<= regs.ints[1]; break;
                    case vm_type_Ptr:  regs.ptrs [0] <<= regs.ints[1]; break;
                    case vm_type_Float:
                        fprintf(stderr, "Shift left isn't supported for floating point types\n");
                        exit(1);
                }
            } break;

            case vm_inst_ShiftR: {
                vm_type_type_t type = VM_MEM_GET(code->data, &regs.ip, vm_type_type_t);

                switch (type) {
                    case vm_type_Int:  regs.ints [0] >>= regs.ints[1]; break;
                    case vm_type_Char: regs.chars[0] >>= regs.ints[1]; break;
                    case vm_type_Ptr:  regs.ptrs [0] >>= regs.ints[1]; break;
                    case vm_type_Float:
                        fprintf(stderr, "Shift right isn't supported for floating point types\n");
                        exit(1);
                }
            } break;

            case vm_inst_UnOp: {
                vm_int_un_op_t un_op = VM_MEM_GET(code->data, &regs.ip, vm_int_un_op_t);
                
                switch (un_op.type_type) {
                    case vm_type_Int: {
                        switch (un_op.type) {
                            case vm_un_op_Not:    regs.ints[un_op.index] = !regs.ints[un_op.index]; break;
                            case vm_un_op_BitNot: regs.ints[un_op.index] = ~regs.ints[un_op.index]; break;
                            case vm_un_op_Negate: regs.ints[un_op.index] = -regs.ints[un_op.index]; break;
                        }
                    } break;
                    case vm_type_Char: {
                        switch (un_op.type) {
                            case vm_un_op_Not:    regs.chars[un_op.index] = !regs.chars[un_op.index]; break;
                            case vm_un_op_BitNot: regs.chars[un_op.index] = ~regs.chars[un_op.index]; break;
                            case vm_un_op_Negate: regs.chars[un_op.index] = -regs.chars[un_op.index]; break;
                        }
                    } break;
                    case vm_type_Ptr: {
                        switch (un_op.type) {
                            case vm_un_op_Not:    regs.ptrs[un_op.index] = !regs.ptrs[un_op.index]; break;
                            case vm_un_op_BitNot: regs.ptrs[un_op.index] = ~regs.ptrs[un_op.index]; break;
                            case vm_un_op_Negate: regs.ptrs[un_op.index] = -regs.ptrs[un_op.index]; break;
                        }
                    } break;
                    case vm_type_Float: {
                        switch (un_op.type) {
                            case vm_un_op_Not:
                                fprintf(stderr, "Logical inversion isn't supported for floating point registers\n");
                                exit(1);
                            case vm_un_op_BitNot: regs.ptrs[un_op.index] = ~regs.ptrs[un_op.index]; break;
                                fprintf(stderr, "Bit complement isn't supported for floating point registers\n");
                                exit(1);
                            case vm_un_op_Negate: regs.floats[un_op.index] = -regs.floats[un_op.index]; break;
                        }
                    } break;
                }
            } break;

            case vm_inst_Jmp: {
                int off = VM_MEM_GET(code->data, &regs.ip, int);

                if (off < 0) {
                    regs.ip -= (size_t)(off * -1);
                }
                else {
                    regs.ip += (size_t)(off);
                }
            } break;

            case vm_inst_JmpIf: {
                int off = VM_MEM_GET(code->data, &regs.ip, int);

                if (regs.ints[0]) {
                    if (off < 0) {
                        regs.ip -= (size_t)(off * -1);
                    }
                    else {
                        regs.ip += (size_t)(off);
                    }
                }
            } break;

            case vm_inst_Print: {
                vm_inst_reg_t reg = VM_MEM_GET(code->data, &regs.ip, vm_inst_reg_t);

                switch (reg.type) {
                    case vm_reg_None:
                        printf("None\n");
                        break;
                    case vm_reg_Int:
                        printf("%d\n", regs.ints[reg.index]);
                        break;
                    case vm_reg_Float:
                        printf("%f\n", regs.floats[reg.index]);
                        break;
                    case vm_reg_Ptr:
                        printf("%zu\n", regs.ptrs[reg.index]);
                        break;
                    case vm_reg_Char:
                        printf("%c\n", regs.chars[reg.index]);
                        break;
                    case vm_reg_DP:
                        printf("%zu\n", regs.dp);
                        break;
                    case vm_reg_SP:
                        printf("%zu\n", regs.sp);
                        break;
                    case vm_reg_IP:
                        printf("%zu\n", regs.ip);
                        break;
                }
            } break;

            default:
                fprintf(stderr, "Unknown instruction: (%u)\n", inst);
                exit(1);
        }
    }
}

#endif
