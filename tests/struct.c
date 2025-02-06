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

    cz_type_t struct_1 = cz_struct_begin(cz);
        cz_entry_id_t struct_1__a = cz_add_entry(cz, CZ_LEAF(Int));
        cz_entry_id_t struct_1__b = cz_add_entry(cz, CZ_LEAF(Float));
    cz_struct_end(cz, struct_1);

    CZ_FUNC(cz, main_func) {
        cz_var_t var_1 = CZ_VAR_TYPE(cz, struct_1);

        CZ_REF(cz, Var, var_1);
        CZ_SELECT(cz, struct_1__a);
        CZ_IMM_INT(cz, 123);
        CZ_SET(cz);

        CZ_REF(cz, Var, var_1);
        CZ_SELECT(cz, struct_1__b);
        CZ_IMM_FLOAT(cz, 6.66f);
        CZ_SET(cz);

        CZ_REF(cz, Var, var_1);
        CZ_SELECT(cz, struct_1__b);
        CZ_DEREF(cz);
        CZ_PRINT(cz);

        CZ_REF(cz, Var, var_1);
        CZ_SELECT(cz, struct_1__a);
        CZ_DEREF(cz);
        CZ_PRINT(cz);

        CZ_COW(cz);
    }

    cz2vm_compile(cz, main_func, c, NULL);

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    vm_run(c, data, data_size);

    return 0;
}
