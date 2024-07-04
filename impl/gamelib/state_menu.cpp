#include "state_menu.hpp"
#include "drawable_helpers.hpp"
#include "fmod_errors.h"
#include "game.hpp"
#include "game_properties.hpp"
#include "shape.hpp"
#include "text.hpp"
#include "texture_manager_interface.hpp"
#include "tweens/tween_alpha.hpp"
#include <exception>
#include <fstream>
#include <sstream>

namespace {

void checkResult(FMOD_RESULT result)
{
    if (result == FMOD_OK) {
        return;
    }

    std::ostringstream oss;
    oss << "FMod Failed: (" << result << ") - " << FMOD_ErrorString(result);
    throw std::logic_error { oss.str() };
}
}

StateMenu::~StateMenu()
{
    // Do not use logger here anymore
    if (m_studioSystem)
        m_studioSystem->release();
    m_studioSystem = nullptr;
}

void StateMenu::onCreate()
{
    setupFMod();

    const float w = static_cast<float>(GP::GetScreenSize().x);
    const float h = static_cast<float>(GP::GetScreenSize().y);

    unsigned button_width = 64;
    unsigned button_height = 32;

    float button_posX_offset = 8;
    float button_posY1 = 12 + 8;
    float button_posY2 = 12 + 8 + (button_height + button_height * 0.2f) * 1;
    float button_posY3 = 12 + 8 + (button_height + button_height * 0.2f) * 2;
    float button_posY4 = 12 + 8 + (button_height + button_height * 0.2f) * 3;
    float button_posY5 = 12 + 8 + (button_height + button_height * 0.2f) * 4;

    auto icon_play = std::make_shared<jt::Sprite>("assets/play.png", textureManager());
    icon_play->setOffset(jt::OffsetMode::CENTER);

    m_buttonPlay
        = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_buttonPlay->setDrawable(icon_play);
    m_buttonPlay->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY1 });
    m_buttonPlay->addCallback([this]() {
        playCurrentTrack();
    });

    add(m_buttonPlay);

    auto icon_fade = std::make_shared<jt::Sprite>("assets/fade.png", textureManager());
    icon_fade->setOffset(jt::OffsetMode::CENTER);
    m_buttonFade = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_buttonFade->setDrawable(icon_fade);
    m_buttonFade->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY2 });
    m_buttonFade->addCallback([this]() {
        fadeOutAllTracks();
    });
    add(m_buttonFade);

    auto icon_stop = std::make_shared<jt::Sprite>("assets/stop.png", textureManager());
    icon_stop->setOffset(jt::OffsetMode::CENTER);
    m_buttonStop = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_buttonStop->setDrawable(icon_stop);
    m_buttonStop->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY3 });
    m_buttonStop->addCallback([this]() {
        stopAllTracks();
    });
    add(m_buttonStop);

    auto icon_skip = std::make_shared<jt::Sprite>("assets/skip.png", textureManager());
    icon_skip->setOffset(jt::OffsetMode::CENTER);
    m_buttonSkip = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_buttonSkip->setDrawable(icon_skip);
    m_buttonSkip->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY4 });
    m_buttonSkip->addCallback([this]() {
        setParameterForAllTracks(0.0f);
    });

    add(m_buttonSkip);

    int i = 0;
    for (auto& kvp : m_musicTracks) {
        kvp.second.button = std::make_shared<jt::Button>(jt::Vector2u { 20, 20 }, textureManager());
        auto image = std::make_shared<jt::Sprite>(kvp.second.buttonImagePath, textureManager());
        image->setOffset(jt::OffsetMode::CENTER);
        kvp.second.button->setDrawable(image);
        kvp.second.button->setPosition(jt::Vector2f { 4.0f + i * 24, button_posY5 });
        kvp.second.button->addCallback([this, &kvp]() {
            switchToTrack(kvp.first);
        });

        add(kvp.second.button);
        kvp.second.button->update(0.0f);
        i++;
    }

    m_buttonPlay->update(0.0f);
    m_buttonFade->update(0.0f);
    m_buttonStop->update(0.0f);
    m_buttonSkip->update(0.0f);
}
void StateMenu::fadeOutAllTracks()
{
    for (auto& kvp : m_musicTracks) {
        kvp.second.eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    }
}
void StateMenu::playCurrentTrack()
{
    stopAllTracks();

    getGame()->logger().info("start playing event instance: '" + m_currentTrackName + "'", { "fmod" });
    m_musicTracks[m_currentTrackName].eventInstance->start();
}

