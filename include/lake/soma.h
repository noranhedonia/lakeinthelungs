#pragma once

/** @file lake/soma.h
 *  @brief Audio interface.
 *
 *  TODO docs
 */
#include <lake/bedrock.h>

/** The audio interface implementation. */
LAKE_DECL_INTERFACE(soma);

struct soma_interface_impl {
    lake_interface_header   header;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef SOMA_ASIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, asio, lake_framework);
#endif /* SOMA_ASIO */
#ifdef SOMA_WASAPI
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, wasapi, lake_framework);
#endif /* SOMA_WASAPI */
#ifdef SOMA_XAUDIO2
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, xaudio2, lake_framework);
#endif /* SOMA_XAUDIO2 */
#ifdef SOMA_COREAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, coreaudio, lake_framework);
#endif /* SOMA_COREAUDIO */
#ifdef SOMA_AAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, aaudio, lake_framework);
#endif /* SOMA_AAUDIO */
#ifdef SOMA_WEBAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, webaudio, lake_framework);
#endif /* SOMA_WEBAUDIO */
#ifdef SOMA_PIPEWIRE
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, pipewire, lake_framework);
#endif /* SOMA_PIPEWIRE */
#ifdef SOMA_PULSEAUDIO
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, pulseaudio, lake_framework);
#endif /* SOMA_PULSEAUDIO */
#ifdef SOMA_JACK
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, jack, lake_framework);
#endif /* SOMA_JACK */
#ifdef SOMA_ALSA
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, alsa, lake_framework);
#endif /* SOMA_ALSA */

LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, dummy, lake_framework);

#ifdef __cplusplus
}
#endif /* __cplusplus */
