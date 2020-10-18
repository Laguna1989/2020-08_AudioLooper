#ifndef GAME_PLAYBAR_HPP_INCLUDEGUARD
#define GAME_PLAYBAR_HPP_INCLUDEGUARD

#include "JamTemplate/GameObject.hpp"
#include <vector>

class PlayBar : public JamTemplate::GameObject {
public:
    virtual void doUpdate(float const /*elapsed*/) override;
    virtual void doDraw() const override;
    virtual void doCreate() override;

    PlayBar(std::int32_t duration, std::int32_t loop_start, std::int32_t loop_end, std::int32_t skip_to, std::vector<std::int32_t> const skip_from);

    void setPosition(std::int32_t position)
    {
        m_position = position;
    }

private:
    std::int32_t m_duration;
    std::int32_t m_skip;
    std::vector<std::int32_t> m_skip_from;
    std::int32_t m_position;
    std::int32_t m_loop_start;
    std::int32_t m_loop_end;

    float const m_width;

    sf::RectangleShape m_shape_full;
    sf::RectangleShape m_shape_progress;
    sf::RectangleShape m_shape_loop_start;
    sf::RectangleShape m_shape_loop_end;
    sf::RectangleShape m_shape_skip;
    mutable sf::RectangleShape m_shape_skip_from;
};

#endif
