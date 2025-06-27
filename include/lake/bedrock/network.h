#pragma once

/** @file lake/bedrock/network.h
 *  @brief A thin network communication layer on top of UDP (User Datagram Protocol).
 *
 *  TODO docs
 */
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/endian.h>
#include <lake/bedrock/job_system.h>
#include <lake/bedrock/machina.h>
#include <lake/bedrock/time.h>

#if defined(LAKE_PLATFORM_WINDOWS)
    #ifndef LAKE_NO_PRAGMA_LINK
        #ifndef LAKE_CC_GNUC_VERSION
            #pragma comment(lib, "ws2_32.lib")
            #pragma comment(lib, "winmm.lib")
        #endif
    #endif
    #ifdef LAKE_CC_GNUC_VERSION
        #if (_WIN32_WINNT < 0x0600)
            #undef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif
    #endif /* LAKE_CC_GNUC_VERSION */
    
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <mmsystem.h>
    #include <ws2ipdef.h>

    #if defined(_WIN32) && defined(LAKE_CC_MSVC_VERSION)
        #if _MSC_VER < 1900
            typedef struct timespec {
                long tv_sec;
                long tv_nsec;
            };
        #endif
        #define CLOCK_MONOTONIC 0
    #endif /* LAKE_CC_MSVC_VERSION */

    typedef SOCKET lake_socket;
    #define LAKE_SOCKET_NULL INVALID_SOCKET

    typedef struct lake_buffer {
        usize len;
        void *data;
    } lake_buffer;

#elif defined(LAKE_PLATFORM_UNIX)
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #include <sys/socket.h>
    #include <poll.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>
    #include <fcntl.h>

    #ifdef LAKE_PLATFORM_APPLE
    #include <mach/clock.h>
    #include <mach/mach.h>
    #include <Availability.h>
    #endif /* LAKE_PLATFORM_APPLE */

    #ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
    #endif

    #define LAKE_SOCKET_NULL -1
    
    #ifdef MSG_MAXIOVLEN
    #define LAKE_BUFFER_MAXIMUM MSG_MAXIOVLEN
    #endif
    typedef s32 lake_socket;

    typedef struct lake_buffer {
        void *data;
        usize len;
    } lake_buffer;

#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
#error Emscripten builds not yet supported.
#endif /* PLATFORMS */

#define LAKE_HOST_TO_NET16(value) (LAKE_CPU_TO_BE16(value))
#define LAKE_HOST_TO_NET32(value) (LAKE_CPU_TO_BE32(value))
#define LAKE_HOST_TO_NET64(value) (LAKE_CPU_TO_BE64(value))

#define LAKE_NET_TO_HOST16(value) (LAKE_BE16_TO_CPU(value))
#define LAKE_NET_TO_HOST32(value) (LAKE_BE32_TO_CPU(value))
#define LAKE_NET_TO_HOST64(value) (LAKE_BE64_TO_CPU(value))

typedef fd_set lake_socket_set;

#define LAKE_SOCKETSET_EMPTY(sockset)          FD_ZERO(&(sockset))
#define LAKE_SOCKETSET_ADD(sockset, socket)    FD_SET(socket, &(sockset))
#define LAKE_SOCKETSET_REMOVE(sockset, socket) FD_CLR(socket, &(sockset))
#define LAKE_SOCKETSET_CHECK(sockset, socket)  FD_ISSET(socket, &(sockset))

#ifndef LAKE_BUFFER_MAXIMUM
#define LAKE_BUFFER_MAXIMUM (1 + 2 * LAKE_PROTOCOL_MAXIMUM_PACKET_COMMANDS)
#endif

#define LAKE_IPV6 1
static constexpr struct in6_addr lake_v4_anyaddr   = {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }}};
static constexpr struct in6_addr lake_v4_noaddr    = {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }}};
static constexpr struct in6_addr lake_v4_localhost = {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x01 }}};
static constexpr struct in6_addr lake_v6_anyaddr   = {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}};
static constexpr struct in6_addr lake_v6_noaddr    = {{{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }}};
static constexpr struct in6_addr lake_v6_localhost = {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}};

#define LAKE_HOST_ANY       in6addr_any
#define LAKE_HOST_BROADCAST 0xffffffffu
#define LAKE_PORT_ANY       0

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct lake_packet lake_packet;

