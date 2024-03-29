/* Copyright 2014 The MathWorks, Inc. */

#ifndef RT_RUNTIME_ERRORS_H
#define RT_RUNTIME_ERRORS_H

#include "tmwtypes.h"
#include "sf_runtime/sf_runtime_spec.h"
#include "sf_runtime/utils.h"

struct SfDebugInstanceStruct;

typedef enum ThrowFlag
{
    ThrowFlag_MESSAGE = -4,
    ThrowFlag_WARNING = -2,        
    ThrowFlag_NONE = -1,
    ThrowFlag_SAFE_ERROR = 0,      
    ThrowFlag_ERROR = 1,           
}ThrowFlag;

LIBMWSF_RUNTIME_API void sf_read_before_write_error(SimStruct* S,
                                                    unsigned int dataNumber,
                                                    unsigned int ssIdOfSourceObject,
                                                    int offsetInSourceObject,
                                                    int lengthInSourceObject);

LIBMWSF_RUNTIME_API double sf_data_range_check_min_max(SimStruct* S,
                                                       unsigned int ssIdOfSourceObject,
                                                       int offsetInSourceObject,
                                                       int lengthInSourceObject,
                                                       unsigned int dataNumber,
                                                       double dataValue,
                                                       double dataMin,
                                                       double dataMax);

LIBMWSF_RUNTIME_API double sf_data_range_check_max(SimStruct* S,
                                                   unsigned int ssIdOfSourceObject,
                                                   int offsetInSourceObject,
                                                   int lengthInSourceObject,
                                                   unsigned int dataNumber,
                                                   double dataValue,
                                                   double dataMax);

LIBMWSF_RUNTIME_API double sf_data_range_check_min(SimStruct* S,
                                                   unsigned int ssIdOfSourceObject,
                                                   int offsetInSourceObject,
                                                   int lengthInSourceObject,
                                                   unsigned int dataNumber,
                                                   double dataValue,
                                                   double dataMin);

LIBMWSF_RUNTIME_API void sf_state_inconsistency_error(SimStruct* S,
                                                      unsigned int ssIdOfSourceObject);

LIBMWSF_RUNTIME_API void sf_cycle_depth_limit_error(SimStruct* S,
                                                    unsigned int ssIdOfSourceObject);

LIBMWSF_RUNTIME_API int sf_array_bounds_check(struct SfDebugInstanceStruct* debugInstance,
                                              SimStruct* S,
                                              unsigned int ssIdOfSourceObject,
                                              int offsetInSourceObject,
                                              int lengthInSourceObject,
                                              int dataNumber,
                                              int indexValue,
                                              int firstIndex,
                                              int lastIndex);

LIBMWSF_RUNTIME_API int sf_eml_array_bounds_check(struct SfDebugInstanceStruct* debugInstance,
                                                  SimStruct* S,
                                                  unsigned int ssIdOfSourceObject,
                                                  int offsetInSourceObject,
                                                  int lengthInSourceObject,
                                                  int dataNumber,
                                                  int indexValue,
                                                  int firstIndex,
                                                  int lastIndex);

#ifdef INT_TYPE_64_IS_SUPPORTED
LIBMWSF_RUNTIME_API int64_T sf_eml_array_bounds_check_int64(struct SfDebugInstanceStruct* debugInstance,
                                                            SimStruct* S,
                                                            unsigned int ssIdOfSourceObject,
                                                            int offsetInSourceObject,
                                                            int lengthInSourceObject,
                                                            int dataNumber,
                                                            int64_T indexValue,
                                                            int64_T firstIndex,
                                                            int64_T lastIndex);
#endif

LIBMWSF_RUNTIME_API int sf_eml_shift_index_bounds_check(struct SfDebugInstanceStruct* debugInstance,
                                                        SimStruct* S,
                                                        unsigned int ssIdOfSourceObject,
                                                        int offsetInSourceObject,
                                                        int lengthInSourceObject,
                                                        const char* operandText,
                                                        int indexValue,
                                                        int firstIndex,
                                                        int lastIndex);

#ifdef INT_TYPE_64_IS_SUPPORTED
LIBMWSF_RUNTIME_API int64_T sf_eml_shift_index_bounds_check_int64(struct SfDebugInstanceStruct* debugInstance,
                                                                  SimStruct* S,
                                                                  unsigned int ssIdOfSourceObject,
                                                                  int offsetInSourceObject,
                                                                  int lengthInSourceObject,
                                                                  const char* operandText,
                                                                  int64_T indexValue,
                                                                  int64_T firstIndex,
                                                                  int64_T lastIndex);
#endif

LIBMWSF_RUNTIME_API int sf_type_range_check(struct SfDebugInstanceStruct* debugInstance,
                                            SimStruct* S,
                                            unsigned int ssIdOfSourceObject,
                                            int offsetInSourceObject,
                                            int lengthInSourceObject,
                                            int indexValue,
                                            int firstIndex,
                                            int lastIndex);

LIBMWSF_RUNTIME_API int sf_eml_type_range_check(struct SfDebugInstanceStruct* debugInstance,
                                                SimStruct* S,
                                                unsigned int ssIdOfSourceObject,
                                                int offsetInSourceObject,
                                                int lengthInSourceObject,
                                                int indexValue,
                                                int firstValue,
                                                int lastIndex);

#ifdef INT_TYPE_64_IS_SUPPORTED
LIBMWSF_RUNTIME_API int64_T sf_eml_type_range_check_int64(struct SfDebugInstanceStruct* debugInstance,
                                                          SimStruct* S,
                                                          unsigned int ssIdOfSourceObject,
                                                          int offsetInSourceObject,
                                                          int lengthInSourceObject,
                                                          int64_T indexValue,
                                                          int64_T firstIndex,
                                                          int64_T lastIndex);

#endif

LIBMWSF_RUNTIME_API void sf_array_size_mismatch_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject,
                                                      unsigned int dimension, const char* arrayName1, int size1, const char* arrayName2, int size2);
LIBMWSF_RUNTIME_API void sf_data_overflow_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_data_divide_by_zero_error(SimStruct* S);
LIBMWSF_RUNTIME_API void sf_data_range_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject, unsigned int dataNumber, double dataValue, double* dataMin, double* dataMax);
LIBMWSF_RUNTIME_API double sf_integer_check(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject, double value);
LIBMWSF_RUNTIME_API double sf_non_negative_check(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject, double value, char* name);
LIBMWSF_RUNTIME_API void sf_unsafe_enum_cast_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);

LIBMWSF_RUNTIME_API void sf_attempt_to_access_forwarded_message_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_attempt_to_forward_unpopped_message_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_attempt_to_update_payload_before_pop_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_attempt_to_update_payload_for_forwarded_message_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_attempt_to_read_payload_before_pop_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_attempt_to_read_payload_for_forwarded_message_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);
LIBMWSF_RUNTIME_API void sf_msg_queue_overflow_error(SimStruct* S, unsigned int ssIdOfSourceObject, int offsetInSourceObject, int lengthInSourceObject);

LIBMWSF_RUNTIME_API ThrowFlag getSimulinkParameterMode(SimStruct* S, const char *modelName, const char *parameter);

#endif
