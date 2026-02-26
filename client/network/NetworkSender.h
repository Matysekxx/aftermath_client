#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H
#include "NetworkHandler.h"
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include <thread>
#include <atomic>

/**
 * @brief Handles sending events from the client to the server.
 *
 * This class runs in a separate thread, consuming events from the output queue
 * and sending them over the WebSocket connection managed by NetworkHandler.
 */
class NetworkSender {
private:
    BlockingQueue<GameEvent> *outputQueue;
    ix::WebSocket *ws;
    std::thread senderThread;
    std::atomic<bool> running;

    /**
     * @brief The main loop of the sender thread.
     */
    void run();

public:
    /**
     * @brief Constructs the NetworkSender.
     * @param outputQueue The queue from which events to send are consumed.
     * @param networkHandler The handler managing the WebSocket connection.
     */
    NetworkSender(BlockingQueue<GameEvent> *outputQueue, NetworkHandler *networkHandler);

    ~NetworkSender();

    /**
     * @brief Starts the sender thread.
     */
    void start();

    /**
     * @brief Stops the sender thread.
     */
    void stop();

    /**
     * @brief Helper method to send a PAY_DEBT event.
     * @param amount The amount of credits to pay.
     */
    void sendPayDebt(int amount);
};


#endif //NETWORKSENDER_H
