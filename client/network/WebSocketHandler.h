//
// Created by chalo on 30.01.2026.
//

#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <string>
#include <thread>
#include <atomic>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include "../event/BlockingQueue.h"
#include "../event/GameEvent.h"

typedef websocketpp::client<websocketpp::config::asio_client> WSClient;

class WebSocketHandler {
private:
    std::string url;
    BlockingQueue<GameEvent> *eventQueue;
    std::thread networkThread;
    std::atomic<bool> running{false};
    std::atomic<bool> connected{false};

    WSClient m_client;
    websocketpp::connection_hdl m_hdl;

public:
    explicit WebSocketHandler(std::string url, BlockingQueue<GameEvent> *eventQueue);
    ~WebSocketHandler();

    void start();
    void stop();
    void send(const std::string& message);
    bool isConnected() const { return connected; }
};

#endif //WEBSOCKETHANDLER_H
