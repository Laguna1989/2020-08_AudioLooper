#include "game.hpp"

jt::Game::Game(GfxInterface& gfx, InputManagerInterface& input,
    StateManagerInterface& stateManager, LoggerInterface& logger,
    ActionCommandManagerInterface& actionCommandManager)
    : GameBase { gfx, input, stateManager, logger, actionCommandManager }
{
    m_logger.debug("Game constructor", { "jt", "game" });
}

void jt::Game::startGame(GameLoopFunctionPtr gameloop_function)
{
    m_logger.info("start game", { "jt", "game" });
    while (gfx().window().isOpen()) {
        gameloop_function();
    }
}
