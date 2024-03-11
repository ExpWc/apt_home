/*
 * Copyright 2007-2015 The MathWorks, Inc.
 *
 * File: raccel_sfcn_utils.h
 *
 *
 * Abstract:
 *      functions for dynamically loading s-function mex files
 *
 * Requires include files
 *	tmwtypes.h
 *	simstruc_type.h
 * Note including simstruc.h before rsim.h is sufficient because simstruc.h
 * includes both tmwtypes.h and simstruc_types.h.
 */

#ifndef __RACCEL_SFCN_UTILS_H__
#define __RACCEL_SFCN_UTILS_H__

void raccelUnloadSFcnLibrariesAndFreeAssociatedGbls();

void raccelSetSFcnParamPtrToGblAndCopyParamData(SimStruct* child, 
                                             size_t childIdx, 
                                             const double** rtpPtr);

void raccelLoadSFcnMexFile(SimStruct* parent, 
                           SimStruct* child, 
                           size_t childIdx);

#endif /* __RACCEL_SFCN_UTILS_H__ */
