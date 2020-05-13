/**
 * @brief Internal header for socket.c. This module contains function related to socket.
 * @aouther Z.F
 * @date 03/05/2020
 */

#ifndef ZASH_SOCKET_INTERNAL_H
#define ZASH_SOCKET_INTERNAL_H

/** Headers ***************************************************/
#include <netinet/tcp.h>
#include <linux/filter.h>

#include "socket.h"

/** Constants ************************************************/
/* The index in the filter to insert port number into */
#define SOCKET_PORT_INDEX_IN_FILTER (2)
/* The success value of inet_pton function */
#define SOCKET_INET_PTON_SUCCESS (1)
/* The size of an ip header in words */
#define SOCKET_IP_SIZE (5)
/* A window size off tcp packet */
#define SOCKET_WINDOW_SIZE (64240)
/* The value of a turned on flag */
#define SOCKET_FLAG_ON (1)
/* The maximum of connections to listen to */
#define SOCKET_LISTEN_MAX_CONNECTIONS (1)


/** Macros ****************************************************/
/* Calculate a checksum by a sum of a buffer */
#define SOCKET_CALCULATE_CHECKSUM(sum, checksum)                                            \
do {                                                                                        \
    (checksum) = ((sum) >> (BITS_IN_BYTE * sizeof(uint16_t))) + ((sum) & (uint16_t)~(0));   \
    (checksum) += (checksum) >> (BITS_IN_BYTE * sizeof(uint16_t));                          \
    (checksum) = (uint16_t)(~(checksum));                                                   \
} while (0)                                                                                 \

/** Structs ***************************************************/
/** @brief The context of syn transmitter object. send and receive syn packet. */
struct SOCKET_syn_context {
    /* The raw socket to send and receive syn packet with */
    int raw_socket;
    /* The ip of the interface to send and receive syn packet with */
    uint32_t source_ip;
};

/** @brief The packet header used to calculate tcp checksum */
struct socket_checksum_header {
    /* The source ip of the packet */
    uint32_t source_ip;
    /* The destination ip of the packet */
    uint32_t dest_ip;
    /* Padding of zeors */
    uint8_t padding;
    /* The transport layer protocol for the packet. */
    uint8_t protocol;
    /* The length of the tcp packet - length of tcp header and data, in bytes. */
    uint16_t tcp_length;
    /* The tcp header of the packet. */
    struct tcphdr tcp_header;
};

/** Globals ***************************************************/
/* Instructions for bpf filter for filtering syn packet on a specific port. */
const struct sock_filter socket_filter_instructions[] = {

        {BPF_LDX | BPF_B | BPF_MSH,  0, 0, 0x00000000},
        {BPF_LD | BPF_H | BPF_IND,   0, 0, 0x00000002},
        {BPF_JMP | BPF_JEQ | BPF_K,  0, 3, 0x00000000},
        {BPF_LD | BPF_B | BPF_IND,   0, 0, 0x0000000d},
        {BPF_JMP | BPF_JSET | BPF_K, 0, 1, 0x00000002},
        {BPF_RET, 0, 0, 0x00040000},
        {BPF_RET, 0, 0, 0000000000},
};


/** Functions ************************************************/
/**
 * @brief Bind an interface to a socket.
 *
 * @param [in]     socket_fd           the socket to bind the interface to.
 *
 * @param [in]     interface           the name of the interface to bind, as null terminated string.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status socket_bind_interface(int socket_fd, const char *interface);


/**
 * @brief Get the ip address of an interface.
 *
 * @param [in]     socket_fd           a socket bind to the devise.
 *
 * @param [in]     interface           the name of the interface to
 *                                     get its ip, as null terminated string.
 *
 * @param [out]    ip                  the ip of the interface.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status socket_get_interface_ip(int socket_fd, const char *interface, uint32_t *ip);


/**
 * @brief Calculate checksum of a buffer.
 *
 * @param [in]     buffer              a buffer to calculate its checksum
 *
 * @param [in]     buffer_len          the length of the buffer, in bytes.
 *
 * @param [out]    checksum            the checksum calculated.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status
socket_calculate_checksum(const uint8_t *buffer, size_t buffer_len, uint16_t *checksum);


/**
 * @brief Get the checksum of a tcp packet.
 *
 * @param [in]     source_ip           the source ip of the packet, in binary form.
 *
 * @param [in]     dest_ip             the destination ip of the packet, in binary form.
 *
 * @param [in]     header              the tcp header of the packet.
 *
 * @param [in,opt] data                the payload of the packet.
 *
 * @param [in]     data_len            the length of the payload, in bytes.
 *
 * @param [out]    checksum            the checksum calculated.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status socket_get_tcp_checksum(uint32_t source_ip,
                                         uint32_t dest_ip,
                                         struct tcphdr *header,
                                         const uint8_t *data,
                                         size_t data_len,
                                         uint16_t *checksum);


/**
 * @brief Get a header for a tcp packet.
 *
 * @param [in]     source_ip           the source ip of the packet, in binary form.
 *
 * @param [in]     dest_ip             the destination ip of the packet, in binary form.
 *
 * @param [in]     port                the destination port of the packet.
 *
 * @param [in,opt] data                the payload of the packet.
 *
 * @param [in]     data_len            the length of the payload, in bytes.
 *
 * @param [out]    header              a tcp header for the packet.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status socket_get_tcp_syn_header(uint32_t source_ip,
                                           uint32_t dest_ip,
                                           uint16_t port,
                                           const uint8_t *data,
                                           size_t data_len,
                                           struct tcphdr *header);


/**
 * @brief Get an address of inet family.
 *
 * @param [in]       port                the port of the address.
 *
 * @param [in, opt]  ip                  the ip of the address, as null terminated string.
 *
 * @param [out]      checksum            the checksum calculated.
 *
 * @return           return value indicating an error may returned.
 *
 */
enum zash_status socket_get_address(uint16_t port, const char *ip, struct sockaddr_in *address);


/**
 * @brief Get a filter instruction that accept only syn packet at a specific port.
 *
 * @param [in]     port                  the destination port to allow syn packet on.
 *
 * @param [out]    filter_instructions   the instruction for filter program.
 *
 * @param [out]    filter_length         the amount of instructions.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           to make sure this filter accept only tcp syn packet, attach it to inet socket
 *                 with tcp protocol.
 *
 * @note           the filter should be destroyed using free.
 *
 */
enum zash_status socket_get_syn_filter(int16_t port,
                                       struct sock_filter **filter_instructions,
                                       size_t *filter_length);


/**
 * @brief Attach a filter that accept only syn packet at a specific port to a socket.
 *
 * @param [in]     port                  the destination port to allow syn packet on.
 *
 * @param [in]     raw_socket            the socket to attach filter to.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           to make sure this filter accept only tcp syn packet, the socket must be an
 *                 inet socket with tcp protocol.
 *
 */
enum zash_status socket_attach_syn_filter(int16_t port, int raw_socket);


/**
 * @brief Connect a socket to a distant address.
 *
 * @param [in]     fd_socket             the socket to connect.
 *
 * @param [in]     address               the address to connect the socket to.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           this function may block until the distant address will listen for connections.
 *
 */
enum zash_status socket_tcp_connect(int fd_socket, struct sockaddr_in *address);


#endif //ZASH_SOCKET_INTERNAL_H
