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
    cz_t *cz = &cz_data;
    
    cz_type_t struct_1 = cz_struct_begin(cz);
    cz_entry_id_t struct_1__a = cz_add_entry(cz, (cz_type_t) { .type = cz_type_type_Leaf, .leaf = cz_type_Int });
    cz_entry_id_t struct_1__b = cz_add_entry(cz, (cz_type_t) { .type = cz_type_type_Leaf, .leaf = cz_type_Float });
    cz_entry_id_t struct_1__c = cz_add_entry(cz, (cz_type_t) { .type = cz_type_type_Leaf, .leaf = cz_type_Bool });
    cz_struct_end(cz, struct_1);

    CZ_FUNC(cz, main_func) {
        cz_var_t var_1 = CZ_VAR_TYPE(cz, struct_1);

        cz_emit_inst(cz, (cz_inst_t) {
            .type = cz_inst_Ref,
            .ref  = {
                .mem_type = cz_mem_Var,
                .var      = var_1,
            },
        });
        cz_emit_inst(cz, (cz_inst_t) {
            .type   = cz_inst_Select,
            .select = {
                .entry_id = struct_1__a,
            },
        });
        CZ_IMM_INT(cz, 123);
        cz_emit_inst(cz, (cz_inst_t) {
            .type   = cz_inst_Set,
        });


        cz_emit_inst(cz, (cz_inst_t) {
            .type = cz_inst_Ref,
            .ref  = {
                .mem_type = cz_mem_Var,
                .var      = var_1,
            },
        });
        cz_emit_inst(cz, (cz_inst_t) {
            .type   = cz_inst_Select,
            .select = {
                .entry_id = struct_1__a,
            },
        });
        cz_emit_inst(cz, (cz_inst_t) {
            .type   = cz_inst_Deref,
        });

        CZ_PRINT(cz);

        CZ_COW(cz);
    }

    cz2vm_compile(cz, main_func, c);

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

