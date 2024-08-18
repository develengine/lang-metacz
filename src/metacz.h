#ifndef METACZ_H_
#define METACZ_H_


#include "utils.h"


typedef struct
{
    unsigned short greater;
    unsigned short lesser;
} cz_op_code_t;

typedef union
{
    cz_op_code_t  op_code;
    unsigned      uint;
    unsigned char bytes[4];
} cz_inst_t;

typedef enum
{
    cz_type_Basic,
    cz_type_Array,
} cz_type_tag_t;

typedef enum
{
    cz_type_basic_Int,
} cz_type_basic_tag_t;

typedef struct
{
    cz_type_tag_t tag;
    unsigned      index;
} cz_type_ref_t;

typedef struct
{
    cz_type_ref_t type_ref;
    unsigned      length;
} cz_type_array_t;

typedef struct
{
    cz_type_ref_t type_ref;
    bool          is_reference;
} cz_object_t;

typedef struct
{
    unsigned label_offset;
    unsigned label_count;
    unsigned stack_base;
    unsigned stack_diff;
} cz_scope_t;

typedef struct
{
    unsigned code_index;
} cz_label_t;

typedef struct
{
    unsigned index;
} cz_scope_ref_t;

typedef struct
{
    unsigned scope_index;
    unsigned index;
} cz_label_ref_t;

typedef struct
{
    unsigned code_offset;
    unsigned code_count;

    unsigned input_offset;
    unsigned input_count;

    unsigned local_offset;
    unsigned local_count;

    unsigned output_offset;
    unsigned output_count;

    unsigned parent_index;
    unsigned stack_diff;
} cz_func_t;

typedef struct
{
    unsigned code_offset;
    unsigned code_count;

    unsigned input_offset;
    unsigned input_count;

    unsigned local_offset;
    unsigned local_count;

    unsigned output_offset;
    unsigned output_count;

    unsigned func_index;
    unsigned scope_index;
} cz_rec_func_t;

typedef struct
{
    unsigned index;
} cz_func_ref_t;

typedef struct
{
    DCK_STRETCHY_T (cz_inst_t,   unsigned) code;
    DCK_STRETCHY_T (cz_object_t, unsigned) inputs;
    DCK_STRETCHY_T (cz_object_t, unsigned) outputs;
    DCK_STRETCHY_T (cz_object_t, unsigned) locals;
} cz_func_data_t;

typedef struct
{
    DCK_STRETCHY_T (cz_func_t, unsigned) funcs;
    cz_func_data_t func;

    DCK_STRETCHY_T (cz_rec_func_t, unsigned) rec_funcs;
    cz_func_data_t rec;

    unsigned stack_size;

    DCK_STRETCHY_T (cz_scope_t, unsigned) scopes;
    DCK_STRETCHY_T (cz_label_t, unsigned) labels;

    char error[1024];
} cz_t;

typedef enum
{
    cz_obj_Input,
    cz_obj_Local,
    cz_obj_Output,
    cz_obj_Global,
} cz_obj_tag_t;

typedef struct
{
    unsigned     func_index;
    cz_obj_tag_t tag;
    unsigned     index;
} cz_obj_ref_t;


void
cz_abort_if_error(cz_t *cz);

cz_func_ref_t
cz_func_begin(cz_t *cz);

void
cz_func_end(cz_t *cz);


#endif // METACZ_H_
