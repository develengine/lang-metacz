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

