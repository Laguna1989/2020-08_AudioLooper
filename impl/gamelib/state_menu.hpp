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
    ~StateMenu() override;

private:
    std::shared_ptr<jt::Button> m_buttonPlay;
    std::shared_ptr<jt::Button> m_buttonStop;
    std::shared_ptr<jt::Button> m_buttonFade;
    std::shared_ptr<jt::Button> m_buttonSkip;

    FMOD::Studio::System* m_studioSystem;

    std::map<std::string, MusicTrack> m_musicTracks;
    std::string m_currentTrackName;

    void onCreate() override;
    void onEnter() override;
    void onUpdate(float elapsed) override;
    void onDraw() const override;
    void setupFMod();

    void stopAllTracks();
    void resetParameterOnAllTracks();
    void playCurrentTrack();
    void fadeOutAllTracks();
    void setParameterForAllTracks(float newParamValue);
    void switchToTrack(std::string const& newTrackName);
};

#endif
