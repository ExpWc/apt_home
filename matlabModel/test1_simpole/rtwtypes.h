/*
 * rtwtypes.h
 *
 * Code generation for model "test1".
 *
 * Model version              : 1.12
 * Simulink Coder version : 8.13 (R2017b) 24-Jul-2017
 * C source code generated on : Mon Mar  4 11:01:49 2024
 *
 * Target selection: AutoTest_grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Generic->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTWTYPES_H
#define RTWTYPES_H
#include "tmwtypes.h"
#include "simstruc_types.h"
#ifndef POINTER_T
# define POINTER_T

typedef void * pointer_T;

#endif

/* Logical type definitions */
#if (!defined(__cplusplus))
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

#ifndef INT64_T
#define INT64_T

typedef long int64_T;

#endif

#ifndef UINT64_T
#define UINT64_T

typedef unsigned long uint64_T;

#endif

/*===========================================================================*
 * Additional complex number type definitions                                           *
 *===========================================================================*/
#ifndef CINT64_T
#define CINT64_T

typedef struct {
  int64_T re;
  int64_T im;
} cint64_T;

#endif

#ifndef CUINT64_T
#define CUINT64_T

typedef struct {
  uint64_T re;
  uint64_T im;
} cuint64_T;

#endif
#endif                                 /* RTWTYPES_H */