
/* ____________________________________________________________________________
 *
 * file:        socketio.cpp
 * created:     Sun Feb 12 18:32:58 2017
 * by:          frank
 *
 * description: Socket based input output
 * ____________________________________________________________________________
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "socketio.h"


SocketIO::SocketIO(int nport)
{
    port = nport;
}


SocketIO::~SocketIO()
{
    close(sockfd0);
    close(sockfd1);
}


int SocketIO::initClient(const char* host)
{
    int     rc  = 0;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd1 = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (sockfd1 < 0) {
        perror("ERROR opening socket\n");
        rc = 1;
    }

    server = gethostbyname(host);

    if (server == NULL) {
        perror("ERROR, no such host\n");
        rc = 2;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    // connect always returns -1 for non blocking sockets -> man connect
    if (connect(sockfd1, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {
            perror("ERROR connecting\n");
            rc = 3;
        }
    }

    return(rc);
}


int SocketIO::initServer()
{
    int     rc  = 0;

    struct sockaddr_in cli_addr, serv_addr;
    socklen_t clilen;

    sockfd0 = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd0 < 0) {
        perror("ERROR opening socket\n");
        rc = 1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    int enable = 1;

    if (setsockopt(sockfd0, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        rc = 2;
    }

    if (bind(sockfd0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding\n");
        rc = 3;
    }

    listen(sockfd0, 5);

    clilen = sizeof(cli_addr);
    sockfd1 = accept(sockfd0, (struct sockaddr *) &cli_addr, &clilen);

    if (sockfd1 < 0) {
        perror("ERROR on accept\n");
        rc = 4;
    }

    return(rc);
}


int SocketIO::rx(char* buffer, int count)
{
    int n = read(sockfd1, buffer, count);

    /*
     *  n < 0 is acceptable for non blocking operation
     *
        if (n < 0) {
        printf("ERROR reading from socket\n");
        }
     *
     */

    return(n);
}


int SocketIO::rxLine(char* buffer, int count)
{
    char    ch;
    int     i = read(sockfd1, &ch, 1);

    if (i > 0) {
        buffer[0] = ch;

        while (i < count && ch != '\n') {
            if (read(sockfd1, &ch, 1) > 0) {
                buffer[i++] = ch;
            }
        }
    }

    return(i);
}


int SocketIO::tx(char* buffer, int count)
{
    int n = write(sockfd1, buffer, count);

    if (n < 0) {
        perror("ERROR writing to socket\n");
    }

    return(n);
}


int SocketIO::txLine(char* buffer, int count)
{
    int n = write(sockfd1, buffer, count);

    if (n < 0) {
        perror("ERROR writing to socket\n");
    }

    return(n);
}

