#ifndef HASHES_H
#define HASHES_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "khash/khash.h"

#endif

#ifndef DICT_NAME
#error "DICT_NAME must be defined"
#endif

#ifndef DICT_KEY_TYPE
#error "DICT_KEY_TYPE must be defined"
#endif

#ifndef DICT_VALUE_TYPE
#error "DICT_VALUE_TYPE must be defined"
#endif

#if defined(DICT_KEY_STR)
#ifndef DICT_HASH_EQUAL
#define DICT_HASH_EQUAL kh_str_hash_equal
#endif
#ifndef DICT_HASH_FUNC
#define DICT_HASH_FUNC kh_str_hash_func
#endif
#endif

#if defined(DICT_KEY_INT32)
#ifndef DICT_HASH_EQUAL
#define DICT_HASH_EQUAL kh_int_hash_equal
#endif
#ifndef DICT_HASH_FUNC
#define DICT_HASH_FUNC kh_int_hash_func
#endif
#endif

#if defined(DICT_KEY_INT64)
#ifndef DICT_HASH_EQUAL
#define DICT_HASH_EQUAL kh_int64_hash_equal
#endif
#ifndef DICT_HASH_FUNC
#define DICT_HASH_FUNC kh_int64_hash_func
#endif
#endif

#ifndef DICT_HASH_FUNC
#error "DICT_HASH_FUNC must be defined, or {DICT_KEY_STR, DICT_KEY_INT32, DICT_KEY_INT64} must be defined"
#endif

#ifdef DICT_MALLOC
#define KH_ORIG_MALLOC kmalloc
#define kmalloc(Z) DICT_MALLOC(Z)
#endif

#ifdef DICT_CALLOC
#define KH_ORIG_CALLOC kcalloc
#define kcalloc(N,Z) DICT_CALLOC(N,Z)
#endif

#ifdef DICT_REALLOC
#define KH_ORIG_REALLOC krealloc
#define krealloc(P,Z) DICT_REALLOC(P,Z)
#endif

#ifdef DICT_FREE
#define KH_ORIG_FREE kfree
#define kfree(P) DICT_FREE(P)
#endif

#if defined(DICT_KEY_COPY) && !defined(DICT_KEY_FREE)
#error "DICT_KEY_FREE must be defined if DICT_KEY_COPY is defined"
#endif

#if defined(DICT_KEY_FREE) && !defined(DICT_KEY_COPY)
#error "DICT_KEY_COPY must be defined if DICT_KEY_FREE is defined"
#endif

#if defined(DICT_VALUE_COPY) && !defined(DICT_VALUE_FREE)
#error "DICT_VALUE_FREE must be defined if DICT_VALUE_COPY is defined"
#endif

#if defined(DICT_VALUE_FREE) && !defined(DICT_VALUE_COPY)
#error "DICT_VALUE_COPY must be defined if DICT_VALUE_FREE is defined"
#endif

#define IS_MAP 1
#define DICT_INIT(name, key_type, value_type, hash_func, equal_func) KHASH_INIT(name, key_type, value_type, IS_MAP, hash_func, equal_func)

DICT_INIT(DICT_NAME, DICT_KEY_TYPE, DICT_VALUE_TYPE, DICT_HASH_FUNC, DICT_HASH_EQUAL)

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT3_(a, b, c) a ## b ## c
#define CONCAT3(a, b, c) CONCAT3_(a, b, c)

#define KH_TYPE(name) CONCAT3(kh_, name, _t)
typedef KH_TYPE(DICT_NAME) DICT_NAME;
#define KH_FUNC(func, name) CONCAT3(func, _, name)

#define DICT_TYPE KH_TYPE(DICT_NAME)
#define DICT_NAME_TYPE CONCAT(DICT_NAME, _t)
#define DICT_TYPED(name) CONCAT(DICT_NAME, _##name)
#define DICT_FUNC(name) CONCAT(DICT_NAME, _##name)

static inline DICT_TYPE *DICT_FUNC(new)() {
    return KH_FUNC(kh_init, DICT_NAME)();
}

static inline void DICT_FUNC(destroy)(DICT_TYPE *dict) {
    KH_FUNC(kh_destroy, DICT_NAME)(dict);
}

static inline bool DICT_FUNC(contains)(DICT_TYPE *dict, DICT_KEY_TYPE key) {
    khiter_t k = KH_FUNC(kh_get, DICT_NAME)(dict, key);
    return k != kh_end(dict);
}

static inline bool DICT_FUNC(get)(DICT_TYPE *dict, DICT_KEY_TYPE key, DICT_VALUE_TYPE *value) {
    khiter_t k = KH_FUNC(kh_get, DICT_NAME)(dict, key);
    if (k == kh_end(dict)) return false;
    *value = kh_value(dict, k);
    return true;
}

static inline bool DICT_FUNC(put)(DICT_TYPE *dict, DICT_KEY_TYPE key, DICT_VALUE_TYPE value) {
    int exists;
    #ifdef DICT_KEY_COPY
    DICT_KEY_TYPE key_copy = DICT_KEY_COPY(key);
    khiter_t k = KH_FUNC(kh_put, DICT_NAME)(dict, key_copy, &exists);
    if (k == kh_end(dict)) {
        DICT_KEY_FREE(key_copy);
        return false;
    }
    #else
    khiter_t k = KH_FUNC(kh_put, DICT_NAME)(dict, key, &exists);
    if (k == kh_end(dict)) return false;
    #endif

    #ifdef DICT_VALUE_COPY
    DICT_VALUE_TYPE value_copy = DICT_VALUE_COPY(value);
    kh_value(dict, k) = value_copy;
    #else
    kh_value(dict, k) = value;
    #endif
    return true;
}

