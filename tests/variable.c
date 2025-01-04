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

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    vm_run(c, data, data_size);

    return 0;
}
