#include "animation.hpp"
#include <fstream>
#include <iostream>
#include <math_helper.hpp>
#include <memory>
#include <sprite.hpp>
#include <strutils.hpp>
#include <system_helper.hpp>
#include <texture_manager_interface.hpp>
#include <vector>

namespace {

std::shared_ptr<jt::Sprite>& getCurrentSprite(
    jt::Animation::AnimationMapType& frames, std::string const& animName, size_t const animIndex)
{
    auto const cit = frames.find(animName);
    if (cit == frames.cend()) {
        throw std::invalid_argument { "AnimName: '" + animName + "' not part of animation" };
    }

    return cit->second.at(animIndex);
}

} // namespace

void jt::Animation::add(std::string const& fileName, std::string const& animName,
    jt::Vector2u const& imageSize, std::vector<unsigned int> const& frameIndices,
    float frameTimeInSeconds, TextureManagerInterface& textureManager)
{
    if (frameTimeInSeconds <= 0) {
        throw std::invalid_argument { "animation frame time is negative or zero." };
    }
    std::vector<float> frameTimes;
    frameTimes.resize(frameIndices.size(), frameTimeInSeconds);
    add(fileName, animName, imageSize, frameIndices, frameTimes, textureManager);
}

void jt::Animation::add(std::string const& fileName, std::string const& animName,
    jt::Vector2u const& imageSize, std::vector<unsigned int> const& frameIndices,
    std::vector<float> const& frameTimesInSeconds, jt::TextureManagerInterface& textureManager)
{
    if (frameIndices.empty()) {
        throw std::invalid_argument { "animation frame indices are empty." };
    }
    if (animName.empty()) {
        throw std::invalid_argument { "animation name is empty." };
    }
    if (frameTimesInSeconds.empty()) {
        throw std::invalid_argument { "frametimes are empty." };
    }

    if (frameTimesInSeconds.size() != frameIndices.size()) {
        throw std::invalid_argument { "different sizes for frametimes and frame indices" };
    }

    if (m_frames.contains(animName)) {
        std::cout << "Warning: Overwriting old animation with name: " << animName << std::endl;
    }

    m_frames[animName] = std::vector<jt::Sprite::Sptr> {};

    for (auto const idx : frameIndices) {
        jt::Recti const rect { static_cast<int>(idx * imageSize.x), 0,
            static_cast<int>(imageSize.x), static_cast<int>(imageSize.y) };
        auto sptr = std::make_shared<Sprite>(fileName, rect, textureManager);
        m_frames[animName].push_back(sptr);
    }
    m_time[animName] = frameTimesInSeconds;
    m_isLooping[animName] = true;
}

void jt::Animation::loadFromJson(
    std::string const& jsonFileName, TextureManagerInterface& textureManager)
{
}

void jt::Animation::loadFromAseprite(
    std::string const& asepriteFileName, jt::TextureManagerInterface& textureManager)
{
}

bool jt::Animation::hasAnimation(std::string const& animationName) const
{
    return (m_frames.contains(animationName));
}

std::vector<std::string> jt::Animation::getAllAvailableAnimationNames() const
{
    std::vector<std::string> names;
    names.resize(m_frames.size());
    std::transform(m_frames.cbegin(), m_frames.cend(), names.begin(),
        [](auto kvp) -> std::string { return kvp.first; });

    return names;
}

std::string jt::Animation::getRandomAnimationName() const
{
    if (m_frames.empty()) {
        throw std::invalid_argument {
            "can not get random animation name if no animation has been added"
        };
    }
    return jt::SystemHelper::select_randomly(m_frames).first;
}

void jt::Animation::play(std::string const& animationName, size_t startFrameIndex, bool restart)
{
    m_isValid = hasAnimation(animationName);
    if (!m_isValid) {
        std::cout << "Warning: Play Animation with invalid animName: '" + m_currentAnimName + "'\n";
        return;
    }

    if (m_currentAnimName != animationName || restart) {
        m_currentIdx = startFrameIndex;
        m_currentAnimName = animationName;
        m_frameTime = 0;
    }
}

void jt::Animation::setColor(jt::Color const& col)
{
    for (auto& kvp : m_frames) {
        for (auto& spr : kvp.second) {
            spr->setColor(col);
        }
    }
}

jt::Color jt::Animation::getColor() const
{
    return getCurrentSprite(m_frames, m_currentAnimName, m_currentIdx)->getColor();
}

void jt::Animation::setPosition(jt::Vector2f const& pos) { m_position = pos; }

jt::Vector2f jt::Animation::getPosition() const { return m_position; }

jt::Rectf jt::Animation::getGlobalBounds() const
{
    return getCurrentSprite(m_frames, m_currentAnimName, m_currentIdx)->getGlobalBounds();
}

jt::Rectf jt::Animation::getLocalBounds() const
{
    return getCurrentSprite(m_frames, m_currentAnimName, m_currentIdx)->getLocalBounds();
}

void jt::Animation::setScale(jt::Vector2f const& scale)
{
    for (auto& kvp : m_frames) {
        for (auto& spr : kvp.second) {
            spr->setScale(scale);
        }
    }
}

jt::Vector2f jt::Animation::getScale() const
{
    return getCurrentSprite(m_frames, m_currentAnimName, m_currentIdx)->getScale();
}

void jt::Animation::setOriginInternal(jt::Vector2f const& origin)
{
    for (auto const& kvp : m_frames) {
        for (auto const& sptr : kvp.second) {
            sptr->setOrigin(origin);
        }
    }
}

void jt::Animation::setOutline(jt::Color const& color, int width)
{
    DrawableImpl::setOutline(color, width);
    for (auto const& kvp : m_frames) {
        for (auto const& sprite : kvp.second) {
            sprite->setOutline(color, width);
        }
    }
}

