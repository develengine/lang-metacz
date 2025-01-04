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

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    *(int*)(data) = 10;
    vm_run(c, data, data_size);

    return 0;
}
