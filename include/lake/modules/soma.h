#pragma once

/** @file lake/soma.h
 *  @brief Audio backend interface.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The audio interface implementation. */
LAKE_DECL_INTERFACE(soma);

struct soma_interface_impl {
    lake_interface_header   header;
};

#ifdef SOMA_ASIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, asio);
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, wasapi);
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, xaudio2);
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, coreaudio);
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, aaudio);
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, webaudio);
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, pipewire);
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, pulseaudio);
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, jack);
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, alsa);
#endif /* SOMA_ALSA */

/** Dummy audio backend for no sound capabilities, used for validation. */
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, dummy);

#ifdef __cplusplus
}
#endif /* __cplusplus */
