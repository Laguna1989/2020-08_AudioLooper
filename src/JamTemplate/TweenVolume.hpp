#ifndef JAMTEMPLATE_TWEENVOLUME_HPP_INCLUDEGUARD
#define JAMTEMPLATE_TWEENVOLUME_HPP_INCLUDEGUARD

#include "Lerp.hpp"
#include "TweenBase.hpp"

namespace JamTemplate {
template <class T>
class TweenVolume : public Tween<T> {
public:
    // Tween alpha value from valueStart to valueEnd of obj withtin time
    static TweenBase::Sptr create(std::weak_ptr<T> obj, float time, float valueStart, float valueEnd)
    {
        return std::make_shared<TweenVolume>(obj, time, valueStart, valueEnd);
    }

    // Tween volume value from valueStart to valueEnd of obj withtin time
    TweenVolume(std::weak_ptr<T> obj, float time, float valueStart, float valueEnd)
        : Tween<T> { obj, [this](auto sptr, auto age) {
                        float volume = Lerp::linear(m_initialValue, m_finalValue, age / m_totalTime);

                        sptr->setVolume(volume);

                        return (age < m_totalTime);
                    } }
    {
        m_totalTime = time;
        m_initialValue = static_cast<float>(valueStart);
        m_finalValue = static_cast<float>(valueEnd);
    }

private:
    float m_totalTime { 1.0f };
    float m_initialValue { 0.0f };
    float m_finalValue { 0.0f };
};

} // namespace JamTemplate

#endif