enum : s32 {
    LAKE_PROTOCOL_MINIMUM_MTU               = 576,
    LAKE_PROTOCOL_MAXIMUM_MTU               = 4096,
    LAKE_PROTOCOL_MAXIMUM_PACKET_COMMANDS   = 32,
    LAKE_PROTOCOL_MINIMUM_WINDOW_SIZE       = 4096,
    LAKE_PROTOCOL_MAXIMUM_WINDOW_SIZE       = 65536,
    LAKE_PROTOCOL_MINIMUM_CHANNEL_COUNT     = 1,
    LAKE_PROTOCOL_MAXIMUM_CHANNEL_COUNT     = 255,
    LAKE_PROTOCOL_MAXIMUM_PEER_ID           = 0xffff,
    LAKE_PROTOCOL_MAXIMUM_FRAGMENT_COUNT    = 1024 * 1024,
};

typedef enum lake_protocol_command : u8 {
    lake_protocol_command_none = 0,
    lake_protocol_command_acknowledge,
    lake_protocol_command_connect,
    lake_protocol_command_verify_connect,
    lake_protocol_command_disconnect,
    lake_protocol_command_ping,
    lake_protocol_command_send_reliable,
    lake_protocol_command_send_unreliable,
    lake_protocol_command_send_fragment,
    lake_protocol_command_send_unsequenced,
    lake_protocol_command_bandwidth_limit,
    lake_protocol_command_throttle_configure,
    lake_protocol_command_send_unreliable_fragment,
    lake_protocol_command_count,
    lake_protocol_command_mask = 0x0f,
} lake_protocol_command;

typedef u32 lake_protocol_flags;
typedef enum lake_protocol_flag_bits : lake_protocol_flags {
    lake_protocol_flag_acknowledge  = (1u << 7),
    lake_protocol_flag_unsequenced  = (1u << 6),
    lake_protocol_flag_peer_extra   = (1u << 13),
    lake_protocol_flag_compressed   = (1u << 14),
    lake_protocol_flag_sent_time    = (1u << 15),
    lake_protocol_flag_mask         = lake_protocol_flag_peer_extra | lake_protocol_flag_compressed | lake_protocol_flag_sent_time,
    lake_protocol_session_mask      = (3u << 11),
    lake_protocol_session_shift     = 11,
} lake_protocol_flag_bits;

#if defined(LAKE_CC_MSVC_VERSION)
#pragma pack(push, 1)
#endif

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_header {
    u16                                 peer_id;
    u16                                 sent_time;
} lake_protocol_header;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_header_minimal {
    u16                                 peer_id;
} lake_protocol_header_minimal;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_command_header {
    u8                                  command;
    u8                                  channel_id;
    u16                                 reliable_sequence_number;
} lake_protocol_command_header;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_acknowledge {
    lake_protocol_command_header        header;
    u16                                 received_reliable_sequence_number;
    u16                                 received_sent_time;
} lake_protocol_acknowledge;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_connect {
    lake_protocol_command_header        header;
    u16                                 outgoing_peer_id;
    u8                                  incoming_session_id;
    u8                                  outgoing_session_id;
    u32                                 mtu;
    u32                                 window_size;
    u32                                 channel_count;
    u32                                 incoming_bandwidth;
    u32                                 outgoing_bandwidth;
    u32                                 packet_throttle_interval;
    u32                                 packet_throttle_acceleration;
    u32                                 packet_throttle_deceleration;
    u32                                 connect_id;
    u32                                 data;
} lake_protocol_connect;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_verify_connect {
    lake_protocol_command_header        header;
    u16                                 outgoing_peer_id;
    u8                                  incoming_session_id;
    u8                                  outgoing_session_id;
    u32                                 mtu;
    u32                                 window_size;
    u32                                 channel_count;
    u32                                 incoming_bandwidth;
    u32                                 outgoing_bandwidth;
    u32                                 packet_throttle_interval;
    u32                                 packet_throttle_acceleration;
    u32                                 packet_throttle_deceleration;
    u32                                 connect_id;
} lake_protocol_verify_connect;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_bandwidth_limit {
    lake_protocol_command_header        header;
    u32                                 incoming_bandwidth;
    u32                                 outgoing_bandwidth;
} lake_protocol_bandwidth_limit;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_throttle_configure {
    lake_protocol_command_header        header;
    u32                                 packet_throttle_interval;
    u32                                 packet_throttle_acceleration;
    u32                                 packet_throttle_deceleration;
} lake_protocol_throttle_configure;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_disconnect {
    lake_protocol_command_header        header;
    u32                                 data;
} lake_protocol_disconnect;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_ping {
    lake_protocol_command_header        header;
} lake_protocol_ping;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_send_reliable {
    lake_protocol_command_header        header;
    u16                                 data_length;
} lake_protocol_send_reliable;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_send_unreliable {
    lake_protocol_command_header        header;
    u16                                 unreliable_sequence_number;
    u16                                 data_length;
} lake_protocol_send_unreliable;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_send_unsequenced {
    lake_protocol_command_header        header;
    u16                                 unsequenced_group;
    u16                                 data_length;
} lake_protocol_send_unsequenced;

