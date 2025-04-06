#include <stdint.h>
#include "greatest/greatest.h"

#define DICT_NAME int_int_map
#define DICT_KEY_TYPE int32_t
#define DICT_VALUE_TYPE int32_t
#define DICT_KEY_INT32
#include "dict.h"
#undef DICT_NAME
#undef DICT_KEY_TYPE
#undef DICT_VALUE_TYPE
#undef DICT_KEY_INT32

TEST test_dict(void) {
    int_int_map *d = int_int_map_new();

    ASSERT(int_int_map_put(d, 1, 1));
    ASSERT(int_int_map_put(d, 2, 2));
    ASSERT(int_int_map_put(d, 3, 3));

    ASSERT_EQ(int_int_map_contains(d, 1), true);
    ASSERT_EQ(int_int_map_contains(d, 2), true);
    ASSERT_EQ(int_int_map_contains(d, 3), true);

    int_int_map *d2 = int_int_map_new();
    ASSERT(int_int_map_put(d2, 2, 2));
    ASSERT(int_int_map_put(d2, 3, 5));
    ASSERT(int_int_map_put(d2, 4, 4));

    size_t key_size;
    int32_t *keys = int_int_map_keys(d, &key_size);
    ASSERT_EQ(key_size, 3);
    for (size_t i = 0; i < key_size; i++) {
        int32_t key = keys[i];
        ASSERT(key == 1 || key == 2 || key == 3);
    }
    free(keys); 

    size_t values_size;
    int32_t *values = int_int_map_values(d2, &values_size);
    ASSERT_EQ(values_size, 3);
    for (size_t i = 0; i < values_size; i++) {
        int32_t value = values[i];
        ASSERT(value == 2 || value == 5 || value == 4);
    }
    free(values);

    for (int_int_map_iterator it = int_int_map_iter(d2); int_int_map_iter_has_next(&it); int_int_map_iter_next(&it)) {
        int32_t key = int_int_map_iter_get_key(it);
        int32_t value = int_int_map_iter_get_value(it);
        ASSERT(key == 2 || key == 3 || key == 4);
        ASSERT(value == 2 || value == 5 || value == 4);
    }

    int_int_map_merge(d, d2);

    ASSERT_EQ(int_int_map_size(d), 4);
    for (int_int_map_iterator it = int_int_map_iter(d); int_int_map_iter_has_next(&it); int_int_map_iter_next(&it)) {
        int32_t key = int_int_map_iter_get_key(it);
        int32_t value = int_int_map_iter_get_value(it);
        ASSERT(key == 1 || key == 2 || key == 3 || key == 4);
        ASSERT(value == 1 || value == 2 || value == 5 || value == 4);
    }

    ASSERT(int_int_map_delete(d, 2));
    ASSERT_EQ(int_int_map_size(d), 3);
    for (int_int_map_iterator it = int_int_map_iter(d); int_int_map_iter_has_next(&it); int_int_map_iter_next(&it)) {
        int32_t key = int_int_map_iter_get_key(it);
        int32_t value = int_int_map_iter_get_value(it);
        ASSERT(key == 1 || key == 3 || key == 4);
        ASSERT(value == 1 || value == 5 || value == 4);
    }

    int_int_map *d3 = int_int_map_copy(d);
    ASSERT_EQ(int_int_map_size(d3), 3);
    for (int_int_map_iterator it = int_int_map_iter(d3); int_int_map_iter_has_next(&it); int_int_map_iter_next(&it)) {
        int32_t key = int_int_map_iter_get_key(it);
        int32_t value = int_int_map_iter_get_value(it);
        ASSERT(key == 1 || key == 3 || key == 4);
        ASSERT(value == 1 || value == 5 || value == 4);
    }

    ASSERT(int_int_map_clear(d3));
    ASSERT_EQ(int_int_map_size(d3), 0);

    int_int_map_destroy(d);
    int_int_map_destroy(d2);
    int_int_map_destroy(d3);
    PASS();
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int32_t main(int32_t argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_dict);

    GREATEST_MAIN_END();        /* display results */
}
