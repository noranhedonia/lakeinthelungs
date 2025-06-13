module lake;

#if !defined(LAKE_GPU_TABLE_SET_BINDING)
#define LAKE_GPU_TABLE_SET_BINDING 0
#define LAKE_STORAGE_BUFFER_BINDING 0
#define LAKE_STORAGE_TEXTURE_BINDING 1
#define LAKE_SAMPLED_TEXTURE_BINDING 2
#define LAKE_SAMPLER_BINDING 3
#define LAKE_DEVICE_ADDRESS_BUFFER_BINDING 4
#define LAKE_ACCELERATION_STRUCTURE_BINDING 5
#endif

/* TODO d3d12, metal, webgpu bindings ? */
#define LAKE_DECL_PUSH_CONSTANT(T)          [[vk::push_constant]] struct T
#define LAKE_DECL_STORAGE_BUFFERS           [[vk::binding(LAKE_STORAGE_BUFFER_BINDING, 0)]]
#define LAKE_DECL_STORAGE_TEXTURES          [[vk::binding(LAKE_STORAGE_TEXTURE_BINDING, 0)]]
#define LAKE_DECL_SAMPLED_TEXTURES          [[vk::binding(LAKE_SAMPLED_TEXTURE_BINDING, 0)]]
#define LAKE_DECL_SAMPLERS                  [[vk::binding(LAKE_SAMPLER_BINDING, 0)]]
#define LAKE_DECL_ACCELERATION_STRUCTURES   [[vk::binding(LAKE_ACCELERATION_STRUCTURE_BINDING, 0)]]
#define LAKE_DECL_BUFFER_ADDRESSES          [[vk::binding(LAKE_DEVICE_ADDRESS_BUFFER_BINDING, 0)]]

#define LAKE_ID_INDEX_BITS 20
#define LAKE_ID_INDEX_MASK ((uint64_t(1) << LAKE_ID_INDEX_BITS) - uint64_t(1))
#define LAKE_ID_INDEX_OFFSET 0
#define LAKE_ID_VERSION_BITS 44
#define LAKE_ID_VERSION_MASK ((uint64_t(1) << LAKE_ID_VERSION_BITS) - uint64_t(1))
#define LAKE_ID_VERSION_OFFSET LAKE_ID_INDEX_BITS

#define _LAKE_DECL_VEC_TYPES(SLANG_TYPE, LAKE_TYPE) \
    typedef SLANG_TYPE LAKE_TYPE;                   \
    typedef SLANG_TYPE ## 1 LAKE_TYPE ## vec1;      \
    typedef SLANG_TYPE ## 2 LAKE_TYPE ## vec2;      \
    typedef SLANG_TYPE ## 3 LAKE_TYPE ## vec3;      \
    typedef SLANG_TYPE ## 4 LAKE_TYPE ## vec4;

#define _LAKE_DECL_MAT_TYPES(SLANG_TYPE, LAKE_TYPE)      \
    typedef matrix<SLANG_TYPE, 1, 1> LAKE_TYPE ## mat1x1 \
    typedef matrix<SLANG_TYPE, 2, 1> LAKE_TYPE ## mat1x2 \
    typedef matrix<SLANG_TYPE, 3, 1> LAKE_TYPE ## mat1x3 \
    typedef matrix<SLANG_TYPE, 4, 1> LAKE_TYPE ## mat1x4 \
    typedef matrix<SLANG_TYPE, 1, 2> LAKE_TYPE ## mat2x1 \
    typedef matrix<SLANG_TYPE, 2, 2> LAKE_TYPE ## mat2x2 \
    typedef matrix<SLANG_TYPE, 3, 2> LAKE_TYPE ## mat2x3 \
    typedef matrix<SLANG_TYPE, 4, 2> LAKE_TYPE ## mat2x4 \
    typedef matrix<SLANG_TYPE, 1, 3> LAKE_TYPE ## mat3x1 \
    typedef matrix<SLANG_TYPE, 2, 3> LAKE_TYPE ## mat3x2 \
    typedef matrix<SLANG_TYPE, 3, 3> LAKE_TYPE ## mat3x3 \
    typedef matrix<SLANG_TYPE, 4, 3> LAKE_TYPE ## mat3x4 \
    typedef matrix<SLANG_TYPE, 1, 4> LAKE_TYPE ## mat4x1 \
    typedef matrix<SLANG_TYPE, 2, 4> LAKE_TYPE ## mat4x2 \
    typedef matrix<SLANG_TYPE, 3, 4> LAKE_TYPE ## mat4x3 \
    typedef matrix<SLANG_TYPE, 4, 4> LAKE_TYPE ## mat4x4

