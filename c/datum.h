#ifndef DATUM_H
#define DATUM_H

#include <stdint.h>

#include "lang/builder.h"
#include "lang/name.h"

#include "pp.h"

#define DATUM_GENERIC(TYPE, GENERATOR, ...) \
  DATUM_GENERIC_IMPL(TYPE, EAT, GENERATOR, __VA_ARGS__)

#define DATUM_GENERIC_IMPL(TYPE, TYPE_CASES, GENERATOR, ...) \
  _Generic((TYPE), \
    char:                 GENERATOR(chr, TYPE, __VA_ARGS__), \
    double:               GENERATOR(dbl, TYPE, __VA_ARGS__), \
    int:                  GENERATOR(s64, TYPE, __VA_ARGS__), \
    long:                 GENERATOR(s64, TYPE, __VA_ARGS__), \
    long long:            GENERATOR(s64, TYPE, __VA_ARGS__), \
    unsigned:             GENERATOR(u64, TYPE, __VA_ARGS__), \
    unsigned long:        GENERATOR(u64, TYPE, __VA_ARGS__), \
    unsigned long long:   GENERATOR(u64, TYPE, __VA_ARGS__), \
    char *:               GENERATOR(str, TYPE, __VA_ARGS__), \
    void *:               GENERATOR(ptr, TYPE, __VA_ARGS__), \
    void **:              GENERATOR(ref, TYPE, __VA_ARGS__), \
    \
    TYPE_CASES(TYPE, GENERATOR, __VA_ARGS__) \
    \
    default:              GENERATOR(ptr, TYPE, __VA_ARGS__) \
  )

// Datum

typedef union datum
{
  char chr;
  double dbl;
  int64_t s64;
  uint64_t u64;
  void *ptr;
  void **ref;
  char *str;
}
datum_t;

// Datum types

#define DATUM_TYPES(PREFIX) \
  NAME(PREFIX, NONE) = 0, \
  NAME(PREFIX, CHR) = 1, \
  NAME(PREFIX, DBL) = 2, \
  NAME(PREFIX, S64) = 4, \
  NAME(PREFIX, U64) = 8, \
  NAME(PREFIX, PTR) = 16, \
  NAME(PREFIX, REF) = 32, \
  NAME(PREFIX, STR) = 64 

typedef enum datum_types
{
  DATUM_TYPES(DATUM)
}
datum_types_t;

// Lifecycle

#define datum_clear(DATUM) \
  (DATUM).chr = 0; \
  (DATUM).dbl = 0; \
  (DATUM).s64 = 0; \
  (DATUM).u64 = 0; \
  (DATUM).ptr = NULL; \
  (DATUM).str = NULL

// Methods::Accessor

#define datum_accessor(DATUM, TYPE) \
  DATUM_GENERIC(TYPE, datum_accessor_generator, DATUM)

#define datum_accessor_generator(CLASSIFIER, TYPE, DATUM) \
  (DATUM).CLASSIFIER

// Methods::Default

#define datum_default(TYPE) \
  DATUM_GENERIC((TYPE) 0, datum_default_generator, NULL)

#define datum_default_generator(CLASSIFIER, TYPE, ...) \
  NAME(datum_default_for, CLASSIFIER)

#define datum_default_for_chr datum_default_for_numeric
#define datum_default_for_dbl datum_default_for_numeric
#define datum_default_for_s64 datum_default_for_numeric
#define datum_default_for_u64 datum_default_for_numeric
#define datum_default_for_ptr datum_default_for_pointer
#define datum_default_for_ref datum_default_for_pointer
#define datum_default_for_str ""

#define datum_default_for_numeric 0
#define datum_default_for_pointer NULL

// Methods::Error

#define datum_error(TYPE) \
  DATUM_GENERIC((TYPE) 0, datum_error_generator, NULL)

#define datum_error_generator(CLASSIFIER, TYPE, ...) \
  NAME(datum_error_for, CLASSIFIER)

#define datum_error_for_chr datum_error_for_numeric
#define datum_error_for_dbl datum_error_for_numeric
#define datum_error_for_s64 datum_error_for_numeric
#define datum_error_for_u64 datum_error_for_numeric
#define datum_error_for_ptr datum_error_for_pointer
#define datum_error_for_ref datum_error_for_pointer
#define datum_error_for_str ""

#define datum_error_for_numeric -1
#define datum_error_for_pointer NULL

// Methods::Access

#define datum_access(DATUM, TYPE) \
  datum_accessor(DATUM, (TYPE) 0)

// Methods::Get

#define datum_get(DATUM, TYPE) \
  datum_access(DATUM, TYPE)

#define datum_read(DATUM, TYPE) \
  ((TYPE)datum_access(DATUM, TYPE))

// Methods::Get Primitive

#define datum_read_primitive(DATUM, DATUM_TYPE) \
  ( \
    (DATUM_TYPE) == DATUM_CHR ? (DATUM).chr : \
    (DATUM_TYPE) == DATUM_S64 ? (DATUM).s64 : \
    (DATUM_TYPE) == DATUM_U64 ? (DATUM).u64 : \
    (DATUM_TYPE) == DATUM_DBL ? (DATUM).dbl : \
    0 \
  )

// Methods::Set

#define datum_set(DATUM, VALUE) \
  (datum_accessor(DATUM, VALUE) = VALUE)

// Methods::Type

#define datum_type_chr DATUM_CHR
#define datum_type_dbl DATUM_DBL
#define datum_type_s64 DATUM_S64
#define datum_type_u64 DATUM_U64
#define datum_type_ptr DATUM_PTR
#define datum_type_ref DATUM_REF
#define datum_type_str DATUM_STR

#define datum_type(TYPE) \
  DATUM_GENERIC(TYPE, datum_type_generator, NULL)

#define datum_type_generator(CLASSIFIER, TYPE, ...) \
  NAME(datum_type, CLASSIFIER)

#endif