/* TODO docs */
typedef struct LAKE_PACKED lake_protocol_send_fragment {
    lake_protocol_command_header        header;
    u16                                 start_sequence_number;
    u16                                 data_length;
    u32                                 fragment_count;
    u32                                 fragment_number;
    u32                                 total_length;
    u32                                 fragment_offset;
} lake_protocol_send_fragment;

/* TODO docs */
typedef union LAKE_PACKED lake_protocol {
    lake_protocol_command_header        header;
    lake_protocol_acknowledge           acknowledge;
    lake_protocol_connect               connect;
    lake_protocol_verify_connect        verify_connect;
    lake_protocol_disconnect            disconnect;
    lake_protocol_ping                  ping;
    lake_protocol_send_reliable         send_reliable;
    lake_protocol_send_unreliable       send_unreliable;
    lake_protocol_send_unsequenced      send_unsequenced;
    lake_protocol_send_fragment         send_fragment;
    lake_protocol_bandwidth_limit       bandwidth_limit;
    lake_protocol_throttle_configure    throttle_configure;
} lake_protocol;

#if defined(LAKE_CC_MSVC_VERSION)
#pragma pack(pop)
#endif

typedef enum lake_socket_type : s8 {
    lake_socket_type_stream     = 1,
    lake_socket_type_datagram   = 2,
} lake_socket_type;

typedef enum lake_socket_wait_bits : u8 {
    lake_socket_wait_none       = 0u,
    lake_socket_wait_send       = (1u << 0),
    lake_socket_wait_receive    = (1u << 1),
    lake_socket_wait_interrupt  = (1u << 2),
} lake_socket_wait_bits;

typedef enum lake_socket_opt : s8 {
    lake_socket_opt_nonblock    = 1,
    lake_socket_opt_broadcast   = 2,
    lake_socket_opt_rcvbuf      = 3,
    lake_socket_opt_sndbuf      = 4,
    lake_socket_opt_reuseaddr   = 5,
    lake_socket_opt_rcvtimeo    = 6,
    lake_socket_opt_sndtimeo    = 7,
    lake_socket_opt_error       = 8,
    lake_socket_opt_nodelay     = 9,
    lake_socket_opt_ipv6_v6only = 10,
    lake_socket_opt_ttl         = 11,
} lake_socket_opt;

typedef enum lake_socket_shutdown_op : s8 {
    lake_socket_shutdown_read = 0,
    lake_socket_shutdown_write,
    lake_socket_shutdown_read_write,
} lake_socket_shutdown_op;

/** Portable internet address structure. The host must be specified in network byte-order,
 *  and the port must be in host byte-order. The constant LAKE_HOST_ANY may be used to 
 *  specify the default server host. The constant LAKE_HOST_BROADCAST may be used to specify 
 *  the broadcast address (255.255.255.255). This makes sense for lake_host_connect, but not 
 *  for lake_host_create. Once a server responds to a broadcast, the address is updated from 
 *  LAKE_HOST_BROADCAST to the server's actual IP address. */
typedef struct lake_address {
    struct in6_addr     host;
    u16                 port;
    u16                 sin6_scope_id;
} lake_address;
#define in6_equal(in6_addr_a, in6_addr_b) (lake_memcmp(&in6_addr_a, &in6_addr_b, sizeof(struct in6_addr)) == 0)

/** Packet flag bit constants. The host must be specified in network byte-order, 
 *  and the port must be in host byte-order. The constant LAKE_HOST_ANY may be used 
 *  to specify the default server host. */
typedef u32 lake_packet_flags;
typedef enum lake_packet_flag_bits : lake_packet_flags {
    /** Packet must be received by the target peer and resend attempts
     *  should be made until the packet is delivered. */
    lake_packet_flag_reliable               = (1u << 0),
    /** Packet will not be sequenced with other packets. */
    lake_packet_flag_unsequenced            = (1u << 1),
    /** Packet will not allocate data, user must supply it instead. */
    lake_packet_flag_no_allocate            = (1u << 2),
    /** Packet will be fragmented using unreliable (instead of reliable)
     *  sends if it exceeds the MTU. */
    lake_packet_flag_unreliable_fragment    = (1u << 3),
    /** Packet that was enqueued for sending unreliably should not be 
     *  dropped due to throttling and sent if possible. */
    lake_packet_flag_unthrottled            = (1u << 4),
    /** Whether the packet has been sent from all queues it has been enqueued. */
    lake_packet_flag_sent                   = (1u << 8),
} lake_packet_flag_bits;

