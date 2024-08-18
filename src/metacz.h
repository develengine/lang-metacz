#ifndef METACZ_H_
#define METACZ_H_

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
    bool          is_reference;
} cz_object_t;

typedef struct
{
    unsigned input_offset;
    unsigned input_count;

    unsigned local_offset;
    unsigned local_count;

    unsigned output_offset;
    unsigned output_count;
} cz_func_t;

typedef struct
{
    DCK_STRETCHY_T (cz_inst_t,   unsigned) insts;
    DCK_STRETCHY_T (cz_object_t, unsigned) inputs;
    DCK_STRETCHY_T (cz_object_t, unsigned) outputs;
    DCK_STRETCHY_T (cz_object_t, unsigned) locals;
} cz_func_data_t;

typedef struct
{
    cz_func_data_t rec;
    DCK_STRETCHY_T (cz_func_t, unsigned) rec_funcs;

    cz_func_data_t func;
    DCK_STRETCHY_T (cz_func_t, unsigned) funcs;

    char error[1024];
} cz_t;

typedef enum
{
    cz_obj_Input,
    cz_obj_Local,
    cz_obj_Output,
} cz_obj_tag_t;

typedef struct
{
    unsigned     func_index;
    cz_obj_tag_t tag;
    unsigned     index;
} cz_obj_ref_t;

#endif // METACZ_H_
