#include <lake/devel/imgui.h>

LAKE_CONST_FN
static ImGuiKey LAKECALL keycode_to_imgui_key(hadal_keycode keycode)
{
    switch (keycode) {
        case hadal_keycode_tab: return ImGuiKey_Tab;
        case hadal_keycode_left: return ImGuiKey_LeftArrow;
        case hadal_keycode_right: return ImGuiKey_RightArrow;
        case hadal_keycode_up: return ImGuiKey_UpArrow;
        case hadal_keycode_down: return ImGuiKey_DownArrow;
        case hadal_keycode_page_up: return ImGuiKey_PageUp;
        case hadal_keycode_page_down: return ImGuiKey_PageDown;
        case hadal_keycode_home: return ImGuiKey_Home;
        case hadal_keycode_end: return ImGuiKey_End;
        case hadal_keycode_insert: return ImGuiKey_Insert;
        case hadal_keycode_delete: return ImGuiKey_Delete;
        case hadal_keycode_backspace: return ImGuiKey_Backspace;
        case hadal_keycode_space: return ImGuiKey_Space;
        case hadal_keycode_enter: return ImGuiKey_Enter;
        case hadal_keycode_escape: return ImGuiKey_Escape;
        case hadal_keycode_left_control: return ImGuiKey_LeftCtrl;
        case hadal_keycode_left_shift: return ImGuiKey_LeftShift;
        case hadal_keycode_left_alt: return ImGuiKey_LeftAlt;
        case hadal_keycode_left_super: return ImGuiKey_LeftSuper;
        case hadal_keycode_right_control: return ImGuiKey_RightCtrl;
        case hadal_keycode_right_shift: return ImGuiKey_RightShift;
        case hadal_keycode_right_alt: return ImGuiKey_RightAlt;
        case hadal_keycode_right_super: return ImGuiKey_RightSuper;
        case hadal_keycode_menu: return ImGuiKey_Menu;
        case hadal_keycode_0: return ImGuiKey_0;
        case hadal_keycode_1: return ImGuiKey_1;
        case hadal_keycode_2: return ImGuiKey_2;
        case hadal_keycode_3: return ImGuiKey_3;
        case hadal_keycode_4: return ImGuiKey_4;
        case hadal_keycode_5: return ImGuiKey_5;
        case hadal_keycode_6: return ImGuiKey_6;
        case hadal_keycode_7: return ImGuiKey_7;
        case hadal_keycode_8: return ImGuiKey_8;
        case hadal_keycode_9: return ImGuiKey_9;
        case hadal_keycode_a: return ImGuiKey_A;
        case hadal_keycode_b: return ImGuiKey_B;
        case hadal_keycode_c: return ImGuiKey_C;
        case hadal_keycode_d: return ImGuiKey_D;
        case hadal_keycode_e: return ImGuiKey_E;
        case hadal_keycode_f: return ImGuiKey_F;
        case hadal_keycode_g: return ImGuiKey_G;
        case hadal_keycode_h: return ImGuiKey_H;
        case hadal_keycode_i: return ImGuiKey_I;
        case hadal_keycode_j: return ImGuiKey_J;
        case hadal_keycode_k: return ImGuiKey_K;
        case hadal_keycode_l: return ImGuiKey_L;
        case hadal_keycode_m: return ImGuiKey_M;
        case hadal_keycode_n: return ImGuiKey_N;
        case hadal_keycode_o: return ImGuiKey_O;
        case hadal_keycode_p: return ImGuiKey_P;
        case hadal_keycode_q: return ImGuiKey_Q;
        case hadal_keycode_r: return ImGuiKey_R;
        case hadal_keycode_s: return ImGuiKey_S;
        case hadal_keycode_t: return ImGuiKey_T;
        case hadal_keycode_u: return ImGuiKey_U;
        case hadal_keycode_v: return ImGuiKey_V;
        case hadal_keycode_w: return ImGuiKey_W;
        case hadal_keycode_x: return ImGuiKey_X;
        case hadal_keycode_y: return ImGuiKey_Y;
        case hadal_keycode_z: return ImGuiKey_Z;
        case hadal_keycode_f1: return ImGuiKey_F1;
        case hadal_keycode_f2: return ImGuiKey_F2;
        case hadal_keycode_f3: return ImGuiKey_F3;
        case hadal_keycode_f4: return ImGuiKey_F4;
        case hadal_keycode_f5: return ImGuiKey_F5;
        case hadal_keycode_f6: return ImGuiKey_F6;
        case hadal_keycode_f7: return ImGuiKey_F7;
        case hadal_keycode_f8: return ImGuiKey_F8;
        case hadal_keycode_f9: return ImGuiKey_F9;
        case hadal_keycode_f10: return ImGuiKey_F10;
        case hadal_keycode_f11: return ImGuiKey_F11;
        case hadal_keycode_f12: return ImGuiKey_F12;
        case hadal_keycode_f13: return ImGuiKey_F13;
        case hadal_keycode_f14: return ImGuiKey_F14;
        case hadal_keycode_f15: return ImGuiKey_F15;
        case hadal_keycode_f16: return ImGuiKey_F16;
        case hadal_keycode_f17: return ImGuiKey_F17;
        case hadal_keycode_f18: return ImGuiKey_F18;
        case hadal_keycode_f19: return ImGuiKey_F19;
        case hadal_keycode_f20: return ImGuiKey_F20;
        case hadal_keycode_f21: return ImGuiKey_F21;
        case hadal_keycode_f22: return ImGuiKey_F22;
        case hadal_keycode_f23: return ImGuiKey_F23;
        case hadal_keycode_f24: return ImGuiKey_F24;
        case hadal_keycode_apostrophe: return ImGuiKey_Apostrophe;
        case hadal_keycode_comma: return ImGuiKey_Comma;
        case hadal_keycode_minus: return ImGuiKey_Minus;
        case hadal_keycode_period: return ImGuiKey_Period;
        case hadal_keycode_slash: return ImGuiKey_Slash;
        case hadal_keycode_semicolon: return ImGuiKey_Semicolon;
        case hadal_keycode_equal: return ImGuiKey_Equal;
        case hadal_keycode_left_bracket: return ImGuiKey_LeftBracket;
        case hadal_keycode_backslash: return ImGuiKey_Backslash;
        case hadal_keycode_right_bracket: return ImGuiKey_RightBracket;
        case hadal_keycode_grave_accent: return ImGuiKey_GraveAccent;
        case hadal_keycode_caps_lock: return ImGuiKey_CapsLock;
        case hadal_keycode_scroll_lock: return ImGuiKey_ScrollLock;
        case hadal_keycode_num_lock: return ImGuiKey_NumLock;
        case hadal_keycode_print_screen: return ImGuiKey_PrintScreen;
        case hadal_keycode_pause: return ImGuiKey_Pause;
        case hadal_keycode_kp_0: return ImGuiKey_Keypad0;
        case hadal_keycode_kp_1: return ImGuiKey_Keypad1;
        case hadal_keycode_kp_2: return ImGuiKey_Keypad2;
        case hadal_keycode_kp_3: return ImGuiKey_Keypad3;
        case hadal_keycode_kp_4: return ImGuiKey_Keypad4;
        case hadal_keycode_kp_5: return ImGuiKey_Keypad5;
        case hadal_keycode_kp_6: return ImGuiKey_Keypad6;
        case hadal_keycode_kp_7: return ImGuiKey_Keypad7;
        case hadal_keycode_kp_8: return ImGuiKey_Keypad8;
        case hadal_keycode_kp_9: return ImGuiKey_Keypad9;
        case hadal_keycode_kp_decimal: return ImGuiKey_KeypadDecimal;
        case hadal_keycode_kp_divide: return ImGuiKey_KeypadDivide;
        case hadal_keycode_kp_multiply: return ImGuiKey_KeypadMultiply;
        case hadal_keycode_kp_subtract: return ImGuiKey_KeypadSubtract;
        case hadal_keycode_kp_add: return ImGuiKey_KeypadAdd;
        case hadal_keycode_kp_enter: return ImGuiKey_KeypadEnter;
        case hadal_keycode_kp_equal: return ImGuiKey_KeypadEqual;
        default: break;
    } 
    return ImGuiKey_None;
}
        /* TODO */
        /* ImGuiKey_AppBack */
        /* ImGuiKey_AppForward */
        /* ImGuiKey_Oem102 */
        /* ImGuiKey_GamepadStart */
        /* ImGuiKey_GamepadBack */
        /* ImGuiKey_GamepadFaceLeft */
        /* ImGuiKey_GamepadFaceRight */
        /* ImGuiKey_GamepadFaceUp */
        /* ImGuiKey_GamepadFaceDown */
        /* ImGuiKey_GamepadDpadLeft */
        /* ImGuiKey_GamepadDpadRight */
        /* ImGuiKey_GamepadDpadUp */
        /* ImGuiKey_GamepadDpadDown */
        /* ImGuiKey_GamepadL1 */
        /* ImGuiKey_GamepadR1 */
        /* ImGuiKey_GamepadL2 */
        /* ImGuiKey_GamepadR2 */
        /* ImGuiKey_GamepadL3 */
        /* ImGuiKey_GamepadR3 */
        /* ImGuiKey_GamepadLStickLeft */
        /* ImGuiKey_GamepadLStickRight */
        /* ImGuiKey_GamepadLStickUp */
        /* ImGuiKey_GamepadLStickDown */
        /* ImGuiKey_GamepadRStickLeft */
        /* ImGuiKey_GamepadRStickRight */
        /* ImGuiKey_GamepadRStickUp */
        /* ImGuiKey_GamepadRStickDown */
        /* ImGuiKey_MouseLeft */
        /* ImGuiKey_MouseRight */
        /* ImGuiKey_MouseMiddle */
        /* ImGuiKey_MouseX1 */
        /* ImGuiKey_MouseX2 */
        /* ImGuiKey_MouseWheelX */
        /* ImGuiKey_MouseWheelY */
        /* ImGuiKey_ReservedForModCtrl */
        /* ImGuiKey_ReservedForModShift */
        /* ImGuiKey_ReservedForModAlt */
        /* ImGuiKey_ReservedForModSuper */
        /* ImGuiKey_NamedKey_END */

