
/* ____________________________________________________________________________
 *
 * file:        socketio.h
 * created:     Sun Feb 12 18:25:35 2017
 * by:          frank
 *
 * description: Socket based input output
 * ____________________________________________________________________________
 */


#ifndef SOCKETIO_H
#define SOCKETIO_H


class SocketIO
{
public:
    SocketIO(int nport);
    ~SocketIO();
    int     initClient(const char* host);
    int     initServer();
    int     rx(char* buffer, int count);
    int     rxLine(char* buffer, int count);
    int     tx(char* buffer, int count);
    int     txLine(char* buffer, int count);

private:
    int     port;
    int     sockfd0;
    int     sockfd1;
};


#endif

