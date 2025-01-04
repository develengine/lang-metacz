#include "utils.h"

#define VM_IMPL
#include "vm.h"

#define CZ_IMPL
#include "cz.h"

#define CZ2VM_IMPL
#include "cz2vm.h"

#define CZ_CZEASY
#include "czeasy.h"

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

    cz2vm_compile(cz, test_func, c);
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

    cz2vm_compile(cz, test_func, c);
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

    cz2vm_compile(cz, test_func, c);
#elif 0
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, test_func) {
        cz_var_t var_1 = CZ_VAR(cz, Int);
        CZ_IMM_INT(cz, 1);
        CZ_STORE(cz, Var, var_1);

        CZ_SCOPE(cz, scope) {
            cz_label_t l_start = CZ_LABEL_MAKE(cz);
            CZ_LABEL_SET(cz, l_start);

            CZ_LOAD(cz, Var, var_1);
            CZ_PRINT(cz);

            CZ_LOAD(cz, Var, var_1);
            CZ_IMM_INT(cz, 123);

            CZ_OP(cz, GT);

            cz_label_t l_end = CZ_LABEL_MAKE(cz);
            CZ_JMP(cz, l_end);

            CZ_LOAD(cz, Var, var_1);
            CZ_IMM_INT(cz, 2);
            CZ_OP(cz, Mul);

            CZ_STORE(cz, Var, var_1);

            CZ_IMM_BOOL(cz, true);
            CZ_JMP(cz, l_start);

            CZ_LABEL_SET(cz, l_end);

            CZ_BRK(cz, scope);
        }

        CZ_COW(cz);
        CZ_HALT(cz);
    }

    cz2vm_compile(cz, test_func, c);
#elif 0
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

    cz2vm_compile(cz, main_func, c);
#else
    cz_t cz_ctx = {0};
    cz_t *cz = &cz_ctx;

    CZ_FUNC(cz, print_a_func) {
        cz_in_t count = CZ_IN(cz, Int);
        CZ_RES(cz, Int);

        cz_var_t loops_left = CZ_VAR(cz, Int);
        CZ_LOAD(cz, In, count);
        CZ_STORE(cz, Var, loops_left);

        CZ_SCOPE(cz, loop) {
            cz_label_t l_continue = CZ_LABEL_MAKE(cz);
            CZ_LABEL_SET(cz, l_continue);

            CZ_LOAD(cz, Var, loops_left);
            CZ_IMM_INT(cz, 0);
            CZ_OP(cz, GT);

            cz_label_t l_print = CZ_LABEL_MAKE(cz);
            CZ_JMP(cz, l_print);

                CZ_BRK(cz, loop);

            CZ_LABEL_SET(cz, l_print);

                CZ_IMM_CHAR(cz, 'A');
                CZ_PRINT(cz);

                CZ_LOAD(cz, Var, loops_left);
                CZ_IMM_INT(cz, 1);
                CZ_OP(cz, Sub);
                CZ_STORE(cz, Var, loops_left);

            CZ_IMM_BOOL(cz, true);
            CZ_JMP(cz, l_continue);
        }

        CZ_LOAD(cz, In, count);
        CZ_IMM_INT(cz, 3);
        CZ_OP(cz, Mul);
    }

    CZ_FUNC(cz, main_func) {
        cz_in_t count = CZ_IN(cz, Int);

        // CZ_IMM_INT(cz, 10);
        CZ_LOAD(cz, In, count);
        CZ_CALL(cz, print_a_func);
        CZ_PRINT(cz);

        CZ_COW(cz);
    }

    cz2vm_compile(cz, main_func, c);
#endif

    printf("Disassembly:\n");
    vm_disassemble(c);
    printf("End of disassambly.\n");

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    *(int*)data = 10;
    vm_run(c, data, data_size);

    return 0;
}