/** A data packet that may be sent to or received from a peer. The shown fields
 *  should only be read and never modified. The data field contains the allocated 
 *  data for the packet. The data_length fields specifies the length of the allocation. */
typedef struct lake_packet {
    usize              _refcnt;         /**< Only internal use. */
    lake_packet_flags   flags;
    u8                 *data;
    usize               data_length;
    PFN_lake_work       zero_refcnt;    /**< Will be called when the packet is no longer in use. */
    void               *userdata;       /**< Applications private data. */
} lake_packet;

/* TODO docs */
typedef struct lake_acknowledgement {
    lake_list_node      acknowledgement_list;
    u32                 sent_time;
    lake_protocol       command;
} lake_acknowledgement;

/* TODO docs */
typedef struct lake_outgoing_command {
    lake_list_node      outgouing_command_list;
    u16                 reliable_sequence_number;
    u16                 unreliable_sequence_number;
    u32                 sent_time;
    u32                 round_trip_timeout;
    u32                 queue_time;
    u32                 fragment_offset;
    u16                 fragment_length;
    u16                 send_attempts;
    lake_protocol       command;
    lake_packet        *packet;
} lake_outgoing_command;

/* TODO docs */
typedef struct lake_incoming_command {
    lake_list_node      incoming_command_list;
    u16                 reliable_sequence_number;
    u16                 unreliable_sequence_number;
    lake_protocol       command;
    u32                 fragment_count;
    u32                 fragment_remaining;
    u32                *fragments;
    lake_packet        *packet;
} lake_incoming_command;

typedef enum lake_peer_state : s8 {
    lake_peer_state_disconnected = 0,
    lake_peer_state_connecting,
    lake_peer_state_acknowledging_connect,
    lake_peer_state_connection_pending,
    lake_peer_state_connection_succeeded,
    lake_peer_state_connected,
    lake_peer_state_disconnect_later,
    lake_peer_state_disconnecting,
    lake_peer_state_acknowledging_disconnect,
    lake_peer_state_zombie,
} lake_peer_state;

enum : s32 {
    LAKE_HOST_RECEIVE_BUFFER_SIZE           = 256 * 1024,
    LAKE_HOST_SEND_BUFFER_SIZE              = 256 * 1024,
    LAKE_HOST_BANDWIDTH_THROTTLE_INTERVAL   = 1000,
    LAKE_HOST_DEFAULT_MTU                   = 1392,
    LAKE_HOST_DEFAULT_MAXINUM_PACKET_SIZE   = 32 * 1024 * 1024,
    LAKE_HOST_DEFAULT_MAXINUM_WAITING_DATA  = 32 * 1024 * 1024,
    LAKE_PEER_DEFAULT_ROUND_TRIP_TIME       = 500,
    LAKE_PEER_DEFAULT_PACKET_THROTTLE       = 32,
    LAKE_PEER_PACKET_THROTTLE_SCALE         = 32,
    LAKE_PEER_PACKET_THROTTLE_COUNTER       = 7,
    LAKE_PEER_PACKET_THROTTLE_ACCELERATION  = 2,
    LAKE_PEER_PACKET_THROTTLE_DECELERATION  = 2,
    LAKE_PEER_PACKET_THROTTLE_INTERVAL      = 5000,
    LAKE_PEER_PACKET_LOSS_SCALE             = (1u << 16),
    LAKE_PEER_PACKET_LOSS_INTERVAL          = 10000,
    LAKE_PEER_WINDOW_SIZE_SCALE             = 64 * 1024,
    LAKE_PEER_TIMEOUT_LIMIT                 = 32,
    LAKE_PEER_TIMEOUT_MINIMUM               = 5000,
    LAKE_PEER_TIMEOUT_MAXIMUM               = 30000,
    LAKE_PEER_PING_INTERVAL                 = 500,
    LAKE_PEER_UNSEQUENCED_WINDOWS           = 64,
    LAKE_PEER_UNSEQUENCED_WINDOW_SIZE       = 1024,
    LAKE_PEER_FREE_UNSEQUENCED_WINDOWS      = 32,
    LAKE_PEER_RELIABLE_WINDOWS              = 16,
    LAKE_PEER_RELIABLE_WINDOW_SIZE          = 0x1000,
    LAKE_PEER_FREE_RELIABLE_WINDOWS         = 8,
};

