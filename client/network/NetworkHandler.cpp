#include "NetworkHandler.h"

#include "event/GameEvent.h"

NetworkHandler::NetworkHandler(const std::string &url, BlockingQueue<GameEvent> *inputQueue) {
    this->inputQueue = inputQueue;
    webSocket = std::make_unique<ix::WebSocket>();
    webSocket->setUrl(url);
    init();
}

void NetworkHandler::init() const {
    webSocket->setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            try {
                const GameEvent gameEvent = parseEvent(msg->str);
                this->inputQueue->enqueue(gameEvent);
            } catch (const std::exception& e) {
                nlohmann::json root;
                root["type"] = "SEND_ERROR";
                root["payload"] = std::string("JSON Error: ") + e.what();
                this->inputQueue->enqueue(GameEvent(EventType::SEND_ERROR, root));
            }
        } else if (msg->type == ix::WebSocketMessageType::Open) {
            this->inputQueue->enqueue(GameEvent(EventType::CONNECTION_ESTABLISHED, {}));
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            nlohmann::json root;
            root["type"] = "SEND_ERROR";
            root["payload"] = "Connection Closed: " + std::to_string(msg->closeInfo.code) + " " + msg->closeInfo.reason;
            this->inputQueue->enqueue(GameEvent(EventType::SEND_ERROR, root));
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            nlohmann::json root;
            root["type"] = "SEND_ERROR";
            root["payload"] = "Connection Error: " + msg->errorInfo.reason;
            this->inputQueue->enqueue(GameEvent(EventType::SEND_ERROR, root));
        }
    });
}

ix::WebSocket *NetworkHandler::getWebSocket() const {
    return webSocket.get();
}

void NetworkHandler::start() const {
    webSocket->start();
}
