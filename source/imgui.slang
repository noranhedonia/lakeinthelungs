import lake;

LAKE_DECL_PUSH_CONSTANT(imgui_push) {
    lake::vec2              scale;
    lake::vec2              translate;
    lake::u32               vbuf_offset;
    lake::u32               ibuf_offset;
    lake::u64               vbuf_ptr;
    lake::u64               ibuf_ptr;
    lake::texture_view_id   texture_id;
    lake::sampler_id        sampler_id;
};

struct imgui_vtx {
    lake::f32vec2 pos;
    lake::f32vec2 uv;
    lake::u32 color;
};

lake::f32vec4 srgb_to_linear(lake::f32vec4 srgb) {
    lake::f32vec3 color_srgb = srgb.rgb;
    /* 0 if under value, 1 if over */
    lake::f32vec3 selector = clamp(ceil(color_srgb - 0.04045), 0.0, 1.0);
    lake::f32vec3 under = color_srgb / 12.92;
    lake::f32vec3 over = pow((color_srgb + 0.055) / 1.055, lake::f32vec3(2.4, 2.4, 2.4));
    lake::f32vec3 result = mix(under, over, selector);
    return lake::f32vec4(result, srgb.a);
}

lake::f32vec4 linear_to_srgb(lake::f32vec4 linear) {
    lake::f32vec3 color_linear = linear.rgb;
    /* 0 if under value, 1 if over */
    lake::f32vec3 selector = clamp(ceil(color_linear - 0.0031308), 0.0, 1.0);
    lake::f32vec3 under = 12.92 * color_linear;
    lake::f32vec3 over = (1.055) * pow(color_linear, lake::f32vec3(1.0 / 2.4)) - 0.055;
    lake::f32vec3 result = mix(under, over, selector);
    return lake::f32vec4(result, linear.a);
}

[shader("imgui")]
void vtx_main() {
     
}

[shader("imgui")]
void frag_main() {

}