static LAKE_NODISCARD lake_result LAKECALL 
impl_imgui_for_hadal(imgui_tools *out_imgui)
{
    ImGuiIO *io = igGetIO_ContextPtr(out_imgui->assembly.context);
    ImFontAtlas_AddFontDefault(io->Fonts, NULL);

    io->BackendPlatformName = "imgui_impl_lakeinthelungs";
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    (void)keycode_to_imgui_key(hadal_keycode_0);

    return LAKE_SUCCESS;
}

static LAKE_NODISCARD lake_result LAKECALL 
impl_imgui_for_moon(imgui_tools *out_imgui)
{
    (void)out_imgui;
    /* TODO create GPU resources */
    return LAKE_SUCCESS;
}

FN_IMGUI_STYLE(imgui_default_style_light)
{
    ImGuiContext *ctx = imgui->assembly.context;
    igSetCurrentContext(ctx);

    ImGuiStyle *style = igGetStyle();
    ImVec4 *colors = style->Colors;
    igStyleColorsLight(style);

    colors[ImGuiCol_Text]                   = (ImVec4){1.00f, 1.00f, 1.00f, 1.00f};
    colors[ImGuiCol_TextDisabled]           = (ImVec4){0.50f, 0.50f, 0.50f, 1.00f};
    colors[ImGuiCol_WindowBg]               = (ImVec4){0.10f, 0.10f, 0.10f, 1.00f};
    colors[ImGuiCol_ChildBg]                = (ImVec4){0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_PopupBg]                = (ImVec4){0.19f, 0.19f, 0.19f, 0.92f};
    colors[ImGuiCol_Border]                 = (ImVec4){0.19f, 0.19f, 0.19f, 0.29f};
    colors[ImGuiCol_BorderShadow]           = (ImVec4){0.00f, 0.00f, 0.00f, 0.24f};
    colors[ImGuiCol_FrameBg]                = (ImVec4){0.05f, 0.05f, 0.05f, 0.54f};
    colors[ImGuiCol_FrameBgHovered]         = (ImVec4){0.19f, 0.19f, 0.19f, 0.54f};
    colors[ImGuiCol_FrameBgActive]          = (ImVec4){0.20f, 0.22f, 0.23f, 1.00f};
    colors[ImGuiCol_TitleBg]                = (ImVec4){0.00f, 0.00f, 0.00f, 1.00f};
    colors[ImGuiCol_TitleBgActive]          = (ImVec4){0.06f, 0.06f, 0.06f, 1.00f};
    colors[ImGuiCol_TitleBgCollapsed]       = (ImVec4){0.00f, 0.00f, 0.00f, 1.00f};
    colors[ImGuiCol_MenuBarBg]              = (ImVec4){0.14f, 0.14f, 0.14f, 1.00f};
    colors[ImGuiCol_ScrollbarBg]            = (ImVec4){0.05f, 0.05f, 0.05f, 0.54f};
    colors[ImGuiCol_ScrollbarGrab]          = (ImVec4){0.34f, 0.34f, 0.34f, 0.54f};
    colors[ImGuiCol_ScrollbarGrabHovered]   = (ImVec4){0.40f, 0.40f, 0.40f, 0.54f};
    colors[ImGuiCol_ScrollbarGrabActive]    = (ImVec4){0.56f, 0.56f, 0.56f, 0.54f};
    colors[ImGuiCol_CheckMark]              = (ImVec4){0.33f, 0.67f, 0.86f, 1.00f};
    colors[ImGuiCol_SliderGrab]             = (ImVec4){0.34f, 0.34f, 0.34f, 0.54f};
    colors[ImGuiCol_SliderGrabActive]       = (ImVec4){0.56f, 0.56f, 0.56f, 0.54f};
    colors[ImGuiCol_Button]                 = (ImVec4){0.05f, 0.05f, 0.05f, 0.54f};
    colors[ImGuiCol_ButtonHovered]          = (ImVec4){0.19f, 0.19f, 0.19f, 0.54f};
    colors[ImGuiCol_ButtonActive]           = (ImVec4){0.20f, 0.22f, 0.23f, 1.00f};
    colors[ImGuiCol_Header]                 = (ImVec4){0.00f, 0.00f, 0.00f, 0.52f};
    colors[ImGuiCol_HeaderHovered]          = (ImVec4){0.00f, 0.00f, 0.00f, 0.36f};
    colors[ImGuiCol_HeaderActive]           = (ImVec4){0.20f, 0.22f, 0.23f, 0.33f};
    colors[ImGuiCol_Separator]              = (ImVec4){0.28f, 0.28f, 0.28f, 0.29f};
    colors[ImGuiCol_SeparatorHovered]       = (ImVec4){0.44f, 0.44f, 0.44f, 0.29f};
    colors[ImGuiCol_SeparatorActive]        = (ImVec4){0.40f, 0.44f, 0.47f, 1.00f};
    colors[ImGuiCol_ResizeGrip]             = (ImVec4){0.28f, 0.28f, 0.28f, 0.29f};
    colors[ImGuiCol_ResizeGripHovered]      = (ImVec4){0.44f, 0.44f, 0.44f, 0.29f};
    colors[ImGuiCol_ResizeGripActive]       = (ImVec4){0.40f, 0.44f, 0.47f, 1.00f};
    colors[ImGuiCol_Tab]                    = (ImVec4){0.00f, 0.00f, 0.00f, 0.52f};
    colors[ImGuiCol_TabHovered]             = (ImVec4){0.14f, 0.14f, 0.14f, 1.00f};
    colors[ImGuiCol_PlotLines]              = (ImVec4){1.00f, 0.00f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotLinesHovered]       = (ImVec4){1.00f, 0.00f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotHistogram]          = (ImVec4){1.00f, 0.00f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotHistogramHovered]   = (ImVec4){1.00f, 0.00f, 0.00f, 1.00f};
    colors[ImGuiCol_TableHeaderBg]          = (ImVec4){0.00f, 0.00f, 0.00f, 0.52f};
    colors[ImGuiCol_TableBorderStrong]      = (ImVec4){0.00f, 0.00f, 0.00f, 0.52f};
    colors[ImGuiCol_TableBorderLight]       = (ImVec4){0.28f, 0.28f, 0.28f, 0.29f};
    colors[ImGuiCol_TableRowBg]             = (ImVec4){0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_TableRowBgAlt]          = (ImVec4){1.00f, 1.00f, 1.00f, 0.06f};
    colors[ImGuiCol_TextSelectedBg]         = (ImVec4){0.20f, 0.22f, 0.23f, 1.00f};
    colors[ImGuiCol_DragDropTarget]         = (ImVec4){0.33f, 0.67f, 0.86f, 1.00f};
    colors[ImGuiCol_NavWindowingHighlight]  = (ImVec4){1.00f, 0.00f, 0.00f, 0.70f};
    colors[ImGuiCol_NavWindowingDimBg]      = (ImVec4){1.00f, 0.00f, 0.00f, 0.20f};
    colors[ImGuiCol_ModalWindowDimBg]       = (ImVec4){1.00f, 0.00f, 0.00f, 0.35f};

    style->WindowPadding                    = (ImVec2){8.00f, 8.00f};
    style->FramePadding                     = (ImVec2){5.00f, 2.00f};
    style->CellPadding                      = (ImVec2){6.00f, 6.00f};
    style->ItemSpacing                      = (ImVec2){6.00f, 6.00f};
    style->ItemInnerSpacing                 = (ImVec2){6.00f, 6.00f};
    style->TouchExtraPadding                = (ImVec2){0.00f, 0.00f};
    style->IndentSpacing                    = 25;
    style->ScrollbarSize                    = 15;
    style->GrabMinSize                      = 10;
    style->WindowBorderSize                 = 1;
    style->ChildBorderSize                  = 1;
    style->PopupBorderSize                  = 1;
    style->FrameBorderSize                  = 1;
    style->TabBorderSize                    = 1;
    style->WindowRounding                   = 7;
    style->ChildRounding                    = 4;
    style->FrameRounding                    = 3;
    style->PopupRounding                    = 4;
    style->ScrollbarRounding                = 9;
    style->GrabRounding                     = 3;
    style->LogSliderDeadzone                = 4;
    style->TabRounding                      = 4;
}

