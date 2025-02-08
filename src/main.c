#include "utils.h"

#define VM_IMPL
#include "vm.h"

#define CZ_IMPL
#include "cz.h"

#define CZ2VM_IMPL
#include "cz2vm.h"

#define CZEASY_IMPL
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
        cz_in_t cz_in = CZ_IN(cz, CZ);
        CZ_RES(cz, Func);

        cz_var_t func = CZ_VAR(cz, Func);

        CZ_LOAD(cz, In, cz_in); CZ_INTR(cz, MetaFuncBegin);
        CZ_STORE(cz, Var, func);

        CZ_LOAD(cz, In, cz_in); CZ_INTR(cz, MetaInstCow);
        CZ_LOAD(cz, In, cz_in); CZ_INTR(cz, MetaInstHalt);

        CZ_LOAD(cz, Var, func);
        CZ_LOAD(cz, In, cz_in); CZ_INTR(cz, MetaFuncEnd);

        CZ_LOAD(cz, Var, func);
    }

    vm_debug_info_t vm_debug_info = {0};

    ptrdiff_t start_offset = cz2vm_compile(cz, main_func, c, &vm_debug_info);

    printf("Disassembly:\n");
    vm_disassemble(c, &vm_debug_info);
    printf("End of disassembly.\n");

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    *(cz_t **)data = cz;
    vm_run(c, start_offset, data, data_size);

    cz_func_t gen_func = *(cz_func_t *)(data);

    ptrdiff_t gen_start_offset = cz2vm_compile(cz, gen_func, c, NULL);
    vm_run(c, gen_start_offset, data, data_size);


    return 0;
}

