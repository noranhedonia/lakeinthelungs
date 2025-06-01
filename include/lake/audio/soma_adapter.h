#pragma once

/** @file lake/audio/soma_adapter.h
 *  @brief Audio backend.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>

/** The audio interface implementation. */
LAKE_DECL_INTERFACE(soma);
/** Platform-specific audio backend. */
LAKE_DECL_HANDLE(soma_adapter);
