#pragma once

/** @file lake/hadean.h
 *  @brief The XR runtime backend.
 *
 *  Tightly cooperates with other backends, especially Hadal.
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>

/** The platform-specific XR runtime backend. */
LAKE_DECL_INTERFACE(hadean);
/** Represents an active XR experience. Without a session the XR features can still be queried. */
LAKE_DECL_HANDLE_INTERFACED(hadean_session, hadean_interface *hadean);
/** Represents a frame of reference between virtual space and real-world locations. */
LAKE_DECL_HANDLE_INTERFACED(hadean_space, hadean_session *session);
/** Represents an individual action when retrieving action data, creating spaces, or sending haptic events. */
LAKE_DECL_HANDLE_INTERFACED(hadean_action, hadean_session *session);
/** Represents a collection of actions that can be logically coupled with the game context. */
LAKE_DECL_HANDLE_INTERFACED(hadean_action_set, hadean_session *session);
/** Represents a swapchain for interfacing with the display and rendering backends. */
LAKE_DECL_HANDLE_INTERFACED(hadean_swapchain, hadean_session *session);

/** Interface of the XR runtime backend. */
struct hadean_interface_impl {
    lake_interface_header header;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef HADEAN_OPENXR
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadean, openxr);
#endif /* HADEAN_OPENXR */
#ifdef HADEAN_WEBXR 
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadean, webxr);
#endif /* HADEAN_WEBXR */

/** Headless XR backend, used for validation. */
LAKEAPI FN_LAKE_INTERFACE_IMPL(hadean, headless);

#ifdef __cplusplus
}
#endif /* __cplusplus */