void jt::Animation::setShadow(jt::Color const& color, jt::Vector2f const& offset)
{
    DrawableImpl::setShadow(color, offset);
    for (auto const& kvp : m_frames) {
        for (auto const& sprite : kvp.second) {
            sprite->setShadow(color, offset);
        }
    }
}

void jt::Animation::setShadowActive(bool active)
{
    DrawableImpl::setShadowActive(active);
    for (auto const& kvp : m_frames) {
        for (auto const& sprite : kvp.second) {
            sprite->setShadowActive(active);
        }
    }
}

void jt::Animation::doDrawShadow(std::shared_ptr<jt::RenderTargetLayer> const /*sptr*/) const { }

void jt::Animation::doDrawOutline(std::shared_ptr<jt::RenderTargetLayer> const /*sptr*/) const { }

void jt::Animation::doDraw(std::shared_ptr<jt::RenderTargetLayer> const sptr) const
{
    if (!m_isValid) {
        std::cerr << "Warning: Drawing Animation with invalid animName: '" + m_currentAnimName
                + "'\n";
        return;
    }
    m_frames.at(m_currentAnimName).at(m_currentIdx)->setBlendMode(getBlendMode());
    m_frames.at(m_currentAnimName).at(m_currentIdx)->draw(sptr);
}

void jt::Animation::doDrawFlash(std::shared_ptr<jt::RenderTargetLayer> const /*sptr*/) const { }

void jt::Animation::doFlashImpl(float t, jt::Color col)
{
    for (auto& kvp : m_frames) {
        for (auto& sprite : kvp.second) {
            sprite->flash(t, col);
        }
    }
}

void jt::Animation::doUpdate(float elapsed)
{
    if (!m_isValid) {
        std::cout << "Warning: Update Animation with invalid animName: '" + m_currentAnimName
                + "'\n";
        return;
    }

    // proceed time
    m_frameTime += elapsed * m_animationplaybackSpeed;

    auto const frame_time = m_time[m_currentAnimName][m_currentIdx];
    // increase index
    while (m_frameTime >= frame_time) {
        m_frameTime -= frame_time;
        m_currentIdx++;
    }
    // wrap index or fix index at last frame
    if (m_currentIdx >= m_frames.at(m_currentAnimName).size()) {
        if (getCurrentAnimationIsLooping()) {
            m_currentIdx = 0;
        } else {
            m_currentIdx = m_frames.at(m_currentAnimName).size() - 1;
        }
    }

    // update values for current sprite
    auto const& currentSprite = m_frames.at(m_currentAnimName).at(m_currentIdx);
    currentSprite->setPosition(m_position + getShakeOffset() + getOffset());
    currentSprite->setIgnoreCamMovement(DrawableImpl::getIgnoreCamMovement());

    // update all sprites
    for (auto& kvp : m_frames) {
        for (auto& sprite : kvp.second) {
            sprite->update(elapsed);
        }
    }
}

void jt::Animation::doRotate(float rot)
{
    for (auto& kvp : m_frames) {
        for (auto& sprite : kvp.second) {
            sprite->setRotation(rot);
        }
    }
}

float jt::Animation::getCurrentAnimationSingleFrameTime() const
{
    return m_time.at(m_currentAnimName).at(m_currentIdx);
}

float jt::Animation::getCurrentAnimTotalTime() const
{
    float sum = 0.0f;
    for (auto frameTime : m_time.at(m_currentAnimName)) {
        sum += frameTime;
    }
    return sum;
}

float jt::Animation::getAnimTotalTimeFor(std::string const& animName)
{
    if (!hasAnimation(animName)) {
        throw std::invalid_argument { "no animation with name " + animName };
    }
    float sum = 0.0f;
    for (auto frameTime : m_time.at(animName)) {
        sum += frameTime;
    }
    return sum;
}

std::size_t jt::Animation::getNumberOfFramesInCurrentAnimation() const
{
    return m_frames.at(m_currentAnimName).size();
}

std::string jt::Animation::getCurrentAnimationName() const { return m_currentAnimName; }

bool jt::Animation::getCurrentAnimationIsLooping() const
{
    if (!hasAnimation(m_currentAnimName)) {
        return true;
    }
    return m_isLooping.at(m_currentAnimName);
}

bool jt::Animation::getIsLoopingFor(std::string const& animName) const
{
    if (!hasAnimation(animName)) {
        throw std::invalid_argument { "no animation with name " + animName };
    }
    return m_isLooping.at(animName);
}

void jt::Animation::setLooping(std::string const& animName, bool isLooping)
{
    if (!hasAnimation(animName)) {
        throw std::invalid_argument { "invalid animation name: " + animName };
    }
    m_isLooping[animName] = isLooping;
}

void jt::Animation::setLoopingAll(bool isLooping)
{
    for (auto& l : m_isLooping) {
        l.second = isLooping;
    }
}

std::size_t jt::Animation::getCurrentAnimationFrameIndex() const { return m_currentIdx; }

void jt::Animation::setFrameTimes(
    std::string const& animationName, std::vector<float> const& frameTimes)
{
    if (!m_frames.contains(animationName)) {
        throw std::invalid_argument { "cannot set frame times for invalid animation: "
            + animationName };
    }
    if (frameTimes.size() != m_frames[animationName].size()) {
        throw std::invalid_argument { "frame times size does not match frame index size" };
    }
    m_time[animationName] = frameTimes;
}

void jt::Animation::setAnimationSpeedFactor(float factor) { m_animationplaybackSpeed = factor; }

float jt::Animation::getAnimationSpeedFactor() const { return m_animationplaybackSpeed; }
