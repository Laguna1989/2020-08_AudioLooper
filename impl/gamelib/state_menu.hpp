#ifndef GAME_STATE_MENU_HPP_INCLUDEGUARD
#define GAME_STATE_MENU_HPP_INCLUDEGUARD

#include "SFML/Audio.hpp"
#include "button.hpp"
#include "game_state.hpp"

namespace jt {
class Text;
class Shape;
}

class StateMenu : public jt::GameState {
private:
    std::shared_ptr<jt::Text> m_text_Credits;
    std::shared_ptr<jt::Button> m_button_play;
    std::shared_ptr<jt::Button> m_button_stop;
    std::shared_ptr<jt::Button> m_button_fade;
    std::shared_ptr<jt::Button> m_button_skip;

    std::shared_ptr<jt::Shape> m_overlay;

    void onCreate() override;
    void onEnter() override;
    void onUpdate(float elapsed) override;
    void onDraw() const override;
};

#endif
