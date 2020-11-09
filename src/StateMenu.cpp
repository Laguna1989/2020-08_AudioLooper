#include "StateMenu.hpp"
#include "GameProperties.hpp"
#include "JamTemplate/Game.hpp"
#include "JamTemplate/SmartShape.hpp"
#include "JamTemplate/SmartText.hpp"
#include "JamTemplate/TextureManager.hpp"
#include "JamTemplate/TweenAlpha.hpp"
#include "JamTemplate/TweenVolume.hpp"
#include "rapidjson/document.h"
#include <exception>
#include <fstream>

StateMenu::StateMenu() = default;

void StateMenu::doInternalUpdate(float const /*elapsed*/)
{
    auto const current_position = m_music->getPlayingOffset().asMilliseconds();
    m_playbar->setPosition(m_music->getPlayingOffset().asMilliseconds());
    if (m_waiting_for_skip) {

        for (auto const v : m_skip_from) {
            if (current_position >= v && m_last_position < v) {
                m_music->setPlayingOffset(sf::Time(sf::milliseconds(m_skip_to)));
                break;
            }
        }
    }
    m_last_position = current_position;
}

void StateMenu::doCreate()
{
    float w = static_cast<float>(getGame()->getRenderTarget()->getSize().x);
    float h = static_cast<float>(getGame()->getRenderTarget()->getSize().y);

    m_text_Credits = std::make_shared<JamTemplate::SmartText>();
    m_text_Credits->loadFont("assets/font.ttf");
    m_text_Credits->setCharacterSize(12U);
    m_text_Credits->setText("Created by @Laguna_999 for Bloodyorange");
    m_text_Credits->setPosition({ 10, 310 });
    m_text_Credits->setColor(GP::PaletteFontFront());
    m_text_Credits->SetTextAlign(JamTemplate::SmartText::TextAlign::LEFT);
    m_text_Credits->update(0.0f);

    m_overlay = std::make_shared<JamTemplate::SmartShape>();
    m_overlay->makeRect(sf::Vector2f { w, h });
    m_overlay->setColor(sf::Color { 0, 0, 0 });
    m_overlay->update(0);

    {
        auto tw = JamTemplate::TweenAlpha<JamTemplate::SmartShape>::create(m_overlay, 0.5f, sf::Uint8 { 255 }, sf::Uint8 { 0 });
        tw->setSkipFrames();
        add(tw);
    }

    unsigned button_width = 64;
    unsigned button_height = 32;

    float button_posX_offset = 8;
    float button_posY1 = 12 + 8;
    float button_posY2 = 12 + 8 + (button_height + button_height * 0.2f) * 1;
    float button_posY3 = 12 + 8 + (button_height + button_height * 0.2f) * 2;
    float button_posY4 = 12 + 8 + (button_height + button_height * 0.2f) * 3;

    auto icon_play = std::make_shared<JamTemplate::SmartSprite>();
    icon_play->loadSprite("assets/play.png");

    m_button_play
        = std::make_shared<JamTemplate::Button>(sf::Vector2u { button_width, button_height });
    m_button_play->setPosition(sf::Vector2f { w / 4 - button_posX_offset, button_posY1 });
    m_button_play->addCallback([this]() {
        m_waiting_for_skip = false;
        m_music->setLoop(true);
        m_music->setLoopPoints(sf::Music::TimeSpan(sf::Time(sf::milliseconds(m_loop_start)), sf::Time(sf::milliseconds(m_loop_end - m_loop_start))));
        m_music->play();
        m_music->setVolume(100);
    });
    m_button_play->setIcon(icon_play);
    add(m_button_play);

    auto icon_fade = std::make_shared<JamTemplate::SmartSprite>();
    icon_fade->loadSprite("assets/fade.png");
    m_button_fade = std::make_shared<JamTemplate::Button>(sf::Vector2u { button_width, button_height });
    m_button_fade->setPosition(sf::Vector2f { w / 4 - button_posX_offset, button_posY2 });
    m_button_fade->addCallback([this]() {
        auto tw = JamTemplate::TweenVolume<sf::Music>::create(m_music, 1.5f, 100.0f, 0.0f);
        add(tw);
    });
    m_button_fade->setIcon(icon_fade);
    add(m_button_fade);

    auto icon_stop = std::make_shared<JamTemplate::SmartSprite>();
    icon_stop->loadSprite("assets/stop.png");

    m_button_stop = std::make_shared<JamTemplate::Button>(sf::Vector2u { button_width, button_height });
    m_button_stop->setPosition(sf::Vector2f { w / 4 - button_posX_offset, button_posY3 });
    m_button_stop->addCallback([this]() { m_music->stop(); });
    m_button_stop->setIcon(icon_stop);
    add(m_button_stop);

    auto icon_skip = std::make_shared<JamTemplate::SmartSprite>();
    icon_skip->loadSprite("assets/skip.png");

    m_button_skip = std::make_shared<JamTemplate::Button>(sf::Vector2u { button_width, button_height });
    m_button_skip->setPosition(sf::Vector2f { w / 4 - button_posX_offset, button_posY4 });
    m_button_skip->addCallback([this]() {
        m_music->setLoop(false);
        // avoid end of music at loop_end
        m_music->setLoopPoints(sf::Music::TimeSpan(sf::Time(sf::milliseconds(0)), m_music->getDuration()));

        m_waiting_for_skip = true;
    });
    m_button_skip->setIcon(icon_skip);
    add(m_button_skip);

    m_button_play->update(0.0f);
    m_button_fade->update(0.0f);
    m_button_stop->update(0.0f);
    m_button_skip->update(0.0f);

    loadJson();

    if (m_audio_filepath == "") {
        throw std::invalid_argument("audio file path shall not be empty");
    }

    m_music = std::make_shared<sf::Music>();
    m_music->openFromFile(m_audio_filepath);
    m_music->setLoop(true);

    verifyParsedValues();

    m_playbar = std::make_shared<PlayBar>(m_music->getDuration().asMilliseconds(), m_loop_start, m_loop_end, m_skip_to, m_skip_from);
    add(m_playbar);
}

