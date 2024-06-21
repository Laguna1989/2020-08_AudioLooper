#ifndef GAME_STATE_MENU_HPP_INCLUDEGUARD
#define GAME_STATE_MENU_HPP_INCLUDEGUARD

#include "SFML/Audio.hpp"
#include "button.hpp"
#include "game_state.hpp"
#include <fmod.hpp>
#include <fmod_studio.hpp>

namespace jt {
class Text;
class Shape;
}

struct MusicTrack {
    FMOD_GUID guid;
    FMOD::Studio::EventDescription* eventDescription;
    FMOD::Studio::EventInstance* eventInstance;
    FMOD_STUDIO_PARAMETER_ID loopParameterId;
    std::shared_ptr<jt::Button> button;
    std::string buttonImagePath;
};

class StateMenu : public jt::GameState {
public:
    ~StateMenu();

private:
    std::shared_ptr<jt::Text> m_text_Credits;
    std::shared_ptr<jt::Button> m_button_play;
    std::shared_ptr<jt::Button> m_button_stop;
    std::shared_ptr<jt::Button> m_button_fade;
    std::shared_ptr<jt::Button> m_button_skip;

    std::shared_ptr<jt::Shape> m_overlay;

    FMOD::Studio::System* studioSystem;

    std::map<std::string, MusicTrack> m_musicTracks;

    std::string m_currentTrackName;

    void onCreate() override;
    void onEnter() override;
    void onUpdate(float elapsed) override;
    void onDraw() const override;
    void setupFMod();
};

#endif
