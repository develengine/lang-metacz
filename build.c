#define BLD_IMPLEMENTATION
#include "core/bld.h"

#include "core/utils.h"

i32
default_target(i32 argc, char *argv[])
{
    char *output = "program";

    bld_sa_t cc = {0};

    BLD_SA_PUSH(cc, "src/main.c");

    BLD_SA_PUSH(cc, "-I.", "-Isrc", "-o", output, BLD_WARNINGS);

    if (bld_contains("debug", argc, argv)) {
        BLD_SA_PUSH(cc, "-D_DEBUG");
    }

    u32 res = bld_cc_params((const char **)cc.data, cc.count);
    if (res != 0)
        return res;

    if (bld_contains("run", argc, argv)) {
        return bld_run_program(output);
    }
}

int
files_equal(const char *path_1, const char *path_2);

typedef struct
{
    const char *name;
} test_t;

i32
test_target(i32 argc, char *argv[])
{
    bld_sa_t cc = {0};

    test_t tests[] = {
        { .name = "vm_basic" },
        { .name = "jmp" },
        { .name = "bool" },
        { .name = "scope" },
        { .name = "variable" },
        { .name = "input" },
        { .name = "return" },
    };

    for (u32 i = 0; i < BLD_LEN(tests); ++i) {
        test_t test = tests[i];

        cc.count = 0;

        char *input  = bld_strf("tests/%s.c",   test.name);
        char *output = bld_strf("tests/bin/%s", test.name);

        BLD_SA_PUSH(cc, input, "-I.", "-Isrc", "-o", output, BLD_WARNINGS, "-D_DEBUG");

        u32 res = bld_cc_params((const char **)cc.data, cc.count);
        if (res != 0) {
            printf("[FAILURE] %s failed to compile!\n", test.name);
            return res;
        }

        char *check_file = bld_strf("tests/out/%s.out", test.name);
        char *dest_file  = check_file;
        int checking = false;

        if (bld_exists(check_file)) {
            checking = true;
            dest_file = bld_strf("%s.tmp", check_file);
        }
        else {
            printf("[INFO] Check file '%s' not found, creating it.\n", check_file);
        }

        res = bld_execute("./%s > %s", output, dest_file);
        if (res != 0) {
            printf("[FAILURE] %s returned status code '%d'!\n", test.name, res);
            return res;
        }

        if (checking) {
            if (!files_equal(check_file, dest_file)) {
                fprintf(stderr, "[FAILURE] %s has different output!\n", test.name);
                return 1;
            }

            remove(dest_file);
            printf("[SUCCESS] %s\n", test.name);
        }
    }

    printf("%ld test(s) passed succesfully!\n", BLD_LEN(tests));
}

i32
main(i32 argc, char *argv[])
{
    BLD_TRY_REBUILD_SELF(argc, argv);

    if (bld_contains("test", argc, argv))
        return test_target(argc, argv);

    return default_target(argc, argv);
}

int
files_equal(const char *path_1, const char *path_2)
{
    FILE *file_1 = fopen(path_1, "rb");
    if (!file_1) {
        fprintf(stderr, "File '%s' not found!\n", path_1);
        return 0;
    }
    FILE *file_2 = fopen(path_2, "rb");
    if (!file_2) {
        fprintf(stderr, "File '%s' not found!\n", path_2);
        return 0;
    }

    static char buffer_1[512];
    static char buffer_2[512];

    size_t line_number   = 1;
    size_t column_number = 1;

    for (;;) {
        size_t buffer_1_len = fread(buffer_1, 1, BLD_LEN(buffer_1), file_1);
        size_t buffer_2_len = fread(buffer_2, 1, BLD_LEN(buffer_2), file_2);

        size_t min_len = buffer_1_len;
        if (buffer_2_len < buffer_1_len) {
            min_len = buffer_2_len;
        }

        for (size_t i = 0; i < min_len; ++i) {
            if (buffer_1[i] != buffer_2[i]) {
                fprintf(stderr, "Difference between %s and %s! %ld:%ld\n", path_1, path_2, line_number, column_number);
                return 0;
            }

            if (buffer_1[i] == '\n') {
                ++line_number;
                column_number = 1;
            }
            else {
                ++column_number;
            }
        }

        if (buffer_1_len != buffer_2_len) {
            fprintf(stderr, "Difference in file length between %s and %s!\n", path_1, path_2);
            return 0;
        }

        if (buffer_1_len < BLD_LEN(buffer_1))
            break;
    }

    fclose(file_1);
    fclose(file_2);

    return 1;
}