void StateMenu::setupFMod()
{
    getGame()->logger().info("FMod initialization", { "fmod" });
    checkResult(FMOD::Studio::System::create(&m_studioSystem));
    checkResult(m_studioSystem->initialize(2, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, nullptr));

    getGame()->logger().info("FMod initialization done", { "fmod" });

    FMOD::Studio::Bank* stringBank;
    checkResult(m_studioSystem->loadBankFile("assets/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringBank));

    FMOD::Studio::Bank* masterBank;
    checkResult(m_studioSystem->loadBankFile("assets/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
    int eventCount { 0 };

    checkResult(masterBank->getEventCount(&eventCount));
    getGame()->logger().info("Loaded " + std::to_string(eventCount) + " events from 'Master.bank'", { "fmod" });

    const std::vector<std::pair<std::string, std::string>> eventPaths = { { "event:/original_clemens", "assets/detective_clemens.png" }, { "event:/euroclemens", "assets/euro_clemens.png" }, { "event:/spooky_clemens", "assets/ghost_clemens.png" }, { "event:/christmasclemens", "assets/christmas_clemens.png" } };

    for (const auto& kvp : eventPaths) {

        MusicTrack track {};

        m_studioSystem->getEvent(kvp.first.c_str(), &track.eventDescription);

        if (!track.eventDescription->isValid()) {
            getGame()->logger().error("invalid event description", { "fmod" });
        }

        checkResult(track.eventDescription->getID(&track.guid));

        int parameterCount { 0 };
        checkResult(track.eventDescription->getParameterDescriptionCount(&parameterCount));
        getGame()->logger().info("Available Event Parameters: " + std::to_string(parameterCount), { "fmod" });

        FMOD_STUDIO_PARAMETER_DESCRIPTION loopParameterDescription;
        checkResult(track.eventDescription->getParameterDescriptionByIndex(0, &loopParameterDescription));
        track.loopParameterId = loopParameterDescription.id;

        getGame()->logger().info("Create fmod event instance", { "fmod" });
        checkResult(track.eventDescription->createInstance(&track.eventInstance));

        if (!track.eventInstance || !track.eventInstance->isValid()) {
            getGame()->logger().error("could not create event instance", { "fmod" });
        }

        track.buttonImagePath = kvp.second;
        m_musicTracks[kvp.first] = track;
    }

    m_currentTrackName = m_musicTracks.begin()->first;
}

void StateMenu::onEnter()
{
}

void StateMenu::onUpdate(float const /*elapsed*/)
{
    if (m_studioSystem)
        m_studioSystem->update();

    for (auto const& kvp : m_musicTracks) {
        if (kvp.first == m_currentTrackName) {
            kvp.second.button->setColor(jt::colors::White);
        } else {
            kvp.second.button->setColor(jt::colors::Gray);
        }
    }
}

void StateMenu::onDraw() const
{
    m_buttonPlay->draw();
    m_buttonFade->draw();
    m_buttonStop->draw();
    m_buttonSkip->draw();

    for (const auto& kvp : m_musicTracks) {
        kvp.second.button->draw();
    }
}

void StateMenu::stopAllTracks()
{
    for (auto& kvpInner : m_musicTracks) {
        kvpInner.second.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    }
}

void StateMenu::resetParameterOnAllTracks()
{
    setParameterForAllTracks(1.0f);
}

void StateMenu::setParameterForAllTracks(float newParamValue)
{
    for (auto& kvp : m_musicTracks) {
        checkResult(m_studioSystem->setParameterByID(kvp.second.loopParameterId, newParamValue));
    }
}

void StateMenu::switchToTrack(std::string const& newTrackName)
{
    if (!m_musicTracks.contains(newTrackName))
        throw std::invalid_argument { "can not switch to non existing track '" + newTrackName + "'" };

    stopAllTracks();
    resetParameterOnAllTracks();

    m_currentTrackName = newTrackName;
}
