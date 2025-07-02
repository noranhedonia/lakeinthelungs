#pragma once

/** @file lake/modules/lovage.h
 *  @brief Immediate-mode UI system.
 *
 *  TODO docs
 */
#include <lake/bedrock/bedrock.h>
#include <lake/data_structures/darray.h>
#include <lake/modules/moon.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** For now let's use 16-bits. */
typedef u16 lovage_draw_index;

/** Data of a single vertex for rendering UI widgets. The same format 
 *  will be used inside shaders for the interface pass. */
typedef struct lovage_draw_vertex {
    vec2                        pos;    /**< Screen space position in pixels. */
    vec2                        uv;     /**< Normalized texture UV coordinates. */
    u32                         color;  /**< sRGB color and opacity of the vertex. */
} lovage_draw_vertex;

/** A single draw command, generally maps to a single GPU draw call. */
typedef struct lovage_draw_command {
    /** Clipping rectangle (x1, y1, x2, y2). Subtract viewport size to get
     *  the clipping rectangle in "viewport" coordinates. */
    vec4                        clip_rect;
    /** The texture ID of the font atlas or custom images. */
    moon_texture_id             texture_id;
    /** Start offset in vertex and index buffers. */
    u64                         vtx_offset, idx_offset;
    /** Number of indices to be rendered as triangles.
     *  Vertices and indices are stored in `lovage_draw_pass`. */
    u32                         triangle_count;
} lovage_draw_command;

/** Perform draw commands in an interface render pass, generally one per 
 *  video viewport or world-space texture target. */
typedef struct lovage_draw_pass {
    /** Recorded draw commands to be translated into GPU API calls. */
    lovage_draw_command        *cmd_list;
    /** All vertices for this interface pass. It's typically a pointer to mapped memory. */
    lovage_draw_vertex         *vtx;
    /** All indices for the triangle lists rendered. It's typically a pointer to mapped memory. */
    lovage_draw_index          *idx;
    /** The capacity of arrays mapped above. */
    u32                         cmd_list_size, vtx_size, idx_size;
    /** The number of recorded commands, vertices and indices in this draw pass. */
    u32                         cmd_list_count, vtx_count, idx_count;
    u32                         _vtx_current_idx;
    lovage_draw_vertex         *_vtx_write;
    lovage_draw_index          *_idx_write;
} lovage_draw_pass;

/** A font input/source. */
typedef struct lovage_font_source {
    /** TTF/OTF data. */
    void                       *font_data;
    /** TTF/OTF data size. */
    u32                         font_data_size;
    /** TTF/OTF data ownership taken by lovage_font_atlas. */
    bool                        font_data_owned_by_atlas;
    /** Merge into previous lovage_font, so multiple fonts are combined into one. */
    bool                        merge_mode;
    /** Align every glyph advance_x to pixel boundaries. */
    bool                        pixel_snap_h;
    /** Index of the font within TTF/OTF file. */
    u32                         font_index;
    /** Rasterize at higher quality for sub-pixel positioning. 0 == auto == 1 or 2. */
    u32                         oversample_h;
    /** Rasterize at higher quality for sub-pixel positioning. 0 == auto == 1. */
    u32                         oversample_v;
    /** Size in pixels for rasterizer. */
    f32                         size_pixels;
    /** Offset all glyphs from this font input. */
    vec2                        glyph_offset;
    /** Pointer to a list of Unicode ranges (2 value per range, zero-terminated list). 
     *  This data must persist for the whole lifetime of this font source. */
    lake_unicode const         *glyph_ranges;
    /** Minimum advance_x for glyphs, set the minimum to align font icons. */
    f32                         glyph_min_advance_x;
    /** Maximum advance_x for glyphs, set both min and max to enforce mono-space font. */
    f32                         glyph_max_advance_x;
    /** Font name (for debugging). */
    char const                 *font_name;
    /** The font source will be written here. */
    struct lovage_font         *destination;
} lovage_font_source;

/** A single font glyph (code point + coordinates within a lovage_font_atlas + offset). */
typedef struct lovage_font_glyph {
    /** Flag to indicate glyph is colored and should generally ignore tinting
     *  (makes it usable with no shift on little-endian systems). */
    u32                         colored : 1;
    /** Flag to indicate glyph has no visible pixels (e.g. space), 
     *  allows early out when rendering. */
    u32                         visible : 1;
    /** Values in range of 0x0000..0x10ffff. */
    u32                         codepoint : 30;
    /** Horizontal distance to advance layout with. */
    f32                         advance_x;
    /** Corners of the glyph rectangle in x0, y0, x1, y1 layout. */
    vec4                        rect;
    /** Texture coordinates in u0, v0, u1, v1 layout. */
    vec4                        tex_coords;
} lovage_font_glyph;

/** Font runtime data and rendering. */
typedef struct lovage_font {
    /** darray<lovage_font_glyph>, an array of glyphs in the loaded font. */
    lake_darray                 glyphs;
    /** The glyph obtained from lovage_font::fallback_char lookup. */
    lovage_font_glyph          *fallback_glyph;
    /** Horizontal advance of the fallback_glyph. */
    f32                         fallback_advance_x;
    /** Height of characters/line, set during loading and immutable later. */
    f32                         font_size;
    /** The atlas this font is loaded into. */
    struct lovage_font_atlas   *atlas;
    /** Pointer within atlas::sources[], to sources_count instances. */
    lovage_font_source         *sources;
    /** Number of sources involved in creating this font (1 or more). */
    u16                         sources_count;
    /** Ellipsis to render. */
    u16                         ellipsis_count;
    /** Character used for ellipsis rendering ('...'). */
    lake_unicode                ellipsis_char;
    /** Character used if a glyph is not found (U+fffd, '?'). */
    lake_unicode                fallback_char;
    /** Total ellipsis width. */
    f32                         ellipsis_width;
    /** Step between characters when ellipsis_count > 0. */
    f32                         ellipsis_char_step;
    /** Base font scale (1.0f) multiplied by the per-window font scale. */
    f32                         scale;
    /** Ascent: distance from top to bottom of e.g. 'A' [0..font_size] (unscaled). */
    f32                         ascent, descent;
} lovage_font;

/** Bakes multiple fonts into a single texture as runtime data (also TTF/OTF loader). */
typedef struct lovage_font_atlas {
    /** Reference for the texture once it is uploaded. Passed into text draw commands. */
    moon_texture_id             texture_id;
    /** 4 components per pixel, each is u8. Total size: (texture_width * texture_height * 4). */ 
    u32                        *texture_pixels_rgba8;
    /** Dimensions of the font atlas texture. */
    u32                         texture_width, texture_height;
    /** An extent obtained from = (1.0f/texture_width, 1.0f/texture_height). */
    vec2                        texture_uv_scale;
    /** Texture coordinates to a white pixel. */
    vec2                        texture_uv_white_pixel;
    /** darray<lovage_font>, holds all fonts added to the atlas, fonts[0] is the default font. */
    lake_darray                 fonts;
    /** darray<lovage_font_source>, holds configurations for the fonts. */
    lake_darray                 sources;
} lovage_font_atlas;

#ifdef __cplusplus
}
#endif /* __cplusplus */
