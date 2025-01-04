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

    ptrdiff_t data_size = 4096;
    unsigned char *data = malloc(data_size);
    UTILS_ASSERT(data);

    vm_run(c, data, data_size);

    return 0;
}
