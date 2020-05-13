/**
 * @brief Header for socket.c. This module contains function related to socket.
 * @aouther Z.F
 * @date 03/05/2020
 */

#ifndef ZASH_SOCKET_H
#define ZASH_SOCKET_H

/** Headers ***************************************************/
#include <stdlib.h>
#include <stdint.h>

#include "zash_status.h"

/** Structs ***************************************************/
/** @brief The context of syn transmitter object. send and receive syn packet. */
struct SOCKET_syn_context;

/** Functions ************************************************/
/**
 * @brief Create a SOCKET_syn object.
 *
 * @param [in]     interface         the name of the interface to send and receive syn packets with.
 *
 * @param [out]    context           the newly created object.
 *
 * @return         return value indicating an error may returned.
 *
 * @note           this must be freed using SOCKET_syn_destroy.
 *
 */
enum zash_status SOCKET_syn_create(const char *interface, struct SOCKET_syn_context **context);


/**
 * @brief Send a syn packet.
 *
 * @param [in]     context           the SOCKET_syn object to send packet with.
 *
 * @param [in]     ip                the destination ip to send packet to.
 *
 * @param [in]     port              the destination port to send packet to.
 *
 * @param [in,opt] data              the payload to add to the syn packet.
 *
 * @param [in]     data_len          the length of the payload, in bytes.
 *
 * @return         return value indicating an error may returned.
 *
 */
enum zash_status SOCKET_syn_send(struct SOCKET_syn_context *context,
                                 const char *ip,
                                 uint16_t port,
                                 const uint8_t *data,
                                 size_t data_len);


/**
 * @brief Receive a syn packet.
 *
 * @param [in]      context           the SOCKET_syn object to receive packet with.
 *
 * @param [in]      port              the port to listen for packet at.
 *
 * @param [in,out]  data_len          contains the maximum length of data to get from the packet.
 *                                    return the actual data size.
 *
 * @param [out]     data              the payload of the syn packet.
 *
 * @param [out]     ip                the source ip that sent the packet.
 *
 * @return          return value indicating an error may returned.
 *
 * @note            ip must be previously allocated with at least 16 bytes length, the maximum size
 *                  of an ip address.
 *
 */
enum zash_status SOCKET_syn_receive(struct SOCKET_syn_context *context,
                                    uint16_t port,
                                    size_t *data_len,
                                    uint8_t *data,
                                    char *ip);


/**
 * @brief Destroy a SOCKET_syn object.
 *
 * @param [in]      context           the SOCKET_syn object to destroy.
 *
 * @return          return value indicating an error may returned.
 *
 */
enum zash_status SOCKET_syn_destroy(struct SOCKET_syn_context *context);


/**
 * @brief Run a tcp server that listen for tcp client connection.
 *
 * @param [in]      port                the port to listen for connection on.
 *
 * @param [out]     socket_fd           the socket listening for clients.
 *
 * @return          return value indicating an error may returned.
 */
enum zash_status SOCKET_tcp_server(uint16_t port, int *socket_fd);


/**
 * @brief Accept a connection from a listening server.
 *
 * @param [in]      server_socket       a socket that listening for connections.
 *
 * @param [out]     socket_fd           the socket connected to the client.
 *
 * @return          return value indicating an error may returned.
 *
 * @note            this function may block until a tcp client will connect to the server.
 */
enum zash_status SOCKET_accept(int server_socket, int *socket_fd);

/**
 * @brief Run a tcp client that connect to a tcp server and return the connected socket.
 *
 * @param [in]      ip                  the destination ip to connect to, as null terminated string.
 *
 * @param [in]      port                the destination port to connect to.
 *
 * @param [out]     socket_fd           the socket connected to the server.
 *
 * @return          return value indicating an error may returned.
 *
 */
enum zash_status
SOCKET_tcp_client(const char *ip, uint16_t port, int *socket_fd);


#endif //ZASH_SOCKET_H