#define _LAKE_DECL_VEC_MAT_8_16_32_64_TYPES(SLANG_TYPE, LAKE_TYPE) \
    _LAKE_DECL_VEC_TYPES(SLANG_TYPE ## 8_t,  LAKE_TYPE ## 8)       \
    _LAKE_DECL_VEC_TYPES(SLANG_TYPE ## 16_t, LAKE_TYPE ## 16)      \
    _LAKE_DECL_VEC_TYPES(SLANG_TYPE ## 32_t, LAKE_TYPE ## 32)      \
    _LAKE_DECL_VEC_TYPES(SLANG_TYPE ## 64_t, LAKE_TYPE ## 64)      \
    _LAKE_DECL_MAT_TYPES(SLANG_TYPE ## 8_t,  LAKE_TYPE ## 8)       \
    _LAKE_DECL_MAT_TYPES(SLANG_TYPE ## 16_t, LAKE_TYPE ## 16)      \
    _LAKE_DECL_MAT_TYPES(SLANG_TYPE ## 32_t, LAKE_TYPE ## 32)      \
    _LAKE_DECL_MAT_TYPES(SLANG_TYPE ## 64_t, LAKE_TYPE ## 64)

namespace lake {
    _LAKE_DECL_VEC_MAT_8_16_32_64_TYPES(uint, u)
    _LAKE_DECL_VEC_MAT_8_16_32_64_TYPES(int, s)
    _LAKE_DECL_VEC_TYPES(half, f16)
    _LAKE_DECL_VEC_TYPES(float, f32)
    _LAKE_DECL_VEC_TYPES(double, f64)
    _LAKE_DECL_MAT_TYPES(half, f16)
    _LAKE_DECL_MAT_TYPES(float, f32)
    _LAKE_DECL_MAT_TYPES(double, f64)
    _LAKE_DECL_VEC_TYPES(bool, b32)
    _LAKE_DECL_MAT_TYPES(bool, b32)

    LAKE_DECL_STORAGE_BUFFERS           __DynamicResource               buffers[];
    LAKE_DECL_STORAGE_BUFFERS           coherent __DynamicResource      buffers_coherent[];
    LAKE_DECL_STORAGE_TEXTURES          __DynamicResource               storage_textures[];
    LAKE_DECL_STORAGE_TEXTURES          coherent __DynamicResource      storage_textures_coherent[];
    LAKE_DECL_SAMPLED_TEXTURES          __DynamicResource               sampled_textures[];
    LAKE_DECL_SAMPLED_TEXTURES          coherent __DynamicResource      sampled_textures_coherent[];
    LAKE_DECL_SAMPLERS                  SamplerState                    samplers[];
#if defined(LAKE_RAY_TRACING)
    LAKE_DECL_ACCELERATION_STRUCTURES   RaytracingAccelerationStructure acceleration_structures[];
#endif /* LAKE_RAY_TRACING */
    LAKE_DECL_BUFFER_ADDRESSES          StructuredBuffer<lake::u64>     buffer_addresses;

    struct buffer_id {
        lake::u64 value;

        lake::u32 index() { return lake::u32(LAKE_ID_INDEX_MASK & value); }
        lake::u64 version() { return (value >> LAKE_ID_VERSION_OFFSET); }
        lake::b32 is_empty() { return value == 0; }
        lake::u64 device_address() { return lake::buffer_addresses[index()]; }
    };

    struct texture_view_id {
        lake::u64 value;

        lake::u32 index() { return lake::u32(LAKE_ID_INDEX_MASK & value); }
        lake::u64 version() { return (value >> LAKE_ID_VERSION_OFFSET); }
        lake::b32 is_empty() { return value == 0; }
    };

    struct texture_view_idx {
        lake::u32 value;

        lake::u32 index() { return lake::u32(LAKE_ID_INDEX_MASK & value); }
        lake::b32 is_empty() { return value == 0; }
    };

    struct sampler_id {
        lake::u64 value;

        lake::u32 index() { return lake::u32(LAKE_ID_INDEX_MASK & value); }
        lake::u64 version() { return (value >> LAKE_ID_VERSION_OFFSET); }
        lake::b32 is_empty() { return value == 0; }
        SamplerState get() { return lake::samplers[index()]; }
    };

#if defined(LAKE_RAY_TRACING)
    LAKE_DECL_ACCELERATION_STRUCTURES RaytracingAccelerationStructure ray_tracing_acceleration_structure_table[];
    extension RaytracingAccelerationStructure {
        static RaytracingAccelerationStructure get(lake::tlas_id i) { 
            return lake::ray_tracing_acceleration_structure_table[i.index()]; 
        }
    };

    struct tlas_id {
        lake::u64 value;

        lake::u32 index() { return lake::u32(LAKE_ID_INDEX_MASK & value); }
        lake::u64 version() { return (value >> LAKE_ID_VERSION_OFFSET); }
        lake::b32 is_empty() { return value == 0; }
        RaytracingAccelerationStructure get() { 
            return lake::ray_tracing_acceleration_structure_table[index()]; 
        }
    };
#endif // LAKE_RAY_TRACING

    extension ByteAddressBuffer {
        static ByteAddressBuffer get(lake::buffer_id i) { return lake::buffers[i.index()]; }
        static ByteAddressBuffer get_coherent(lake::buffer_id i) { return lake::buffers_coherent[i.index()]; }
    };

    extension RWByteAddressBuffer {
        static RWByteAddressBuffer get(lake::buffer_id i) { return lake::buffers[i.index()]; }
        static RWByteAddressBuffer get_coherent(lake::buffer_id i) { return lake::buffers_coherent[i.index()]; }
    };

    extension SamplerState {
        static SamplerState get(lake::sampler_id i) { return lake::samplers[i.index()]; }
    };
} /* namespace lake */

#define _LAKE_DECL_TEX_EXTENSION(TYPE, TABLE)                           \
__generic<T : ITexelElement> extension TYPE<T> {                        \
    static TYPE<T> get(lake::texture_view_id i) {                       \
        return TABLE[i.index()];                                        \
    }                                                                   \
    static TYPE<T> get_coherent(lake::texture_view_id i) {              \
        return TABLE ## _coherent[i.index()];                           \
    }                                                                   \
    static TYPE<T> get(lake::texture_view_idx i) {                      \
        return TABLE[i.index()];                                        \
    }                                                                   \
    static TYPE<T> get_coherent(lake::texture_view_idx i) {             \
        return TABLE ## _coherent[i.index()];                           \
    }                                                                   \
}                                                                       \
namespace lake {                                                        \
    __generic<T : ITexelElement>                                        \
    struct TYPE ## _id                                                  \
    {                                                                   \
        texture_view_id handle;                                         \
        TYPE<T> get() { return TYPE<T>::get(handle); }                  \
        TYPE<T> get_coherent() { return TYPE<T>::get_coherent(handle); }\
    };                                                                  \
    __generic<T : ITexelElement>                                        \
    struct TYPE ## _idx                                                 \
    {                                                                   \
        texture_view_idx index;                                         \
        TYPE<T> get() { return TYPE<T>::get(index); }                   \
        TYPE<T> get_coherent() { return TYPE<T>::get_coherent(index); } \
    };                                                                  \
} /* namespace lake */

_LAKE_DECL_TEX_EXTENSION(RWTexture1D, lake::storage_textures)
_LAKE_DECL_TEX_EXTENSION(RWTexture2D, lake::storage_textures)
_LAKE_DECL_TEX_EXTENSION(RWTexture3D, lake::storage_textures)
_LAKE_DECL_TEX_EXTENSION(RWTexture1DArray, lake::storage_textures)
_LAKE_DECL_TEX_EXTENSION(RWTexture2DArray, lake::storage_textures)

_LAKE_DECL_TEX_EXTENSION(Texture1D, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(Texture2D, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(Texture3D, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(Texture1DArray, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(Texture2DArray, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(TextureCube, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(TextureCubeArray, lake::sampled_textures)
_LAKE_DECL_TEX_EXTENSION(Texture2DMS, lake::sampled_textures)
