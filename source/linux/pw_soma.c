#include "pw_soma.h"
#ifdef SOMA_PIPEWIRE

#include <stdio.h> /* sscanf */

static bool load_pipewire_symbols(soma_adapter soma, char const *name)
{
    lake_dbg_assert(soma->pw_get_library_version && soma->pw_init, 
            LAKE_ERROR_INITIALIZATION_FAILED, "Assumed pipewire procedures are not present.");

    void *module = soma->pipewire_library;

    soma->pw_deinit = (PFN_pw_deinit)
        lake_get_proc_address(module, "pw_deinit");
    soma->pw_loop_new = (PFN_pw_loop_new)
        lake_get_proc_address(module, "pw_loop_new");           
    soma->pw_loop_destroy = (PFN_pw_loop_destroy)
        lake_get_proc_address(module, "pw_loop_destroy");
    soma->pw_loop_set_name = (PFN_pw_loop_set_name)
        lake_get_proc_address(module, "pw_loop_set_name");
    soma->pw_context_new = (PFN_pw_context_new)
        lake_get_proc_address(module, "pw_context_new");
    soma->pw_context_destroy = (PFN_pw_context_destroy)
        lake_get_proc_address(module, "pw_context_destroy");
    soma->pw_context_connect = (PFN_pw_context_connect)
        lake_get_proc_address(module, "pw_context_connect");
    soma->pw_proxy_add_object_listener = (PFN_pw_proxy_add_object_listener)
        lake_get_proc_address(module, "pw_proxy_add_object_listener");
    soma->pw_proxy_get_user_data = (PFN_pw_proxy_get_user_data)
        lake_get_proc_address(module, "pw_proxy_get_user_data");
    soma->pw_proxy_destroy = (PFN_pw_proxy_destroy)
        lake_get_proc_address(module, "pw_proxy_destroy");
    soma->pw_core_disconnect = (PFN_pw_core_disconnect)
        lake_get_proc_address(module, "pw_core_disconnect");
    soma->pw_stream_new_simple = (PFN_pw_stream_new_simple)
        lake_get_proc_address(module, "pw_stream_new_simple");
    soma->pw_stream_destroy = (PFN_pw_stream_destroy)
        lake_get_proc_address(module, "pw_stream_destroy");
    soma->pw_stream_connect = (PFN_pw_stream_connect)
        lake_get_proc_address(module, "pw_stream_connect");
    soma->pw_stream_get_state = (PFN_pw_stream_get_state)
        lake_get_proc_address(module, "pw_stream_get_state");
    soma->pw_stream_dequeue_buffer = (PFN_pw_stream_dequeue_buffer)
        lake_get_proc_address(module, "pw_stream_dequeue_buffer");
    soma->pw_stream_queue_buffer = (PFN_pw_stream_queue_buffer)
        lake_get_proc_address(module, "pw_stream_queue_buffer");
    soma->pw_properties_new = (PFN_pw_properties_new)
        lake_get_proc_address(module, "pw_properties_new");
    soma->pw_properties_set = (PFN_pw_properties_set)
        lake_get_proc_address(module, "pw_properties_set");
    soma->pw_properties_setf = (PFN_pw_properties_setf)
        lake_get_proc_address(module, "pw_properties_setf");

    if (!soma->pw_deinit ||
        !soma->pw_loop_new ||
        !soma->pw_loop_destroy ||
        !soma->pw_loop_set_name ||
        !soma->pw_context_new ||
        !soma->pw_context_destroy ||
        !soma->pw_context_connect ||
        !soma->pw_proxy_add_object_listener ||
        !soma->pw_proxy_get_user_data ||
        !soma->pw_proxy_destroy ||
        !soma->pw_core_disconnect ||
        !soma->pw_stream_new_simple ||
        !soma->pw_stream_destroy ||
        !soma->pw_stream_connect ||
        !soma->pw_stream_get_state ||
        !soma->pw_stream_dequeue_buffer ||
        !soma->pw_stream_queue_buffer ||
        !soma->pw_properties_new ||
        !soma->pw_properties_set ||
        !soma->pw_properties_setf)
    {
        lake_dbg_1("%s: can't load PipeWire procedures.", name);
        return false;
    }
    return true;
}

/** Only one pipewire backend is allowed to exist at a time. */
static soma_adapter g_soma = nullptr;

static FN_LAKE_WORK(_soma_pipewire_zero_refcnt, soma_adapter soma)
{
    if (soma == nullptr) return;

    s32 refcnt = lake_atomic_read(&soma->interface.header.refcnt);
    lake_assert(refcnt <= 0, LAKE_HANDLE_STILL_REFERENCED, nullptr);

    if (soma->pw_deinit != nullptr)
        soma->pw_deinit();
    if (soma->pipewire_library != nullptr)
        lake_close_library(soma->pipewire_library);

    __lake_free(soma);
    g_soma = nullptr;
}

FN_LAKE_INTERFACE_IMPL(soma, pipewire, lake_framework)
{
    char const *name = "soma/pipewire";

    if (lake_unlikely(g_soma != nullptr)) {
        lake_inc_refcnt(&g_soma->interface.header.refcnt);
        return g_soma;
    }
    void *pipewire_library = lake_open_library("libpipewire-0.3.so");
    if (pipewire_library == nullptr) {
        lake_dbg_1("%s: libpipewire-0.3.so is missing.", name);
        return nullptr;
    }

    PFN_pw_get_library_version _pw_get_library_version = (PFN_pw_get_library_version)
        lake_get_proc_address(pipewire_library, "pw_get_library_version");
    PFN_pw_init _pw_init = (PFN_pw_init)
        lake_get_proc_address(pipewire_library, "pw_init");
    if (_pw_get_library_version == nullptr || _pw_init == nullptr) {
        lake_dbg_1("%s: can't load PipeWire entry point procedures.", name);
        lake_close_library(pipewire_library);
        return nullptr;
    }

    char const *pipewire_version = _pw_get_library_version();
    s32 version_major, version_minor, version_patch;
    s32 nargs = sscanf(pipewire_version, "%d.%d.%d", &version_major, &version_minor, &version_patch);
    if (nargs < 3) {
        lake_dbg_1("%s: an unsupported Pipewire version of %s.", name, pipewire_version);
        lake_close_library(pipewire_library);
        return nullptr;
    }
    _pw_init(nullptr, nullptr);

    soma_adapter soma = __lake_malloc_t(struct soma_adapter_impl);
    lake_zerop(soma);
    g_soma = soma;

    soma->pipewire_library = pipewire_library;
    soma->version_major = version_major;
    soma->version_minor = version_minor;
    soma->version_patch = version_patch;
    soma->pw_get_library_version = _pw_get_library_version;
    soma->pw_init = _pw_init;

    /* write the interface header */
    soma->interface.header.framework = assembly;
    soma->interface.header.zero_refcnt = (PFN_lake_work)_soma_pipewire_zero_refcnt;
    soma->interface.header.name.len = lake_strlen(name) + 1;
    lake_memcpy(soma->interface.header.name.str, name, soma->interface.header.name.len);

    /* load pipewire symbols */
    if (lake_unlikely(!load_pipewire_symbols(soma, name))) {
        _soma_pipewire_zero_refcnt(soma);
        return nullptr;
    }

    /* XXX there are no custom `PFN_soma` procedures for now */

    lake_trace("Connected to %s, PipeWire ver. %s.", name, pipewire_version);
    lake_inc_refcnt(&soma->interface.header.refcnt);
    return soma;
}
#endif /* SOMA_PIPEWIRE */
