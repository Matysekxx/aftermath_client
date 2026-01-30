//
// Created by chalo on 30.01.2026.
//

#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H
#include "ixwebsocket/IXWebSocket.h"


class WebSocketHandler {
    private:
        ix::WebSocket webSocket;
    public:
        explicit WebSocketHandler(const std::string &url);
        ~WebSocketHandler();
        void start();
        void stop();


};



#endif //WEBSOCKETHANDLER_H
