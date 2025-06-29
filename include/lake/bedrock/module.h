#pragma once

/** @file lake/bedrock/module.h
 *  @brief Dynamically access shared libraries, hot-reload plugins.
 *
 *  TODO docs
 */
#include <lake/bedrock/file_system.h>
#include <lake/bedrock/network.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Open a shared library. */
LAKE_NONNULL_ALL 
LAKEAPI void *LAKECALL 
lake_open_library(char const *libname);

/** Close a shared library. */
LAKE_NONNULL_ALL 
LAKEAPI void LAKECALL 
lake_close_library(void *library);

/** Get a procedure address from a shared library. */
LAKE_NONNULL_ALL LAKE_HOT_FN
LAKEAPI void *LAKECALL
lake_get_proc_address(void *library, char const *procname);

#ifdef __cplusplus
}
#endif /* __cplusplus */