/* TODO docs */
typedef struct lake_channel {
    u16                 outgoing_reliable_sequence_number;
    u16                 outgoing_unreliable_sequence_number;
    u16                 used_reliable_windows;
    u16                 reliable_windows[LAKE_PEER_RELIABLE_WINDOWS];
    u16                 incoming_reliable_sequence_number;
    u16                 incoming_unreliable_sequence_number;
    lake_list           incoming_reliable_commands;
    lake_list           incoming_unreliable_commands;
} lake_channel;

typedef u8 lake_peer_flags;
typedef enum lake_peer_flag_bits : lake_peer_flags {
    lake_peer_flag_needs_dispatch   = (1u << 0),
    lake_peer_flag_continue_sending = (1u << 1),
} lake_peer_flag_bits;

/** Peer shich data packets may be sent or received from. 
 *  No fields should be modified unless otherwise specified. */
typedef struct lake_peer {
    lake_list_node      dispatch_list;
    struct lake_host   *host;
    u16                 outgoing_peer_id;
    u16                 incoming_peer_id;
    u32                 connect_id;
    u8                  outgoing_session_id;
    u8                  incoming_session_id;
    lake_address        address;                /**< Internet address of the peer. */
    void               *userdata;               /**< Application private data, may be freely modified. */
    lake_peer_state     state;
    lake_channel       *channels;
    usize               channel_count;          /**< Number of channels allocated for communication with peer. */
    u32                 incoming_bandwidth;     /**< Downstream bandwidth of the client in bytes/second. */
    u32                 outgoing_bandwidth;     /**< Upstream bandwidth of the client in bytes/second. */
    u32                 incoming_bandwidth_throttle_epoch;
    u32                 outgoing_bandwidth_throttle_epoch;
    u32                 incoming_data_total;
    u64                 total_data_received;
    u32                 outgoing_data_total;
    u64                 total_data_sent;
    u32                 last_send_time;
    u32                 last_receive_time;
    u32                 next_timeout;
    u32                 earliest_timeout;
    u32                 packet_loss_epoch;
    u32                 packets_sent;
    u64                 total_packets_sent;
    u32                 packets_lost;
    u32                 total_packets_lost;
    /** Mean packet loss of reliable packets as a ratio with respect to the constant LAKE_PEER_PACKET_LOSS_SCALE. */
    u32                 packet_loss;
    u32                 packet_loss_variance;
    u32                 packet_throttle;
    u32                 packet_throttle_limit;
    u32                 packet_throttle_counter;
    u32                 packet_throttle_epoch;
    u32                 packet_throttle_acceleration;
    u32                 packet_throttle_deceleration;
    u32                 packet_throttle_interval;
    u32                 ping_interval;
    u32                 timeout_limit;
    u32                 timeout_minimum;
    u32                 timeout_maximum;
    u32                 last_round_trip_time;
    u32                 lowest_round_trip_time;
    u32                 last_round_trip_time_variance;
    /** Mean round trip time (RTT) in milliseconds, between sending a reliable packet and receiving it's acknowledgement. */
    u32                 round_trip_time;
    u32                 round_trip_time_variance;
    u32                 mtu;
    u32                 window_size;
    u32                 reliable_data_in_transit;
    u16                 outgoing_reliable_sequence_number;
    lake_list           acknowledgements;
    lake_list           sent_reliable_commands;
    lake_list           outgoing_commands;
    lake_list           outgoing_send_reliable_commands;
    lake_list           dispatched_commands;
    u16                 flags;
    u16                 reserved;
    u16                 incoming_unsequenced_group;
    u16                 outgoing_unsequenced_group;
    u32                 unsequenced_window[LAKE_PEER_UNSEQUENCED_WINDOW_SIZE / 32];
    u32                 event_data;
    usize               total_waiting_data;
} lake_peer;

/** Compresses from in_buffers[0:in_buffer_count-1], containing in_limit bytes, to out_data, 
 *  outputting at most out_limit bytes. Should return 0 on failure. */
typedef usize (LAKECALL *PFN_lake_packet_compress)(
        void               *ctx, 
        lake_buffer const  *in_buffers,
        usize               in_buffer_count,
        usize               in_limit,
        u8                 *out_data,
        usize               out_limit);
#define FN_LAKE_PACKET_COMPRESS(fn) \
    usize LAKECALL fn(void *ctx, lake_buffer const *in_buffers, usize in_buffer_count, usize in_limit, u8 *out_data, usize out_limit)

/** Decompress from in_data, containing in_limit bytes, to out_data, outputting
 *  at most out_limit bytes. Should return 0 on failure. */
typedef usize (LAKECALL *PFN_lake_packet_decompress)(
        void               *ctx,
        u8           const *in_data,
        usize               in_limit,
        u8                 *out_data,
        usize               out_limit);