lake_result imgui_tools_init(imgui_tools_assembly const *assembly, imgui_tools *out_imgui)
{
    lake_result result = LAKE_SUCCESS;
    imgui_tools_fini(out_imgui);
    *out_imgui = (imgui_tools){
        .assembly = *assembly,
    };
    out_imgui->assembly.context = igCreateContext(nullptr);
    if (out_imgui->assembly.style == nullptr)
        out_imgui->assembly.style = imgui_default_style_light;

    /* set the ImGui style */
    out_imgui->assembly.style(out_imgui);

    result = impl_imgui_for_hadal(out_imgui);
    if (result != LAKE_SUCCESS) {
        lake_error("Could not implement Hadal display backend for ImGui devtools.");
        return result;
    }
    result = impl_imgui_for_moon(out_imgui);
    if (result != LAKE_SUCCESS) {
        lake_error("Could not implement Moon rendering backend for ImGui devtools.");
        return result;
    }
    lake_trace("Initialized devtools using ImGui ver. %s docking branch.", igGetVersion());
    return result;
}

FN_LAKE_WORK(imgui_tools_fini, imgui_tools *imgui)
{
    if (imgui->assembly.context) {
        ImGuiContext *ctx = imgui->assembly.context;

        /* TODO destroy GPU resources */

        lake_darray_fini(&imgui->image_sampler_pairs.da);
        igDestroyContext(ctx);
    }
    lake_zerop(imgui);
}