static inline bool DICT_FUNC(delete)(DICT_TYPE *dict, DICT_KEY_TYPE key) {
    khiter_t k = KH_FUNC(kh_get, DICT_NAME)(dict, key);
    if (k == kh_end(dict)) return false;

    #ifdef DICT_KEY_FREE
    DICT_KEY_TYPE existing_key = kh_key(dict, k);
    #endif
    #ifdef DICT_VALUE_FREE
    DICT_VALUE_TYPE existing_value = kh_value(dict, k);
    #endif

    KH_FUNC(kh_del, DICT_NAME)(dict, k);

    #ifdef DICT_KEY_FREE
    DICT_KEY_FREE(existing_key);
    #endif
    #ifdef DICT_VALUE_FREE
    DICT_VALUE_FREE(existing_value);
    #endif
    return true;
}

static inline bool DICT_FUNC(clear)(DICT_TYPE *dict) {
    KH_FUNC(kh_clear, DICT_NAME)(dict);
    return true;
}

static inline size_t DICT_FUNC(size)(DICT_TYPE *dict) {
    return kh_size(dict);
}

static inline bool DICT_FUNC(is_empty)(DICT_TYPE *dict) {
    return kh_size(dict) == 0;
}

static DICT_TYPE *DICT_FUNC(copy)(DICT_TYPE *dict) {
    DICT_TYPE *copy = DICT_FUNC(new)();
    khiter_t k;
    khiter_t end = kh_end(dict);
    for (k = kh_begin(dict); k != end; ++k) {
        if (!kh_exist(dict, k)) continue;
        if (!DICT_FUNC(put)(copy, kh_key(dict, k), kh_value(dict, k))) {
            DICT_FUNC(destroy)(copy);
            return NULL;
        }
    }
    return copy;
}

static bool DICT_FUNC(merge)(DICT_TYPE *dict, DICT_TYPE *other) {
    khiter_t k;
    khiter_t end = kh_end(other);
    for (k = kh_begin(other); k != end; ++k) {
        if (kh_exist(other, k)) {
            DICT_KEY_TYPE key = kh_key(other, k);
            DICT_FUNC(put)(dict, key, kh_value(other, k));
        }
    }
    return true;
}

// Iterator types and functions
typedef struct {
    DICT_TYPE *dict;
    khiter_t current;
    khiter_t end;
} CONCAT(DICT_NAME, _iterator);

typedef CONCAT(DICT_NAME, _iterator) DICT_TYPED(iterator);

static inline DICT_TYPED(iterator) DICT_TYPED(iter)(DICT_TYPE *dict) {
    DICT_TYPED(iterator) it = {
        .dict = dict,
        .current = kh_begin(dict),
        .end = kh_end(dict)
    };
    // Skip any empty slots at the beginning
    while (it.current != it.end && !kh_exist(dict, it.current)) {
        it.current++;
    }
    return it;
}

static inline bool DICT_FUNC(iter_has_next)(DICT_TYPED(iterator) *it) {
    return it->current != it->end;
}

static inline void DICT_FUNC(iter_next)(DICT_TYPED(iterator) *it) {
    if (!DICT_FUNC(iter_has_next)(it)) return;
    for (it->current++; it->current != it->end && !kh_exist(it->dict, it->current); it->current++);
}

static inline DICT_KEY_TYPE DICT_FUNC(iter_get_key)(DICT_TYPED(iterator) it) {
    return kh_key(it.dict, it.current);
}

static inline DICT_VALUE_TYPE DICT_FUNC(iter_get_value)(DICT_TYPED(iterator) it) {
    return kh_value(it.dict, it.current);
}

static DICT_KEY_TYPE *DICT_FUNC(keys)(DICT_TYPE *dict, size_t *keys_size) {
    size_t n = kh_size(dict);
    DICT_KEY_TYPE *keys = kmalloc(sizeof(DICT_KEY_TYPE) * n);
    size_t i = 0;
    DICT_KEY_TYPE key;
    DICT_VALUE_TYPE value;
    kh_foreach(dict, key, value, {
        keys[i] = (DICT_KEY_TYPE)key;
        i++;
    });
    *keys_size = i;
    return keys;
}

static DICT_VALUE_TYPE *DICT_FUNC(values)(DICT_TYPE *dict, size_t *values_size) {
    size_t n = kh_size(dict);
    DICT_VALUE_TYPE *values = kmalloc(sizeof(DICT_VALUE_TYPE) * n);
    size_t i = 0;
    DICT_KEY_TYPE key;
    DICT_VALUE_TYPE value;
    kh_foreach(dict, key, value, {
        values[i] = value;
        i++;
    });
    *values_size = i;
    return values;
}


#undef CONCAT
#undef CONCAT_
#undef CONCAT3
#undef CONCAT3_
#undef KH_TYPE
#undef KH_FUNC
#undef DICT_FUNC
#undef DICT_TYPE
#undef DICT_NAME_TYPE

#ifdef DICT_ORIG_MALLOC
#define kmalloc(Z) DICT_ORIG_MALLOC(Z)
#undef DICT_ORIG_MALLOC
#endif

#ifdef DICT_ORIG_CALLOC
#define kcalloc(N,Z) DICT_ORIG_CALLOC(N,Z)
#undef DICT_ORIG_CALLOC
#endif

#ifdef DICT_ORIG_REALLOC
#define krealloc(P,Z) DICT_ORIG_REALLOC(P,Z)
#undef DICT_ORIG_REALLOC
#endif

#ifdef DICT_ORIG_FREE
#define kfree(P) DICT_ORIG_FREE(P)
#undef DICT_ORIG_FREE
#endif

#undef IS_MAP