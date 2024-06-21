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

void StateMenu::onCreate()
{

    setupFMod();

    float w
        = static_cast<float>(GP::GetScreenSize().x);
    float h = static_cast<float>(GP::GetScreenSize().y);

    m_text_Credits = std::make_shared<jt::Text>();
    m_text_Credits->loadFont("assets/font.ttf", 12u, renderTarget()->get(0));
    m_text_Credits->setText("Created by @Laguna_999 for Bloodyorange");
    m_text_Credits->setPosition({ 10, 310 });
    m_text_Credits->setColor(GP::PaletteFontFront());
    m_text_Credits->setTextAlign(jt::Text::TextAlign::LEFT);
    m_text_Credits->update(0.0f);

    m_overlay = std::make_shared<jt::Shape>();
    m_overlay->makeRect(jt::Vector2f { w, h }, textureManager());
    m_overlay->setColor(jt::Color { 0, 0, 0 });
    m_overlay->update(0);

    {
        auto tw = jt::TweenAlpha::create(m_overlay, 0.5f, sf::Uint8 { 255 }, sf::Uint8 { 0 });
        tw->setSkipTicks();
        add(tw);
    }

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

    m_button_play
        = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_play->setDrawable(icon_play);
    m_button_play->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY1 });
    m_button_play->addCallback([this]() {
        getGame()->logger().info("start playing event instance", { "fmod" });
        for (auto& kvp : m_musicTracks) {
            kvp.second.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        }

        m_musicTracks[m_currentTrackName].eventInstance->start();
    });

    add(m_button_play);

    auto icon_fade = std::make_shared<jt::Sprite>("assets/fade.png", textureManager());
    icon_fade->setOffset(jt::OffsetMode::CENTER);
    m_button_fade = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_fade->setDrawable(icon_fade);
    m_button_fade->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY2 });
    m_button_fade->addCallback([this]() {
        for (auto& kvp : m_musicTracks) {
            kvp.second.eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        }
    });
    add(m_button_fade);

    auto icon_stop = std::make_shared<jt::Sprite>("assets/stop.png", textureManager());
    icon_stop->setOffset(jt::OffsetMode::CENTER);
    m_button_stop = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_stop->setDrawable(icon_stop);
    m_button_stop->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY3 });
    m_button_stop->addCallback([this]() {
        for (auto& kvp : m_musicTracks) {
            kvp.second.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        }
    });
    add(m_button_stop);

    auto icon_skip = std::make_shared<jt::Sprite>("assets/skip.png", textureManager());
    icon_skip->setOffset(jt::OffsetMode::CENTER);
    m_button_skip = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_skip->setDrawable(icon_skip);
    m_button_skip->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY4 });
    m_button_skip->addCallback([this]() {
        for (auto& kvp : m_musicTracks) {
            checkResult(studioSystem->setParameterByID(kvp.second.loopParameterId, 0.0f));
        }
    });

    add(m_button_skip);

    int i = 0;
    for (auto& kvp : m_musicTracks) {
        auto image = std::make_shared<jt::Sprite>(kvp.second.buttonImagePath, textureManager());
        image->setOffset(jt::OffsetMode::CENTER);

        kvp.second.button = std::make_shared<jt::Button>(jt::Vector2u { 20, 20 }, textureManager());
        kvp.second.button->setDrawable(image);
        kvp.second.button->setPosition(jt::Vector2f { 4.0f + i * 24, button_posY5 });
        kvp.second.button->addCallback([this, &kvp]() {
            for (auto& kvpInner : m_musicTracks) {
                kvpInner.second.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
            }

            m_currentTrackName = kvp.first;
        });

        add(kvp.second.button);
        kvp.second.button->update(0.0f);
        i++;
    }

    m_button_play->update(0.0f);
    m_button_fade->update(0.0f);
    m_button_stop->update(0.0f);
    m_button_skip->update(0.0f);
}

void StateMenu::setupFMod()
{
    FMOD_RESULT result;
    getGame()->logger().info("FMod initialization", { "fmod" });
    checkResult(FMOD::Studio::System::create(&studioSystem));
    checkResult(studioSystem->initialize(2, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, nullptr));

    getGame()->logger().info("FMod initialization done", { "fmod" });

    FMOD::Studio::Bank* stringBank;
    checkResult(studioSystem->loadBankFile("assets/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringBank));

    FMOD::Studio::Bank* masterBank;
    checkResult(studioSystem->loadBankFile("assets/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
    int eventCount { 0 };

    checkResult(masterBank->getEventCount(&eventCount));
    getGame()->logger().info("Loaded " + std::to_string(eventCount) + " events from 'Master.bank'", { "fmod" });

    const std::vector<std::pair<std::string, std::string>> eventPaths = { { "event:/original_clemens", "assets/detective_clemens.png" }, { "event:/euroclemens", "assets/euro_clemens.png" }, { "event:/spooky_clemens", "assets/ghost_clemens.png" }, { "event:/christmasclemens", "assets/christmas_clemens.png" } };

    for (const auto& kvp : eventPaths) {

        MusicTrack track {};

        studioSystem->getEvent(kvp.first.c_str(), &track.eventDescription);

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
void StateMenu::onUpdate(float const elapsed)
{
    if (studioSystem)
        studioSystem->update();

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
    m_text_Credits->draw(renderTarget());
    m_overlay->draw(renderTarget());

    m_button_play->draw();
    m_button_fade->draw();
    m_button_stop->draw();
    m_button_skip->draw();

    for (const auto& kvp : m_musicTracks) {
        kvp.second.button->draw();
    }
}

StateMenu::~StateMenu()
{
    // Do not use logger here anymore
    if (studioSystem)
        studioSystem->release();
    studioSystem = nullptr;
}