struct _vertex {
    vec2 pos;
    vec2 uv;
    u32  color;
};

struct _push {
    vec2                    scale;
    vec2                    translate;
    u32                     vbuf_offset;
    u32                     ibuf_offset;
    moon_device_address     vbuf_ptr;
    moon_device_address     ibuf_ptr;
    moon_texture_view_id    texture_id;
    moon_sampler_id         sampler_id;
};

static lake_result LAKECALL recreate_vertex_buffer(moon_device device, imgui_tools *imgui, u64 vbuf_new_size)
{
    moon_buffer_assembly const assembly = {
        .size = vbuf_new_size,
        .name = lake_small_string_cstr("imgui vertex buffer"),
    };
    return device.moon->interface->create_buffer(device.impl, &assembly, &imgui->vertex_buffer);
}

static lake_result LAKECALL recreate_index_buffer(moon_device device, imgui_tools *imgui, u64 ibuf_new_size)
{
    moon_buffer_assembly const assembly = {
        .size = ibuf_new_size,
        .name = lake_small_string_cstr("imgui index buffer"),
    };
    return device.moon->interface->create_buffer(device.impl, &assembly, &imgui->index_buffer);
}

ImTextureID imgui_create_texture_id(
    imgui_tools               *imgui, 
    imgui_image_context const *context)
{
    lake_darray_append_t(&imgui->image_sampler_pairs.da, imgui_image_context, context);
    return (ImTextureID){lake_darray_size(&imgui->image_sampler_pairs.da) - 1};
}

