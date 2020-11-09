#include "PlayBar.hpp"
#include "JamTemplate/Game.hpp"

PlayBar::PlayBar(std::int32_t duration, std::int32_t loop_start, std::int32_t loop_end, std::int32_t skip_to, std::vector<std::int32_t> const skip_from)
    : m_duration { duration }
    , m_loop_start { loop_start }
    , m_loop_end { loop_end }
    , m_skip { skip_to }
    , m_skip_from { skip_from }
    , m_position { 0 }
    , m_width { 64 }
{
}

void PlayBar::doCreate()
{
    float height = 20;
    float pos_offsetX = 16;
    float pos_offsetY = 180;

    m_shape_full = sf::RectangleShape(sf::Vector2f { m_width, height });
    m_shape_full.setFillColor(sf::Color { 150, 150, 150 });
    m_shape_full.setPosition(pos_offsetX, pos_offsetY);

    m_shape_progress
        = sf::RectangleShape(sf::Vector2f { m_width, height });
    m_shape_progress.setFillColor(sf::Color { 255, 255, 255 });
    m_shape_progress.setPosition(pos_offsetX, pos_offsetY);

    float relpos_loop_start = (float)m_loop_start / (float)m_duration * m_width;
    m_shape_loop_start = sf::RectangleShape(sf::Vector2f { 1, height });
    m_shape_loop_start.setFillColor(sf::Color { 100, 255, 100 });
    m_shape_loop_start.setPosition(relpos_loop_start + pos_offsetX, pos_offsetY);
    m_shape_loop_start.setScale(1, 0.5);

    float relpos_loop_end = (float)m_loop_end / (float)m_duration * m_width;
    m_shape_loop_end = sf::RectangleShape(sf::Vector2f { 1, height });
    m_shape_loop_end.setFillColor(sf::Color { 10, 255, 10 });
    m_shape_loop_end.setPosition(relpos_loop_end + pos_offsetX, pos_offsetY);
    m_shape_loop_end.setScale(1, 0.5);

    float relpos_skip = (float)m_skip / (float)m_duration * m_width;
    m_shape_skip = sf::RectangleShape(sf::Vector2f { 2, height });
    m_shape_skip.setFillColor(sf::Color { 0, 0, 255 });
    m_shape_skip.setPosition(relpos_skip + pos_offsetX, pos_offsetY);

    m_shape_skip_from = sf::RectangleShape(sf::Vector2f { 1, height * 0.3f });
    m_shape_skip_from.setFillColor(sf::Color(100, 100, 255));
    m_shape_skip_from.setPosition(sf::Vector2f(0, pos_offsetY + 0.7f * height));
}

void PlayBar::doUpdate(float const /*elapsed*/)
{
    float value = static_cast<float>(m_position) / m_duration;
    m_shape_progress.setScale(value, 1);
}

void PlayBar::doDraw() const
{
    getGame()->getRenderTarget()->draw(m_shape_full);
    getGame()->getRenderTarget()->draw(m_shape_progress);
    getGame()->getRenderTarget()->draw(m_shape_skip);
    getGame()->getRenderTarget()->draw(m_shape_loop_start);
    getGame()->getRenderTarget()->draw(m_shape_loop_end);

    for (auto const v : m_skip_from) {
        float relpos = static_cast<float>(v) / m_duration;
        m_shape_skip_from.setPosition(sf::Vector2f(16 + relpos * m_width, m_shape_skip_from.getPosition().y));
        getGame()->getRenderTarget()->draw(m_shape_skip_from);
    }
}
