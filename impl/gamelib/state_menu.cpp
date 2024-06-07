#include "state_menu.hpp"
#include "game.hpp"
#include "game_properties.hpp"
#include "shape.hpp"
#include "text.hpp"
#include "texture_manager_interface.hpp"
#include "tweens/tween_alpha.hpp"
#include <exception>
#include <fstream>

void StateMenu::onCreate()
{
    float w = static_cast<float>(GP::GetScreenSize().x);
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

    auto icon_play = std::make_shared<jt::Sprite>("assets/play.png", textureManager());

    m_button_play
        = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_play->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY1 });
    m_button_play->addCallback([this]() {
        // TODO
    });
    m_button_play->setDrawable(icon_play);
    add(m_button_play);

    auto icon_fade = std::make_shared<jt::Sprite>("assets/fade.png", textureManager());
    m_button_fade = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_fade->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY2 });
    m_button_fade->addCallback([this]() {
        // TODO
    });
    m_button_fade->setDrawable(icon_fade);
    add(m_button_fade);

    auto icon_stop = std::make_shared<jt::Sprite>("assets/stop.png", textureManager());

    m_button_stop = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_stop->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY3 });
    m_button_stop->addCallback([this]() {
        // TODO
    });
    m_button_stop->setDrawable(icon_stop);
    add(m_button_stop);

    auto icon_skip = std::make_shared<jt::Sprite>("assets/skip.png", textureManager());

    m_button_skip = std::make_shared<jt::Button>(jt::Vector2u { button_width, button_height }, textureManager());
    m_button_skip->setPosition(jt::Vector2f { w / 4 - button_posX_offset, button_posY4 });
    m_button_skip->addCallback([this]() {
        // TODO
    });
    m_button_skip->setDrawable(icon_skip);
    add(m_button_skip);

    m_button_play->update(0.0f);
    m_button_fade->update(0.0f);
    m_button_stop->update(0.0f);
    m_button_skip->update(0.0f);
}

void StateMenu::onEnter()
{
}
void StateMenu::onUpdate(float const elapsed)
{
    // TODO
}
void StateMenu::onDraw() const
{
    m_text_Credits->draw(renderTarget());
    m_overlay->draw(renderTarget());

    m_button_play->draw();
    m_button_fade->draw();
    m_button_stop->draw();
    m_button_skip->draw();
}