#define FN_LAKE_PACKET_DECOMPRESS(fn) \
    usize LAKECALL fn(void *ctx, u8 const *in_data, usize in_limit, u8 *out_data, usize out_limit)

/** For compressing UDP packets before socket sends or receives. */
typedef struct lake_packet_compressor {
    /** Context data for the compressor, must be non-null. */
    void                       *ctx;
    PFN_lake_packet_compress    compress;
    PFN_lake_packet_decompress  decompress;
    /** Destroys the context when compression is disabled or the host is destroyed, may be null. */
    PFN_lake_work               destroy;
} lake_packet_compressor;

/** Function prototype that computes the checksum of the data held in buffers[0:buffer_count-1]. */
typedef u32 (LAKECALL *PFN_lake_checksum)(lake_buffer const *buffers, usize buffer_count);
#define FN_LAKE_CHECKSUM(fn) \
    u32 LAKECALL fn(lake_buffer const *buffers, usize buffer_count)

/** Function prototype for intercepting received raw UDP packets. Should return 1 to intercept, 
 *  0 to ignore, or -1 to propagate an error. */
typedef s32 (LAKECALL *PFN_lake_intercept)(struct lake_host *host, void *event);
#define FN_LAKE_INTERCEPT(fn) \
    s32 LAKECALL fn(struct lake_host *host, void *event)

/** A host for communicating with peers. No fields should be modified unless otherwise stated. */
typedef struct lake_host {
    lake_socket             socket;
    lake_address            address;                /**< Internet address of the host. */
    u32                     incoming_bandwidth;     /**< Downstream bandwidth of the client in bytes/second. */
    u32                     outgoing_bandwidth;     /**< Upstream bandwidth of the client in bytes/second. */
    u32                     bandwidth_throttle_epoch;
    u32                     mtu;
    u32                     random_seed;
    s32                     recalculate_bandwidth_limits;
    lake_peer              *peers;                  /**< Array of peers allocated for this host. */
    usize                   peer_count;             /**< Number of peers allocated for this host. */
    usize                   channel_limit;          /**< Maximum number of channels allowed for connected peers. */
    u32                     service_time;
    lake_list               dispatch_queue;
    u32                     total_queued;
    usize                   packet_size;
    u16                     header_flags;
    lake_protocol           commands[LAKE_PROTOCOL_MAXIMUM_PACKET_COMMANDS];
    usize                   command_count;
    lake_buffer             buffers[LAKE_BUFFER_MAXIMUM];
    usize                   buffer_count;
    PFN_lake_checksum       checksum;
    lake_packet_compressor  compressor;
    u8                      packet_data[2][LAKE_PROTOCOL_MAXIMUM_MTU];
    lake_address            received_address;
    u8                     *received_data;
    usize                   received_data_length;
    /** Total data sent, user should reset to 0 as needed to prevent overflow. */
    u32                     total_sent_data;
    /** Total UDP packets sent, user should reset to 0 as needed to prevent overflow. */
    u32                     total_sent_packets;
    /** Total data received, user should reset to 0 as needed to prevent overflow. */
    u32                     total_received_data;
    /** Total UDP packets received, user should reset to 0 as needed to prevent overflow. */
    u32                     total_received_packets;
    PFN_lake_intercept      intercept;
    usize                   connected_peers;
    usize                   bandwidth_limited_peers;
    /** Optional number of allowed peers from duplicate IPs, defaults to LAKE_PROTOCOL_MAXIMUM_PEER_ID. */
    usize                   duplicate_peers;
    /** Max allowable packet size that may be sent or received on a peer. */
    usize                   maximum_packet_size;
    /** Max aggregate amount of buffer space a peer may use waiting for packets to be delivered. */
    usize                   maximum_waiting_data;
} lake_host;

typedef enum lake_net_event_type : s8 {
    lake_net_event_type_none = 0,
    lake_net_event_type_connect,
    lake_net_event_type_disconnect,
    lake_net_event_type_receive,
    lake_net_event_type_disconnect_timeout,
} lake_net_event_type;

typedef struct lake_net_event {
    lake_net_event_type     type;       /**< Type of the event. */
    u8                      channel_id; /**< Channel on the peer that generated the event, if appropriate. */
    u32                     data;       /**< Data associated with the event, if appropriate. */
    lake_peer              *peer;       /**< Peer that generated a connect, disconnect or receive event. */
    lake_packet            *packet;     /**< Packet associated with the event, if appropriate; */
} lake_net_event;

