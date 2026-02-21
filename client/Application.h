#ifndef APPLICATION_H
#define APPLICATION_H
#include "event/BlockingQueue.h"
#include "event/GameEvent.h"
#include "game/GameController.h"
#include "input/InputHandler.h"
#include "network/NetworkHandler.h"
#include "network/NetworkSender.h"
#include "audio/AudioService.h"


class Application {
private:
    std::unique_ptr<BlockingQueue<GameEvent> > fromServerToClient;
    std::unique_ptr<BlockingQueue<GameEvent> > fromClientToServer;
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<NetworkHandler> networkHandler;
    std::unique_ptr<GameController> gameController;
    std::unique_ptr<NetworkSender> networkSender;
    std::unique_ptr<AudioService> audioService;

public:
    explicit Application(const std::string &url);

    void execute() const;
};


#endif //APPLICATION_H
