#pragma once

/** @file lake/audio/soma.h
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
LAKEAPI FN_LAKE_INTERFACE_IMPL(soma, pipewire, lake_framework);
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
