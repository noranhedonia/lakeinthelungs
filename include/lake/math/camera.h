#pragma once

/** @file lake/math/camera.h
 *  @brief TODO docs
 */
#include <lake/bedrock/simd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_camera {
    vec3 position;
    /** A quaternion describing the camera's rotation. */
    quat rotation;
    /** The vertical field of view (top to bottom) in radians. */
    f32  fov;
    /** The distance of the near plane and the far plane to the camera position. */
    f32  near, far;
    /** The proportion between's the camera frustum's width and height. */
    f32  aspect_ratio;
} lake_camera;

#ifdef __cplusplus
}
#endif /* __cplusplus */
