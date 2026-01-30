//
// Created by chalo on 30.01.2026.
//

#include "WebSocketHandler.h"
#include <iostream>

WebSocketHandler::WebSocketHandler(std::string url, BlockingQueue<GameEvent> *queue)
    : url(std::move(url)), eventQueue(queue) {
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.set_access_channels(websocketpp::log::alevel::connect);
    m_client.set_access_channels(websocketpp::log::alevel::disconnect);

    m_client.init_asio();

    m_client.set_open_handler([this](const websocketpp::connection_hdl &hdl) {
        m_hdl = hdl;
        connected = true;
        std::cout << "Successfully connected to server via WebSocket++!" << std::endl;
    });

    m_client.set_message_handler([this](websocketpp::connection_hdl, const WSClient::message_ptr& msg) {
        try {
            const std::string data = msg->get_payload();
            const GameEvent gameEvent = parseEvent(data);
            eventQueue->enqueue(gameEvent);
        } catch (const std::exception &e) {
            std::cerr << "Error processing message: " << e.what() << std::endl;
        }
    });

    m_client.set_fail_handler([this](const websocketpp::connection_hdl& hdl) {
        std::cerr << "Connection failed!" << std::endl;
        connected = false;
        running = false;
    });

    m_client.set_close_handler([this](const websocketpp::connection_hdl& hdl) {
        std::cout << "Connection closed." << std::endl;
        connected = false;
        running = false;
    });
}

void WebSocketHandler::start() {
    if (running) return;
    running = true;

    websocketpp::lib::error_code ec;
    const WSClient::connection_ptr con = m_client.get_connection(url, ec);

    if (ec) {
        std::cerr << "Could not create connection: " << ec.message() << std::endl;
        return;
    }

    m_client.connect(con);
    networkThread = std::thread([this]() {
        m_client.run();
        std::cout << "Network thread finished" << std::endl;
    });
}

void WebSocketHandler::stop() {
    if (!running) return;

    websocketpp::lib::error_code ec;
    m_client.close(m_hdl, websocketpp::close::status::normal, "Closing connection", ec);

    m_client.stop();

    if (networkThread.joinable()) {
        networkThread.join();
    }
    running = false;
}

void WebSocketHandler::send(const std::string &message) {
    if (!connected) {
        return;
    }
    websocketpp::lib::error_code ec;
    m_client.send(m_hdl, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "Error sending message: " << ec.message() << std::endl;
    }
}

WebSocketHandler::~WebSocketHandler() {
    stop();
}
