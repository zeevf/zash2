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

/* The index in the filter to insert port number into */
#define SOCKET_PORT_INDEX_IN_FILTER (2)
/* The success value of inet_pton function */
#define SOCKET_INET_PTON_SUCCESS (1)
/* The size of an ip header in words */
#define SOCKET_IP_SIZE (5)
/* The value of a turned on flag */
#define SOCKET_FLAG_ON (1)
/* The maximum of connections to listen into */
#define SOCKET_LISTEN_MAX_CONNECTIONS (1)

struct SOCKET_syn_context {
    int raw_socket;
};

const struct sock_filter global_filter_instructions[] = {

        {BPF_LDX | BPF_B | BPF_MSH,  0, 0, 0x00000000},
        {BPF_LD | BPF_H | BPF_IND,   0, 0, 0x00000002},
        {BPF_JMP | BPF_JEQ | BPF_K,  0, 3, 0x00000000},
        {BPF_LD | BPF_B | BPF_IND,   0, 0, 0x0000000d},
        {BPF_JMP | BPF_JSET | BPF_K, 0, 1, 0x00000002},
        {BPF_RET, 0, 0, 0x00040000},
        {BPF_RET, 0, 0, 0000000000},
};


enum zash_status socket_bind_interface(int socket_fd, const char *interface)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    size_t interface_len = 0;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if (NULL == interface) {
        status = ZASH_STATUS_SOCKET_BIND_INTERFACE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the length of the interface name */
    interface_len = strlen(interface) + 1;

    /* Bind the socket to the interface */
    return_value = setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, interface, interface_len);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_BIND_INTERFACE_SETSOCKOPT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;

}


