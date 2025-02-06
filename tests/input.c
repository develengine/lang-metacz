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
        CZ_IMM_CHAR(cz, 'A');
        CZ_PRINT(cz);
    }

    CZ_FUNC(cz, main_func) {

        CZ_CALL(cz, print_a_func);

        CZ_COW(cz);

        // CZ_CALL(cz, main_func);
    }

    cz2vm_compile(cz, main_func, c, NULL);

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    vm_run(c, data, data_size);

    return 0;
}