/* TODO docs */
LAKEAPI lake_socket LAKECALL lake_socket_create(lake_socket_type type);
LAKEAPI s32 LAKECALL lake_socket_bind(lake_socket socket, lake_address const *address);
LAKEAPI s32 LAKECALL lake_socket_get_address(lake_socket socket, lake_address *address);
LAKEAPI s32 LAKECALL lake_socket_listen(lake_socket socket, s32);
LAKEAPI s32 LAKECALL lake_socket_accept(lake_socket socket, lake_address *address);
LAKEAPI s32 LAKECALL lake_socket_connect(lake_socket socket, lake_address const *address);
LAKEAPI s32 LAKECALL lake_socket_send(lake_socket socket, lake_address const *address, lake_buffer const *, usize);
LAKEAPI s32 LAKECALL lake_socket_receive(lake_socket socket, lake_address *address, lake_buffer *, usize);
LAKEAPI s32 LAKECALL lake_socket_wait(lake_socket socket, u32 *, u64);
LAKEAPI s32 LAKECALL lake_socket_set_option(lake_socket socket, lake_socket_opt opt, s32);
LAKEAPI s32 LAKECALL lake_socket_get_option(lake_socket socket, lake_socket_opt opt, s32 *);
LAKEAPI s32 LAKECALL lake_socket_shutdown(lake_socket socket, lake_socket_shutdown_op op);
LAKEAPI s32 LAKECALL lake_socket_destroy(lake_socket socket);
LAKEAPI s32 LAKECALL lake_socket_set_select(lake_socket socket, lake_socket_set *, lake_socket_set *, u32);

/** Attempts to parse printable form of the IP address in the parameter hostname 
 *  and sets the host field in the address parameter if successful. */
LAKEAPI s32 LAKECALL 
lake_address_set_host_ip(
        lake_address   *address, 
        char const     *hostname);

/** Attempts to resolve the host named by the parameter hostname and sets the host 
 *  field in the address parameter if successful. */
LAKEAPI s32 LAKECALL 
lake_address_set_host(
        lake_address   *address, 
        char const     *hostname);

/** Gives the printable form of the IP address specified in the address parameter. */
LAKEAPI s32 LAKECALL 
lake_address_get_host_ip(
        lake_address   *address, 
        char           *hostname, 
        usize           name_length);

/** Attempts to do a reverse lookup of the host field in the address parameter. */
LAKEAPI s32 LAKECALL 
lake_address_get_host(
        lake_address   *address, 
        char           *hostname, 
        usize           name_length);

/* TODO docs */
LAKEAPI u32 LAKECALL lake_host_get_peers_count(lake_host *host);
LAKEAPI u32 LAKECALL lake_host_get_packets_sent(lake_host *host);
LAKEAPI u32 LAKECALL lake_host_get_packets_received(lake_host *host);
LAKEAPI u32 LAKECALL lake_host_get_bytes_sent(lake_host *host);
LAKEAPI u32 LAKECALL lake_host_get_bytes_received(lake_host *host);
LAKEAPI u32 LAKECALL lake_host_get_received_data(lake_host *host, u8 **out_data);
LAKEAPI u32 LAKECALL lake_host_get_mtu(lake_host *host);

LAKEAPI u32 LAKECALL lake_peer_get_id(lake_peer *peer);
LAKEAPI u32 LAKECALL lake_peer_get_ip(lake_peer *peer, char *ip, usize ip_length);
LAKEAPI u16 LAKECALL lake_peer_get_port(lake_peer *peer);
LAKEAPI u32 LAKECALL lake_peer_get_rtt(lake_peer *peer);
LAKEAPI u64 LAKECALL lake_peer_get_packets_sent(lake_peer *peer);
LAKEAPI u32 LAKECALL lake_peer_get_packets_lost(lake_peer *peer);
LAKEAPI u64 LAKECALL lake_peer_get_bytes_sent(lake_peer *peer);
LAKEAPI u64 LAKECALL lake_peer_get_bytes_received(lake_peer *peer);

/* TODO docs */
LAKEAPI lake_peer_state LAKECALL lake_peer_get_state(lake_peer *peer);

/* TODO docs */
LAKEAPI void *LAKECALL lake_peer_get_data(lake_peer *peer);
LAKEAPI void LAKECALL lake_peer_set_data(lake_peer *peer, void const *data);

/* TODO docs */
LAKEAPI lake_packet *LAKECALL lake_packet_create(void const *, usize, u32);
LAKEAPI lake_packet *LAKECALL lake_packet_resize(lake_packet *packet, usize);
LAKEAPI lake_packet *LAKECALL lake_packet_copy(lake_packet *packet);
LAKEAPI void LAKECALL lake_packet_destroy(lake_packet *packet);
LAKEAPI void *LAKECALL lake_packet_get_data(lake_packet *packet);
LAKEAPI u32 LAKECALL lake_packet_get_length(lake_packet *packet);
LAKEAPI void LAKECALL lake_packet_set_zero_refcnt(lake_packet *packet, PFN_lake_work const zero_refcnt);
LAKEAPI lake_packet *LAKECALL lake_packet_create_offset(void const *, usize, usize, u32);
LAKEAPI u32 LAKECALL lake_crc32(lake_buffer const *, usize);

