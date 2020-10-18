#ifndef GAME_STATE_MENU_HPP_INCLUDEGUARD
#define GAME_STATE_MENU_HPP_INCLUDEGUARD

#include "JamTemplate/Button.hpp"
#include "JamTemplate/GameState.hpp"
#include "PlayBar.hpp"
#include "SFML/Audio.hpp"

namespace JamTemplate {
class SmartText;
class SmartShape;
}

class StateMenu : public JamTemplate::GameState {
public:
    StateMenu();

private:
    std::shared_ptr<JamTemplate::SmartText> m_text_Credits;
    std::shared_ptr<JamTemplate::Button> m_button_play;
    std::shared_ptr<JamTemplate::Button> m_button_stop;
    std::shared_ptr<JamTemplate::Button> m_button_fade;
    std::shared_ptr<JamTemplate::Button> m_button_skip;

    std::shared_ptr<JamTemplate::SmartShape> m_overlay;

    std::shared_ptr<sf::Music> m_music;
    std::shared_ptr<PlayBar> m_playbar;

    std::string m_audio_filepath { "" };
    std::int32_t m_loop_start { 0 };
    std::int32_t m_loop_end { 0 };
    std::int32_t m_skip_to { 0 };
    std::vector<std::int32_t> m_skip_from {};
    bool m_waiting_for_skip { false };
    int m_last_position { 0 };

    bool m_starting { false };

    void doInternalUpdate(float const /*elapsed*/) override;
    void doCreate() override;
    void doInternalDraw() const override;

    void loadJson();
    // will throw an exception if a value in config file is
    // not according to specification
    void verifyParsedValues();
};

#endif
