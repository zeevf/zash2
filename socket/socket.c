//
// Created by user on 5/3/20.
//

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/filter.h>

#include "common.h"

#include "socket.h"


struct SOCKET_syn_context {
    int raw_socket;
};

const struct sock_filter global_filter_instructions[] = {

        {0x28, 0, 0, 0x00000014},
        {0x45, 6, 0, 0x00001fff},
        {0xb1, 0, 0, 0x0000000e},
        {0x48, 0, 0, 0x00000010},
        {0x15, 0, 3, 0},
        {0x50, 0, 0, 0x0000001b},
        {0x15, 0, 1, 0x00000002},
        {0x6,  0, 0, 0x00040000},
        {0x6,  0, 0, 0x00000000},};


enum zash_status
socket_get_syn_filter(int16_t port, struct sock_filter **filter_instructions, size_t *filter_length)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct sock_filter *temp_filter = NULL;
    size_t i = 0;

    /* Check for valid parameters */
    if (NULL == filter_instructions) {
        status = ZASH_STATUS_SOCKET_GET_SYN_FILTER_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for new context */
    temp_filter = HEAPALLOCZ(sizeof(*temp_filter) * ARRAY_LEN(global_filter_instructions));
    if (NULL == temp_filter) {
        status = ZASH_STATUS_SOCKET_GET_SYN_FILTER_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Copy the instructions for a syn packet filter. */
    for (i = 0; i < ARRAY_LEN(global_filter_instructions); ++i) {
        temp_filter[i] = global_filter_instructions[i];
    }

    /* Change the port value for the listening port */
    temp_filter[4].k = port; //TODO: magic

    /* transfer Ownership */
    *filter_length = ARRAY_LEN(global_filter_instructions);
    *filter_instructions = temp_filter;
    temp_filter = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(temp_filter);

    return status;

}


enum zash_status SOCKET_syn_create(const char *interface, struct SOCKET_syn_context **context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SOCKET_syn_context *temp_context = NULL;
    int temp_socket = INVALID_FILE_DESCRIPTOR;
    size_t interface_len = 0;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == interface)) {
        status = ZASH_STATUS_SOCKET_SYN_CREATE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for new context */
    temp_context = HEAPALLOCZ(sizeof(*temp_context));
    if (NULL == temp_context) {
        status = ZASH_STATUS_SOCKET_SYN_CREATE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }
    temp_context->raw_socket = INVALID_FILE_DESCRIPTOR;

    /* Create a new raw socket */
    temp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (INVALID_FILE_DESCRIPTOR == temp_socket) {
        status = ZASH_STATUS_SOCKET_SYN_CREATE_SOCKET_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the length of the interface name */
    interface_len = strlen(interface);

    /* Bind the socket to the interface */
    return_value = setsockopt(temp_socket, SOL_SOCKET, SO_BINDTODEVICE, interface, interface_len);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_SYN_CREATE_SETSOCKOPT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    temp_context->raw_socket = temp_socket;
    temp_socket = INVALID_FILE_DESCRIPTOR;
    *context = temp_context;
    temp_context = NULL;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    CLOSE(temp_socket);

    if (NULL != temp_context) {
        temp_status = SOCKET_syn_destroy(temp_context);
        ZASH_UPDATE_STATUS(status, temp_status);
    }

    return status;

}


enum zash_status SOCKET_syn_send(struct SOCKET_syn_context *context,
                                 const char *ip,
                                 uint16_t port,
                                 const uint8_t *data,
                                 size_t data_len
)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    uint8_t *packet = NULL;
    uint8_t *payload = NULL;
    struct tcphdr header = {0};
    struct sockaddr_in address = {0};
    size_t packet_len = 0;
    uint16_t network_port = 0;
    in_addr_t network_ip = 0;
    int send_return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == ip) || (NULL == data)) {
        status = ZASH_STATUS_SOCKET_SYN_SEND_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* convert ip and port to network byte order */
    network_port = htons(port);
    network_ip = inet_addr(ip); //TODO: may fail!!!

    /* Initialize tcp header */
    header.dest = network_port;
    header.syn = 1; //TODO: magic
    header.doff = 5; //TODO: magic

    /* Initialize destination address */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = network_ip;
    address.sin_port = network_port;

    packet_len = sizeof(header) + data_len;

    /* allocate memory for packet */
    packet = HEAPALLOCZ(sizeof(*packet) * packet_len);
    if (NULL == packet) {
        status = ZASH_STATUS_SOCKET_SYN_SEND_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble the packet to send */
    payload = memcpy(packet, &header, sizeof(header));
    (void)memcpy(payload, data, data_len);

    /* Send the syn packet */
    send_return_value = sendto(context->raw_socket,
                               packet,
                               packet_len,
                               0,
                               (struct sockaddr *)&address,
                               sizeof(address));
    if (packet_len > send_return_value) {
        status = ZASH_STATUS_SOCKET_SYN_SEND_SEND_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(packet);

    return status;
}


enum zash_status SOCKET_syn_receive(struct SOCKET_syn_context *context,
                                    uint16_t listen_port,
                                    size_t *data_len,
                                    uint8_t *data,
                                    char *src_ip
)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    uint8_t *packet = NULL;
    struct iphdr *ip_header = (struct iphdr *)packet;
    uint8_t *payload = packet + sizeof(struct iphdr) + sizeof(struct tcphdr);
    size_t packet_len = 0;
    struct in_addr address = {0};
    char *temp_src_ip = NULL;
    int return_value = C_STANDARD_FAILURE_VALUE;

    struct sock_filter *filter_instructions = NULL;
    struct sock_fprog filter_program = {0};
    size_t filter_instructions_len = 0;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == data_len) || (NULL == data) || (NULL == src_ip)) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the instruction for the filter to use */
    status = socket_get_syn_filter(listen_port, &filter_instructions, &filter_instructions_len);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble syn-packet filter */
    filter_program.len = filter_instructions_len;
    filter_program.filter = filter_instructions;

    /* Set up the filter */
    return_value = setsockopt(context->raw_socket,
                              SOL_SOCKET,
                              SO_ATTACH_FILTER,
                              &filter_program,
                              sizeof(filter_program));
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_SETSOCKOPT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    packet_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + *data_len;

    packet = HEAPALLOCZ(sizeof(*packet) * packet_len);
    if (NULL == packet) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    return_value = recv(context->raw_socket, packet, packet_len, 0);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_RECV_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Extract src ip address */
    address.s_addr = ip_header->saddr;
    temp_src_ip = inet_ntoa(address);

    /* Transfer Ownership */
    *data_len = return_value - sizeof(struct iphdr) - sizeof(struct tcphdr);
    (void)memcpy(data, payload, *data_len);
    (void)strcpy(src_ip, temp_src_ip);

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(packet);

    return status;
}


enum zash_status SOCKET_syn_destroy(struct SOCKET_syn_context *context)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    if (NULL != context) {
        CLOSE(context->raw_socket);
        HEAPFREE(context);
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

    return status;
}