/* TODO docs */
LAKEAPI lake_host *LAKECALL lake_host_create(lake_address const *address, usize, usize, u32, u32);
LAKEAPI void LAKECALL lake_host_destroy(lake_host *host);
LAKEAPI lake_peer *LAKECALL lake_host_connect(lake_host *host, lake_address const *address, usize, u32);
LAKEAPI s32 LAKECALL lake_host_check_events(lake_host *host, lake_net_event *event);
LAKEAPI s32 LAKECALL lake_host_service(lake_host *host, lake_net_event *event, u32);
LAKEAPI s32 LAKECALL lake_host_send_raw(lake_host *host, lake_address const *address, u8 *data, usize bytes_to_send);
LAKEAPI s32 LAKECALL lake_host_send_raw_ex(lake_host *host, lake_address const *address, u8 *data, usize skip_bytes, usize bytes_to_send);
LAKEAPI void LAKECALL lake_host_set_intercept(lake_host *host, PFN_lake_intercept const intercept);
LAKEAPI void LAKECALL lake_host_flush(lake_host *host);
LAKEAPI void LAKECALL lake_host_broadcast(lake_host *host, u8, lake_packet *packet);
LAKEAPI void LAKECALL lake_host_compress(lake_host *host, lake_packet_compressor const *compressor);
LAKEAPI void LAKECALL lake_host_channel_limit(lake_host *host, usize);
LAKEAPI void LAKECALL lake_host_bandwidth_limit(lake_host *host, u32, u32);
LAKEAPI void LAKECALL lake_host_bandwidth_throttle(lake_host *host);

/* TODO docs */
LAKEAPI s32 LAKECALL lake_peer_send(lake_peer *peer, u8, lake_packet *packet);
LAKEAPI lake_packet *LAKECALL lake_peer_receive(lake_peer *peer, u8 *channel_id);
LAKEAPI void LAKECALL lake_peer_ping(lake_peer *peer);
LAKEAPI void LAKECALL lake_peer_ping_interval(lake_peer *peer, u32);
LAKEAPI void LAKECALL lake_peer_timeout(lake_peer *peer, u32, u32, u32);
LAKEAPI void LAKECALL lake_peer_reset(lake_peer *peer);
LAKEAPI void LAKECALL lake_peer_disconnect(lake_peer *peer, u32);
LAKEAPI void LAKECALL lake_peer_disconnect_now(lake_peer *peer, u32);
LAKEAPI void LAKECALL lake_peer_disconnect_deferred(lake_peer *peer, u32);
LAKEAPI void LAKECALL lake_peer_throttle_configure(lake_peer *peer, u32, u32, u32);
LAKEAPI s32 LAKECALL lake_peer_throttle(lake_peer *peer, u32);
LAKEAPI void LAKECALL lake_peer_reset_queues(lake_peer *peer);
LAKEAPI s32 LAKECALL lake_peer_has_outgoing_commands(lake_peer *peer);
LAKEAPI s32 LAKECALL lake_peer_setup_outgoing_command(lake_peer *peer, lake_outgoing_command *);

/* TODO docs */
LAKEAPI lake_outgoing_command *LAKECALL lake_peer_queue_outgoing_command(lake_peer *peer, lake_protocol const *protocol, lake_packet *packet, u32, u16);
LAKEAPI lake_incoming_command *LAKECALL lake_peer_queue_incoming_command(lake_peer *peer, lake_protocol const *protocol, void const *, usize, u32, u32);
LAKEAPI lake_acknowledgement  *LAKECALL lake_peer_queue_acknowledgement(lake_peer *peer, lake_protocol const *protocol, u16);

/* TODO docs */
LAKEAPI void LAKECALL lake_peer_dispatch_incoming_unreliable_commands(lake_peer *peer, lake_channel *channel);
LAKEAPI void LAKECALL lake_peer_dispatch_incoming_reliable_commands(lake_peer *peer, lake_channel *channel, lake_incoming_command *);
LAKEAPI void LAKECALL lake_peer_on_connect(lake_peer *peer);
LAKEAPI void LAKECALL lake_peer_on_disconnect(lake_peer *peer);
LAKEAPI usize LAKECALL lake_protocol_command_size(u8);

#ifdef __cplusplus
}
#endif /* __cplusplus */
