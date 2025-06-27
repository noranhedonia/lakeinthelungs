#include <lake/bedrock/network.h>

lake_socket lake_socket_create(lake_socket_type type)
{
    (void)type;
    return LAKE_SOCKET_NULL;
}

s32 lake_socket_bind(lake_socket socket, lake_address const *address)
{
    (void)socket;
    (void)address;
    return -1;
}

s32 lake_socket_get_address(lake_socket socket, lake_address *address)
{
    (void)socket;
    (void)address;
    return -1;
}

s32 lake_socket_listen(lake_socket socket, s32)
{
    (void)socket;
    return -1;
}

s32 lake_socket_accept(lake_socket socket, lake_address *address)
{
    (void)socket;
    (void)address;
    return -1;
}

s32 lake_socket_connect(lake_socket socket, lake_address const *address)
{
    (void)socket;
    (void)address;
    return -1;
}

s32 lake_socket_send(lake_socket socket, lake_address const *address, lake_buffer const *, usize)
{
    (void)socket;
    (void)address;
    return -1;
}

s32 lake_socket_receive(lake_socket socket, lake_address *address, lake_buffer *, usize)
{
    (void)socket;
    (void)address;
    return -1;
}

s32 lake_socket_wait(lake_socket socket, u32 *, u64)
{
    (void)socket;
    return -1;
}

s32 lake_socket_set_option(lake_socket socket, lake_socket_opt opt, s32)
{
    (void)socket;
    (void)opt;
    return -1;
}

s32 lake_socket_get_option(lake_socket socket, lake_socket_opt opt, s32 *)
{
    (void)socket;
    (void)opt;
    return -1;
}

s32 lake_socket_shutdown(lake_socket socket, lake_socket_shutdown_op op)
{
    (void)socket;
    (void)op;
    return -1;
}

s32 lake_socket_destroy(lake_socket socket)
{
    (void)socket;
    return -1;
}

s32 lake_socket_set_select(lake_socket socket, lake_socket_set *, lake_socket_set *, u32)
{
    (void)socket;
    return -1;
}

s32 lake_address_set_host_ip(lake_address *address, char const *hostname)
{
    (void)address;
    (void)hostname;
    return -1;
}

s32 lake_address_set_host(lake_address *address, char const *hostname)
{
    (void)address;
    (void)hostname;
    return -1;
}

s32 lake_address_get_host_ip(lake_address *address, char *hostname, usize name_length)
{
    (void)address;
    (void)hostname;
    (void)name_length;
    return -1;
}

s32 lake_address_get_host(lake_address *address, char *hostname, usize name_length)
{
    (void)address;
    (void)hostname;
    (void)name_length;
    return -1;
}

u32 lake_host_get_peers_count(lake_host *host)
{
    (void)host;
    return 0;
}

u32 lake_host_get_packets_sent(lake_host *host)
{
    (void)host;
    return 0;
}

u32 lake_host_get_packets_received(lake_host *host)
{
    (void)host;
    return 0;
}

u32 lake_host_get_bytes_sent(lake_host *host)
{
    (void)host;
    return 0;
}

u32 lake_host_get_bytes_received(lake_host *host)
{
    (void)host;
    return 0;
}

u32 lake_host_get_received_data(lake_host *host, u8 **out_data)
{
    (void)host;
    (void)out_data;
    return 0;
}

u32 lake_host_get_mtu(lake_host *host)
{
    (void)host;
    return 0;
}


u32 lake_peer_get_id(lake_peer *peer)
{
    (void)peer;
    return 0;
}

u32 lake_peer_get_ip(lake_peer *peer, char *ip, usize ip_length)
{
    (void)peer;
    (void)ip;
    (void)ip_length;
    return 0;
}

u16 lake_peer_get_port(lake_peer *peer)
{
    (void)peer;
    return 0;
}

u32 lake_peer_get_rtt(lake_peer *peer)
{
    (void)peer;
    return 0;
}

u64 lake_peer_get_packets_sent(lake_peer *peer)
{
    (void)peer;
    return 0;
}

u32 lake_peer_get_packets_lost(lake_peer *peer)
{
    (void)peer;
    return 0;
}

u64 lake_peer_get_bytes_sent(lake_peer *peer)
{
    (void)peer;
    return 0;
}

u64 lake_peer_get_bytes_received(lake_peer *peer)
{
    (void)peer;
    return 0;
}


lake_peer_state lake_peer_get_state(lake_peer *peer)
{
    (void)peer;
    return 0;
}

void *lake_peer_get_data(lake_peer *peer)
{
    (void)peer;
    return nullptr;
}

void lake_peer_set_data(lake_peer *peer, void const *data)
{
    (void)peer;
    (void)data;
}

lake_packet *lake_packet_create(void const *, usize, u32)
{
    return nullptr;
}

lake_packet *lake_packet_resize(lake_packet *packet, usize)
{
    (void)packet;
    return nullptr;
}

lake_packet *lake_packet_copy(lake_packet *packet)
{
    (void)packet;
    return nullptr;
}

void lake_packet_destroy(lake_packet *packet)
{
    (void)packet;
}

void *lake_packet_get_data(lake_packet *packet)
{
    (void)packet;
    return nullptr;
}

