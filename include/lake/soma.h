#pragma once

/** @file lake/soma.h
 *  @brief Audio backend.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>

/** Platform-specific audio backend. */
LAKE_DECL_HANDLE(soma_adapter);

/** Interface of the audio backend. */
typedef struct soma_interface_impl {
    lake_interface_header                   header;
} soma_interface_impl;

/** A view into the audio interface implementation. */
typedef union soma_interface {
    lake_interface_header                  *header;
    struct soma_adapter_impl               *adapter;
    struct soma_interface_impl             *interface;
    void                                   *data;
} soma_interface;

/** Details needed to assemble an audio backend. */
typedef struct soma_interface_assembly {
    soma_interface                         *out_impl; 
} soma_interface_assembly;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef SOMA_ASIO
/* TODO */
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
/* TODO */
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
/* TODO */
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
/* TODO */
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
/* TODO */
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
/* TODO */
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
LAKEAPI FN_LAKE_WORK(soma_interface_assembly_pipewire, soma_interface_assembly const *assembly);
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
/* TODO */
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
/* TODO */
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
/* TODO */
#endif /* SOMA_ALSA */

#ifdef __cplusplus
}
#endif /* __cplusplus */
