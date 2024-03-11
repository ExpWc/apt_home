/* Copyright 2008-2011 The MathWorks, Inc. */

#ifndef SL_CORE_BLOCK_SPEC_HPP
#define SL_CORE_BLOCK_SPEC_HPP

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifdef _MSC_VER
#define DECL_DLL_EXPORT_EXP_INST(ClassInst)
#define DECL_DLL_IMPORT_EXP_INST(ClassInst)
#elif __GNUC__ >= 4
#define DECL_DLL_EXPORT_EXP_INST(ClassInst)
#define DECL_DLL_IMPORT_EXP_INST(ClassInst)    extern template class ClassInst
#else
#define DLL_EXPORT_EXP_INST(ClassInst)
#define DLL_IMPORT_EXP_INST(ClassInst)
#endif
#define DLL_EXPORT_EXP_INST_TEMPLATE DLL_EXPORT_SYM
#define DLL_IMPORT_EXP_INST_TEMPLATE DLL_IMPORT_SYM

#ifdef BUILDING_SIMULINKBLOCK

# define SL_CORE_BLOCK_EXPORT_CLASS               DLL_EXPORT_SYM
# define SL_CORE_BLOCK_EXPORT_FCN                 DLL_EXPORT_SYM
# define SL_CORE_BLOCK_EXPORT                     DLL_EXPORT_SYM
# define SL_CORE_BLOCK_EXPORT_TEMPLATE            DLL_EXPORT_TEMPLATE
# define SL_CORE_BLOCK_EXPORT_EXP_INST_TEMPLATE   DLL_EXPORT_EXP_INST_TEMPLATE
# define SL_CORE_BLOCK_EXPORT_EXTERN_C            extern "C" DLL_EXPORT_SYM
# define SL_CORE_BLOCK_DECL_EXPORT_EXP_INST_TEMPLATE(ClassInst)  DECL_DLL_EXPORT_EXP_INST(ClassInst)

#elif defined(DLL_IMPORT_SYM)

# define SL_CORE_BLOCK_EXPORT_CLASS               DLL_IMPORT_SYM
# define SL_CORE_BLOCK_EXPORT_FCN                 DLL_IMPORT_SYM
# define SL_CORE_BLOCK_EXPORT                     DLL_IMPORT_SYM
# define SL_CORE_BLOCK_EXPORT_TEMPLATE            DLL_EXPORT_TEMPLATE
# define SL_CORE_BLOCK_EXPORT_EXP_INST_TEMPLATE   DLL_IMPORT_EXP_INST_TEMPLATE
# define SL_CORE_BLOCK_EXPORT_EXTERN_C            extern "C" DLL_IMPORT_SYM
# define SL_CORE_BLOCK_DECL_EXPORT_EXP_INST_TEMPLATE(ClassInst) DECL_DLL_IMPORT_EXP_INST(ClassInst)
#else

#ifdef __cplusplus
#define SL_CORE_BLOCK_EXPORT_EXTERN_C extern "C"
#else
#define SL_CORE_BLOCK_EXPORT_EXTERN_C extern
#endif

# define SL_CORE_BLOCK_EXPORT_CLASS  extern
# define SL_CORE_BLOCK_EXPORT_FCN    extern
# define SL_CORE_BLOCK_EXPORT        extern
# define SL_CORE_BLOCK_EXPORT_TEMPLATE extern 
#endif

#endif 

