#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H
#include "ixwebsocket/IXWebSocket.h"
#include <string>

#include "event/BlockingQueue.h"
#include "event/GameEvent.h"

/**
 * @brief Manages the WebSocket connection to the server.
 *
 * This class handles the low-level network communication using IXWebSocket.
 * It receives messages from the server, parses them into GameEvents, and
 * pushes them into the input queue for the GameController to process.
 */
class NetworkHandler {
    BlockingQueue<GameEvent> *inputQueue;
    std::unique_ptr<ix::WebSocket> webSocket;

    /**
     * @brief Configures the WebSocket callbacks and options.
     */
    void init() const;

public:
    /**
     * @brief Constructs the NetworkHandler.
     * @param url The WebSocket URL to connect to.
     * @param inputQueue The queue where received events will be pushed.
     */
    NetworkHandler(const std::string &url, BlockingQueue<GameEvent> *inputQueue);

    /**
     * @brief Returns the underlying WebSocket instance.
     * @return Pointer to the ix::WebSocket object.
     */
    [[nodiscard]] ix::WebSocket *getWebSocket() const;

    /**
     * @brief Starts the WebSocket connection.
     *
     * This method initiates the connection process asynchronously.
     */
    void start() const;
};


#endif //NETWORKHANDLER_H
