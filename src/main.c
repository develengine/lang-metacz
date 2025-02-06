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

    cz_t cz_data = {0};

    cz_debug_info_t debug_info = {0};
    cz_data.debug_info = &debug_info;

    cz_t *cz = &cz_data;
    
    CZ_FUNC(cz, main_func) {
        int def_arr_size = 20;
        cz_type_t arr_type = cz_array(cz, CZ_LEAF(Int), def_arr_size);
        cz_var_t var_1 = CZ_VAR_TYPE(cz, arr_type);
        cz_var_t test = CZ_VAR(cz, Int);
        cz_var_t counter = CZ_VAR(cz, Int);

        CZ_IMM_INT(cz, 0);
        CZ_STORE(cz, Var, counter);

        CZ_SCOPE(cz, loop) {
            cz_label_t top = CZ_LABEL_MAKE(cz);
            CZ_LABEL_SET(cz, top);

            CZ_LOAD(cz, Var, counter);
            CZ_IMM_INT(cz, def_arr_size);
            CZ_OP(cz, LT);

            cz_label_t body = CZ_LABEL_MAKE(cz);
            CZ_JMP(cz, body);
                CZ_BRK(cz, loop);
            CZ_LABEL_SET(cz, body);

            CZ_REF(cz, Var, var_1);
            CZ_LOAD(cz, Var, counter);
            CZ_INDEX(cz);

            CZ_LOAD(cz, Var, counter);
            CZ_IMM_INT(cz, 1);
            CZ_OP(cz, Add);
            CZ_IMM_INT(cz, 2);
            CZ_OP(cz, Mul);

            CZ_SET(cz);

            CZ_LOAD(cz, Var, counter);
            CZ_IMM_INT(cz, 1);
            CZ_OP(cz, Add);
            CZ_STORE(cz, Var, counter);

            CZ_IMM_BOOL(cz, true);
            CZ_JMP(cz, top);
        }

        CZ_IMM_INT(cz, 0);
        CZ_STORE(cz, Var, counter);

        CZ_SCOPE(cz, loop) {
            cz_label_t top = CZ_LABEL_MAKE(cz);
            CZ_LABEL_SET(cz, top);

            CZ_LOAD(cz, Var, counter);
            CZ_IMM_INT(cz, def_arr_size);
            CZ_OP(cz, LT);

            cz_label_t body = CZ_LABEL_MAKE(cz);
            CZ_JMP(cz, body);
                CZ_BRK(cz, loop);
            CZ_LABEL_SET(cz, body);

            CZ_REF(cz, Var, var_1);
            CZ_LOAD(cz, Var, counter);
            CZ_INDEX(cz);
            CZ_DEREF(cz);
            CZ_PRINT(cz);

            CZ_LOAD(cz, Var, counter);
            CZ_IMM_INT(cz, 1);
            CZ_OP(cz, Add);
            CZ_STORE(cz, Var, counter);

            CZ_IMM_BOOL(cz, true);
            CZ_JMP(cz, top);
        }
    }

    vm_debug_info_t vm_debug_info = {0};

    cz2vm_compile(cz, main_func, c, &vm_debug_info);

    printf("Disassembly:\n");
    vm_disassemble(c, &vm_debug_info);
    printf("End of disassambly.\n");

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    *(int*)data = 10;
    vm_run(c, data, data_size);

    return 0;
}