enum zash_status socket_get_tcp_syn_header(uint16_t port, struct tcphdr *header)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct tcphdr temp_header = {0};
    uint16_t network_port = 0;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if (NULL == header) {
        status = ZASH_STATUS_SOCKET_GET_TCP_SYN_HEADER_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* convert port to network byte order */
    network_port = htons(port);

    /* Initialize tcp header */
    temp_header.dest = network_port;
    temp_header.syn = SOCKET_FLAG_ON;
    temp_header.doff = SOCKET_IP_SIZE;

    /* Transfer Ownership */
    *header = temp_header;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status socket_get_address(uint16_t port, const char *ip, struct sockaddr_in *address)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct sockaddr_in temp_address = {0};
    uint16_t network_port = 0;
    in_addr_t network_ip = 0;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if (NULL == address) {
        status = ZASH_STATUS_SOCKET_GET_ADDRESS_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    if (NULL != ip) {
        /* convert ip to binary form */
        return_value = inet_pton(AF_INET, ip, &network_ip);
        if (SOCKET_INET_PTON_SUCCESS != return_value) {
            status = ZASH_STATUS_SOCKET_GET_ADDRESS_INET_PTON_FAILED;
            DEBUG_PRINT("status: %d", status);
            goto lbl_cleanup;
        }
    } else {
        /* no specific ip requested */
        network_ip = INADDR_ANY;
    }

    /* convert port to network byte order */
    network_port = htons(port);

    /* Initialize destination address */
    temp_address.sin_family = AF_INET;
    temp_address.sin_port = network_port;
    temp_address.sin_addr.s_addr = network_ip;

    /* Transfer Ownership */
    *address = temp_address;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    return status;
}


enum zash_status
socket_get_syn_filter(int16_t port, struct sock_filter **filter_instructions, size_t *filter_length)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct sock_filter *temp_filter = NULL;
    size_t i = 0;

    /* Check for valid parameters */
    if ((NULL == filter_instructions) || (NULL == filter_length)) {
        status = ZASH_STATUS_SOCKET_GET_SYN_FILTER_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for filter instructions */
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
    temp_filter[SOCKET_PORT_INDEX_IN_FILTER].k = port;

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


enum zash_status socket_attach_syn_filter(int16_t port, int raw_socket)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    struct sock_filter *filter_instructions = NULL;
    struct sock_fprog filter_program = {0};
    size_t filter_instructions_len = 0;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Get the instruction for the filter to use */
    status = socket_get_syn_filter(port, &filter_instructions, &filter_instructions_len);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Assemble syn-packet filter */
    filter_program.len = filter_instructions_len;
    filter_program.filter = filter_instructions;

    /* Set up the filter */
    return_value = setsockopt(raw_socket,
                              SOL_SOCKET,
                              SO_ATTACH_FILTER,
                              &filter_program,
                              sizeof(filter_program));
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_ATTACH_SYN_FILTER_SETSOCKOPT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    HEAPFREE(filter_instructions);

    return status;

}


enum zash_status SOCKET_syn_create(const char *interface, struct SOCKET_syn_context **context)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;
    enum zash_status temp_status = ZASH_STATUS_UNINITIALIZED;

    struct SOCKET_syn_context *temp_context = NULL;
    int temp_socket = INVALID_FILE_DESCRIPTOR;

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

    /* Bind the socket to the interface */
    status = socket_bind_interface(temp_socket, interface);
    if (ZASH_STATUS_SUCCESS != status) {
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
                                 size_t data_len)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    uint8_t *packet = NULL;
    uint8_t *payload = NULL;
    struct tcphdr header = {0};
    struct sockaddr_in address = {0};
    size_t packet_len = 0;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == ip) || (NULL == data)) {
        status = ZASH_STATUS_SOCKET_SYN_SEND_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the tcp header of the syn packet */
    status = socket_get_tcp_syn_header(port, &header);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the destination address */
    status = socket_get_address(port, ip, &address);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* allocate memory for packet */
    packet_len = sizeof(header) + data_len;
    packet = HEAPALLOCZ(sizeof(*packet) * packet_len);
    if (NULL == packet) {
        status = ZASH_STATUS_SOCKET_SYN_SEND_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    payload = packet + sizeof(header);

    /* Assemble the packet to send */
    (void)memcpy(packet, &header, sizeof(header));
    (void)memcpy(payload, data, data_len);

    /* Send the syn packet */
    return_value = sendto(context->raw_socket,
                          packet,
                          packet_len,
                          0,
                          (struct sockaddr *)&address,
                          sizeof(address));
    if (packet_len > return_value) {
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
                                    uint16_t port,
                                    size_t *data_len,
                                    uint8_t *data,
                                    char *ip)
{
    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    uint8_t *packet = NULL;
    struct iphdr *ip_header = NULL;
    uint8_t *payload = NULL;
    size_t packet_len = 0;
    struct in_addr address = {0};
    char *src_ip = NULL;
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == context) || (NULL == data_len) || (NULL == data) || (NULL == ip)) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Attach syn packet filter to the raw socket */
    status = socket_attach_syn_filter(port, context->raw_socket);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Allocate memory for packet */
    packet_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + *data_len;
    packet = HEAPALLOCZ(sizeof(*packet) * packet_len);
    if (NULL == packet) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_CALLOC_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    ip_header = (struct iphdr *)packet;
    payload = packet + sizeof(struct iphdr) + sizeof(struct tcphdr);

    /* receive the packet */
    return_value = recv(context->raw_socket, packet, packet_len, 0);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_SYN_RECEIVE_RECV_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Extract source ip address from packet header */
    address.s_addr = ip_header->saddr;
    src_ip = inet_ntoa(address);

    /* Transfer Ownership */
    *data_len = return_value - sizeof(struct iphdr) - sizeof(struct tcphdr);
    (void)memcpy(data, payload, *data_len);
    (void)strcpy(ip, src_ip);

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


enum zash_status SOCKET_tcp_server(const char *interface, uint16_t port, int *socket_fd)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int server_socket = INVALID_FILE_DESCRIPTOR;
    int temp_socket = INVALID_FILE_DESCRIPTOR;
    struct sockaddr_in address = {0};
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == interface) || (NULL == socket_fd)) {
        status = ZASH_STATUS_SOCKET_TCP_SERVER_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a socket for listening for connections */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_FILE_DESCRIPTOR == server_socket) {
        status = ZASH_STATUS_SOCKET_TCP_SERVER_SOCKET_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Bind the socket to the interface */
    status = socket_bind_interface(server_socket, interface);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the address to bind socket to */
    status = socket_get_address(port, NULL, &address);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Bind the socket to an address */
    return_value = bind(server_socket, (const struct sockaddr *)&address, sizeof(address));
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_TCP_SERVER_BIND_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Listen for connection */
    return_value = listen(server_socket, SOCKET_LISTEN_MAX_CONNECTIONS);
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_TCP_SERVER_LISTEN_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Accept the connection */
    temp_socket = accept(server_socket, NULL, NULL);
    if (INVALID_FILE_DESCRIPTOR == temp_socket) {
        status = ZASH_STATUS_SOCKET_TCP_SERVER_ACCEPT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *socket_fd = temp_socket;
    temp_socket = INVALID_FILE_DESCRIPTOR;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    CLOSE(temp_socket);
    CLOSE(server_socket);

    return status;
}


enum zash_status
SOCKET_tcp_client(const char *interface, const char *ip, uint16_t port, int *socket_fd)
{

    enum zash_status status = ZASH_STATUS_UNINITIALIZED;

    int temp_socket = INVALID_FILE_DESCRIPTOR;
    struct sockaddr_in address = {0};
    int return_value = C_STANDARD_FAILURE_VALUE;

    /* Check for valid parameters */
    if ((NULL == interface) || (NULL == ip) || (NULL == socket_fd)) {
        status = ZASH_STATUS_SOCKET_TCP_CLIENT_NULL_POINTER;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Create a socket */
    temp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_FILE_DESCRIPTOR == temp_socket) {
        status = ZASH_STATUS_SOCKET_TCP_CLIENT_SOCKET_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Bind the socket to the interface */
    status = socket_bind_interface(temp_socket, interface);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Get the address to connect to */
    status = socket_get_address(port, ip, &address);
    if (ZASH_STATUS_SUCCESS != status) {
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* connect to the destination */
    errno = 0;
    return_value = connect(temp_socket, (const struct sockaddr *)&address, sizeof(address));
    while ((C_STANDARD_FAILURE_VALUE) == return_value && (ECONNREFUSED == errno)) {
        /* Continue to try to connect until the server will listen for connections */
        errno = 0;
        return_value = connect(temp_socket, (const struct sockaddr *)&address, sizeof(address));
    }
    if (C_STANDARD_FAILURE_VALUE == return_value) {
        status = ZASH_STATUS_SOCKET_TCP_CLIENT_CONNECT_FAILED;
        DEBUG_PRINT("status: %d", status);
        goto lbl_cleanup;
    }

    /* Transfer Ownership */
    *socket_fd = temp_socket;
    temp_socket = INVALID_FILE_DESCRIPTOR;

    /* Indicate Success */
    status = ZASH_STATUS_SUCCESS;

lbl_cleanup:

    CLOSE(temp_socket);

    return status;
}