lake_result imgui_record_commands(
    imgui_tools            *imgui, 
    moon_command_recorder   cmd, 
    moon_texture_id         target_texture,
    ImDrawData const       *draw_data, 
    u32                     size_x,
    u32                     size_y)
{
    imgui->cpu_timeline++;
    if ((draw_data == nullptr) && draw_data->TotalIdxCount <= 0)
        return LAKE_INCOMPLETE;

    moon_device device = imgui->assembly.device;
    moon_interface moon = *device.moon;
    moon_buffer_assembly assembly;
    LAKE_UNUSED lake_result result;
    LAKE_UNUSED char const *dbg = "Recording ImGui draw commands failed:";

    if (imgui->raster.impl == nullptr) {
        lake_warn("%s the raster pipeline is invalid.", dbg);
        return LAKE_ERROR_MEMORY_MAP_FAILED;
    }

    result = moon.interface->read_buffer_assembly(device.impl, imgui->vertex_buffer, &assembly);
    if (result != LAKE_SUCCESS) {
        lake_warn("%s can't read vertex buffer assembly.", dbg);
        return result;
    }
    u64 vbuf_current_size = assembly.size;
    u64 vbuf_needed_size = assembly.size;

    result = moon.interface->read_buffer_assembly(device.impl, imgui->index_buffer, &assembly);
    if (result != LAKE_SUCCESS) {
        lake_warn("%s can't read index buffer assembly.", dbg);
        return result;
    }
    u64 ibuf_current_size = assembly.size;
    u64 ibuf_needed_size = assembly.size;

    if (vbuf_needed_size > vbuf_current_size) {
        u64 vbuf_new_size = vbuf_needed_size + 4096;
        result = moon.interface->destroy_buffer(device.impl, imgui->vertex_buffer);
        if (result == LAKE_SUCCESS)
            result = recreate_vertex_buffer(device, imgui, vbuf_new_size);
        if (result != LAKE_SUCCESS) {
            lake_warn("%s can't recreate vertex buffer; Sizes - %lu current, %lu needed.", dbg, vbuf_current_size, vbuf_needed_size);
            return result;
        }
    }
    if (ibuf_needed_size > ibuf_current_size) {
        u64 ibuf_new_size = ibuf_needed_size + 4096;
        result = moon.interface->destroy_buffer(device.impl, imgui->index_buffer);
        if (result == LAKE_SUCCESS)
            result = recreate_index_buffer(device, imgui, ibuf_new_size);
        if (result != LAKE_SUCCESS) {
            lake_warn("%s can't recreate index buffer; Sizes - %lu current, %lu needed.", dbg, ibuf_current_size, ibuf_needed_size);
            return result;
        }
    }
    constexpr usize max_imgui_resource_name = 8; 
    lake_static_assert(lake_is_pow2(max_imgui_resource_name), "must be a power of 2");

    void *write_ptr = nullptr;
    moon_buffer_id staging_vbuf;
    assembly = (moon_buffer_assembly){
        .size = vbuf_needed_size,
        .memory_flags = moon_memory_flag_host_access_random,
        .name = lake_small_string_cstr("imgui vertex staging buffer "),
    };
    assembly.name.len = snprintf(assembly.name.str + assembly.name.len, LAKE_SMALL_STRING_CAPACITY - assembly.name.len, "%lu", imgui->cpu_timeline & (max_imgui_resource_name - 1));

    /* stage the vertex buffer */
    result = moon.interface->create_buffer(device.impl, &assembly, &staging_vbuf);
    if (result != LAKE_SUCCESS) {
        lake_warn("%s can't create vertex staging buffer.", dbg);
        return result;
    }
    result = moon.interface->buffer_host_address(device.impl, staging_vbuf, &write_ptr);
    if (result != LAKE_SUCCESS || write_ptr == nullptr) {
        lake_warn("%s can't acquire the vertex buffer host address.", dbg);
        return result;
    }
    for (s32 i = 0; i < draw_data->CmdListsCount; i++) {
        ImDrawList const *draws = draw_data->CmdLists.Data[i];
        lake_memcpy(write_ptr, draws->VtxBuffer.Data, draws->VtxBuffer.Size * sizeof(ImDrawVert));
        write_ptr += draws->VtxBuffer.Size;
    }
    result = moon.interface->cmd_destroy_buffer_deferred(cmd.impl, staging_vbuf);

    moon_buffer_id staging_ibuf;
    assembly = (moon_buffer_assembly){
        .size = ibuf_needed_size,
        .memory_flags = moon_memory_flag_host_access_random,
        .name = lake_small_string_cstr("imgui index staging buffer "),
    };
    assembly.name.len = snprintf(assembly.name.str + assembly.name.len, LAKE_SMALL_STRING_CAPACITY - assembly.name.len, "%lu", imgui->cpu_timeline & (max_imgui_resource_name - 1));
    write_ptr = nullptr;

    /* stage the index buffer */
    result = moon.interface->create_buffer(device.impl, &assembly, &staging_ibuf);
    if (result != LAKE_SUCCESS) {
        lake_warn("%s can't create index staging buffer.", dbg);
        return result;
    }
    result = moon.interface->buffer_host_address(device.impl, staging_ibuf, &write_ptr);
    if (result != LAKE_SUCCESS || write_ptr == nullptr) {
        lake_warn("%s can't acquire the index buffer host address.", dbg);
        return result;
    }
    for (s32 i = 0; i < draw_data->CmdListsCount; i++) {
        ImDrawList const *draws = draw_data->CmdLists.Data[i];
        lake_memcpy(write_ptr, draws->IdxBuffer.Data, draws->IdxBuffer.Size * sizeof(ImDrawIdx));
        write_ptr += draws->VtxBuffer.Size;
    }
    result = moon.interface->cmd_destroy_buffer_deferred(cmd.impl, staging_ibuf);

    moon_access src_access_barrier = moon_access_host_write;
    moon_access dst_access_barrier = moon_access_transfer_read;

    moon_global_barrier const global_barrier = {
        .src_access_count = 1,
        .src_accesses = &src_access_barrier,
        .dst_access_count = 1,
        .dst_accesses = &dst_access_barrier,
    };
    moon_pipeline_barrier const pipeline_barrier_work = {
        .global_barrier = &global_barrier,
    };
    result = moon.interface->cmd_pipeline_barrier(cmd.impl, &pipeline_barrier_work);

    moon_buffer_copy_region copy_buffer_region = {
        .dst_offset = 0,
        .src_offset = 0,
        .size = ibuf_needed_size,
    };
    moon_copy_buffer copy_buffer_work = {
        .src_buffer = staging_ibuf,
        .dst_buffer = imgui->index_buffer,
        .region_count = 1,
        .regions = &copy_buffer_region,
    };
    result = moon.interface->cmd_copy_buffer(cmd.impl, &copy_buffer_work);

    copy_buffer_region.size = vbuf_needed_size;
    copy_buffer_work.src_buffer = staging_vbuf;
    copy_buffer_work.dst_buffer = imgui->vertex_buffer;
    result = moon.interface->cmd_copy_buffer(cmd.impl, &copy_buffer_work);

    src_access_barrier = moon_access_transfer_write;
    dst_access_barrier = moon_access_index_buffer;

    moon_begin_renderpass const renderpass = {
        .color_attachments[0] = {
            .texture_view.handle = target_texture.handle,
            .load_op = moon_load_op_load,
            .store_op = moon_store_op_store,
            .texture_layout = moon_layout_optimal,
            .texture_access = moon_access_color_attachment_read_write,
        },
        .color_attachment_count = 1,
        .render_area = {{.x = 0, .y = 0}, {.width = size_x, .height = size_y}},
    };
    result = moon.interface->cmd_begin_renderpass(cmd.impl, &renderpass);
    lake_dbg_assert(result == LAKE_SUCCESS, result, nullptr);

    result = moon.interface->cmd_set_raster_pipeline(cmd.impl, imgui->raster.impl);

    moon_set_index_buffer const set_index_buf = {
        .buffer = imgui->index_buffer,
        .offset = 0,
        .index_format = moon_index_format_u16,
    };
    result = moon.interface->cmd_set_index_buffer(cmd.impl, &set_index_buf);

    struct _push push = { 
        .scale = {2.f / draw_data->DisplaySize.x, 2.f / draw_data->DisplaySize.y},
        .translate = {-1.f - draw_data->DisplayPos.x * push.scale[0], -1.f - draw_data->DisplayPos.y * push.scale[1]},
    };
    ImVec2 const clip_off = draw_data->DisplayPos; /* (0,0) unless using multiple viewports */
    ImVec2 const clip_scale = draw_data->FramebufferScale; /* (1,1) unless using retina display */
    s32 global_vtx_offset = 0;
    s32 global_idx_offset = 0;
    result = moon.interface->buffer_device_address(device.impl, imgui->vertex_buffer, &push.vbuf_ptr);
    result = moon.interface->buffer_device_address(device.impl, imgui->index_buffer, &push.ibuf_ptr);

    for (s32 i = 0; i < draw_data->CmdListsCount; i++) {
        ImDrawList const *draws = draw_data->CmdLists.Data[i];
        for (s32 j = 0; j < draws->CmdBuffer.Size; j++) {
            ImDrawCmd const *command = &draws->CmdBuffer.Data[j];

            /* project scissor/clipping rectangles into framebuffer space */
            ImVec2 clip_min = {
                .x = (command->ClipRect.x - clip_off.x) * clip_scale.x, 
                .y = (command->ClipRect.y - clip_off.y) * clip_scale.y,
            };
            ImVec2 const clip_max = {
                .x = (command->ClipRect.z - clip_off.x) * clip_scale.x,
                .y = (command->ClipRect.w - clip_off.y) * clip_scale.y,
            };
            /* clamp to viewport as the drivers won't accept values that are off */
            clip_min.x = lake_clamp(clip_min.x, 0.f, (f32)size_x);
            clip_min.y = lake_clamp(clip_min.y, 0.f, (f32)size_y);
            if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                continue;

            /* apply scissor/clipping rectangle */
            lake_rect2d const rect = {
                .offset = { .x = (s32)clip_min.x, .y = (s32)clip_min.y },
                .extent = { .width = (u32)clip_max.x, .height = (u32)clip_max.y },
            };
            moon_set_scissor const set_scissor_work = {
                .scissor_count = 1,
                .scissors = &rect,
            };
            moon.interface->cmd_set_scissor(cmd.impl, &set_scissor_work);

            /* draw */
            imgui_image_context const image_context = imgui->image_sampler_pairs.v[command->TextureId];
            push.texture_id = image_context.texture_view;
            push.sampler_id = image_context.sampler;
            push.vbuf_offset = command->VtxOffset + (u32)global_vtx_offset;
            push.ibuf_offset = command->IdxOffset + (u32)global_idx_offset;

            moon_push_constants const push_work = {
                .data = &push,
                .size = sizeof(struct _push),
            };
            result = moon.interface->cmd_push_constants(cmd.impl, &push_work);

            moon_draw_indexed const draw_work = {
                .index_count = command->ElemCount,
                .first_index = command->IdxOffset + (u32)global_idx_offset,
                .vertex_offset = (s32)command->VtxOffset + global_vtx_offset,
            };
            moon.interface->cmd_draw_indexed(cmd.impl, &draw_work);
        }
        global_idx_offset += draws->IdxBuffer.Size;
        global_vtx_offset += draws->VtxBuffer.Size;
    }
    moon.interface->cmd_end_renderpass(cmd.impl);
    return LAKE_SUCCESS;
}