u32 lake_packet_get_length(lake_packet *packet)
{
    (void)packet;
    return 0;
}

void lake_packet_set_zero_refcnt(lake_packet *packet, PFN_lake_work const zero_refcnt)
{
    (void)packet;
    (void)zero_refcnt;
}

lake_packet *lake_packet_create_offset(void const *, usize, usize, u32)
{
    return nullptr;
}

u32 lake_crc32(lake_buffer const *, usize)
{
    return 0;
}

lake_host *lake_host_create(lake_address const *address, usize, usize, u32, u32)
{
    (void)address;
    return nullptr;
}

void lake_host_destroy(lake_host *host)
{
    (void)host;
}

lake_peer *lake_host_connect(lake_host *host, lake_address const *address, usize, u32)
{
    (void)host;
    (void)address;
    return nullptr;
}

s32 lake_host_check_events(lake_host *host, lake_net_event *event)
{
    (void)host;
    (void)event;
    return -1;
}

s32 lake_host_service(lake_host *host, lake_net_event *event, u32)
{
    (void)host;
    (void)event;
    return -1;
}

s32 lake_host_send_raw(lake_host *host, lake_address const *address, u8 *data, usize bytes_to_send)
{
    (void)host;
    (void)address;
    (void)data;
    (void)bytes_to_send;
    return -1;
}

s32 lake_host_send_raw_ex(lake_host *host, lake_address const *address, u8 *data, usize skip_bytes, usize bytes_to_send)
{
    (void)host;
    (void)address;
    (void)data;
    (void)skip_bytes;
    (void)bytes_to_send;
    return -1;
}

void lake_host_set_intercept(lake_host *host, PFN_lake_intercept const intercept)
{
    (void)host;
    (void)intercept;
}

void lake_host_flush(lake_host *host)
{
    (void)host;
}

void lake_host_broadcast(lake_host *host, u8, lake_packet *packet)
{
    (void)host;
    (void)packet;
}

void lake_host_compress(lake_host *host, lake_packet_compressor const *compressor)
{
    (void)host;
    (void)compressor;
}

void lake_host_channel_limit(lake_host *host, usize)
{
    (void)host;
}

void lake_host_bandwidth_limit(lake_host *host, u32, u32)
{
    (void)host;
}

void lake_host_bandwidth_throttle(lake_host *host)
{
    (void)host;
}

s32 lake_peer_send(lake_peer *peer, u8, lake_packet *packet)
{
    (void)peer;
    (void)packet;
    return -1;
}

lake_packet *lake_peer_receive(lake_peer *peer, u8 *channel_id)
{
    (void)peer;
    (void)channel_id;
    return nullptr;
}

void lake_peer_ping(lake_peer *peer)
{
    (void)peer;
}

void lake_peer_ping_interval(lake_peer *peer, u32)
{
    (void)peer;
}

void lake_peer_timeout(lake_peer *peer, u32, u32, u32)
{
    (void)peer;
}

void lake_peer_reset(lake_peer *peer)
{
    (void)peer;
}

void lake_peer_disconnect(lake_peer *peer, u32)
{
    (void)peer;
}

void lake_peer_disconnect_now(lake_peer *peer, u32)
{
    (void)peer;
}

void lake_peer_disconnect_deferred(lake_peer *peer, u32)
{
    (void)peer;
}

void lake_peer_throttle_configure(lake_peer *peer, u32, u32, u32)
{
    (void)peer;
}

s32 lake_peer_throttle(lake_peer *peer, u32)
{
    (void)peer;
    return -1;
}

void lake_peer_reset_queues(lake_peer *peer)
{
    (void)peer;
}

s32 lake_peer_has_outgoing_commands(lake_peer *peer)
{
    (void)peer;
    return -1;
}

s32 lake_peer_setup_outgoing_command(lake_peer *peer, lake_outgoing_command *)
{
    (void)peer;
    return -1;
}

lake_outgoing_command *lake_peer_queue_outgoing_command(lake_peer *peer, lake_protocol const *protocol, lake_packet *packet, u32, u16)
{
    (void)peer;
    (void)protocol;
    (void)packet;
    return nullptr;
}

lake_incoming_command *lake_peer_queue_incoming_command(lake_peer *peer, lake_protocol const *protocol, void const *, usize, u32, u32)
{
    (void)peer;
    (void)protocol;
    return nullptr;
}

lake_acknowledgement *lake_peer_queue_acknowledgement(lake_peer *peer, lake_protocol const *protocol, u16)
{
    (void)peer;
    (void)protocol;
    return nullptr;
}

void lake_peer_dispatch_incoming_unreliable_commands(lake_peer *peer, lake_channel *channel)
{
    (void)peer;
    (void)channel;
}

void lake_peer_dispatch_incoming_reliable_commands(lake_peer *peer, lake_channel *channel, lake_incoming_command *)
{
    (void)peer;
    (void)channel;
}

void lake_peer_on_connect(lake_peer *peer)
{
    (void)peer;
}

void lake_peer_on_disconnect(lake_peer *peer)
{
    (void)peer;
}

usize lake_protocol_command_size(u8)
{
    return 0;
}