void StateMenu::loadJson()
{
    std::string filename = "assets/al_config.json";
    std::ifstream file { filename };
    if (!file) {
        std::cerr << "ERROR: cannot open file: " << filename << std::endl;
        std::terminate();
    }
    std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    rapidjson::Document json {};
    json.Parse(str.c_str());

    // json entries have to be checked here, because the json object
    // only exists in this function.
    if (!json.HasMember("audio_file_path")) {
        throw std::runtime_error("config file does not contain 'audio_file_path' entry");
    }
    if (!json.HasMember("loop_start")) {
        throw std::runtime_error("config file does not contain 'loop_start' entry");
    }
    if (!json.HasMember("loop_end")) {
        throw std::runtime_error("config file does not contain 'loop_end' entry");
    }
    if (!json.HasMember("skip_to_end")) {
        throw std::runtime_error("config file does not contain 'skip_to_end' entry");
    }
    if (!json.HasMember("skip_from")) {
        throw std::runtime_error("config file does not contain 'skip_from' entry");
    }

    m_loop_start = json["loop_start"].GetInt();
    m_loop_end = json["loop_end"].GetInt();
    m_audio_filepath = json["audio_file_path"].GetString();
    m_skip_to = json["skip_to_end"].GetInt();

    auto const arr = json["skip_from"].GetArray();
    for (auto i = 0U; i != arr.Size(); ++i) {
        m_skip_from.push_back(arr[i].GetInt());
    }
}

void StateMenu::verifyParsedValues()
{
    auto const duration = m_music->getDuration().asMilliseconds();
    if (m_loop_end < 0) {
        throw std::invalid_argument("loop end position shall be positive");
    }
    if (m_loop_end > duration) {
        throw std::invalid_argument("loop end position shall be smaller than music duration");
    }
    if (m_loop_start < 0) {
        throw std::invalid_argument("loop start position shall be positive");
    }
    if (m_loop_start > duration) {
        throw std::invalid_argument("loop start position shall be smaller than music duration");
    }
    if (m_skip_to < 0) {
        throw std::invalid_argument("skip to position shall be positive");
    }
    if (m_skip_to > duration) {
        throw std::invalid_argument("skip to position shall be smaller than music duration");
    }
    for (auto const v : m_skip_from) {
        if (v < 0) {
            throw std::invalid_argument("all skip from positions shall be positive");
        }
        if (v > duration) {
            throw std::invalid_argument("all skip from positions shall be smaller than music duration");
        }
    }
}

void StateMenu::doInternalDraw() const
{
    float w = static_cast<float>(getGame()->getRenderTarget()->getSize().x);
    float wC = w / 2;

    m_text_Credits->draw(getGame()->getRenderTarget());
    m_overlay->draw(getGame()->getRenderTarget());

    m_button_play->draw();
    m_button_fade->draw();
    m_button_stop->draw();
    m_button_skip->draw();
    m_playbar->draw();
}
