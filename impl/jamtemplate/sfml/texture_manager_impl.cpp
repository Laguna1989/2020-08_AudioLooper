#include "texture_manager_impl.hpp"
#include <array>
#include <color_lib.hpp>
#include <sprite_functions.hpp>
#include <stdexcept>
#include <strutils.hpp>

namespace {

sf::Image createButtonImage(std::array<std::string, 3> const& ssv)
{
    std::size_t count { 0 };
    std::int64_t w = std::stol(ssv.at(1), &count);
    if (count != ssv.at(1).size()) {
        throw std::invalid_argument { "invalid button size string" };
    }
    std::int64_t h = std::stol(ssv.at(2), &count);
    if (count != ssv.at(2).size()) {
        throw std::invalid_argument { "invalid button size string" };
    }
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument { "invalid button size" };
    }

    return jt::SpriteFunctions::makeButtonImage(
        static_cast<unsigned int>(w), static_cast<unsigned int>(h));
}

sf::Image createBlankImage(std::array<std::string, 3> const& ssv)
{
    std::size_t count { 0 };
    std::int64_t w = std::stol(ssv.at(1), &count);
    if (count != ssv.at(1).size()) {
        throw std::invalid_argument { "invalid image size string" };
    }
    std::int64_t h = std::stol(ssv.at(2), &count);
    if (count != ssv.at(2).size()) {
        throw std::invalid_argument { "invalid image size string" };
    }
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument { "invalid image size" };
    }

    return jt::SpriteFunctions::makeBlankImage(
        static_cast<unsigned int>(w), static_cast<unsigned int>(h));
}

sf::Image createGlowImage(std::array<std::string, 3> const& ssv)
{
    std::size_t count { 0 };
    auto const s = std::stol(ssv.at(1), &count);
    if (count != ssv.at(1).size() || s <= 0) {
        throw std::invalid_argument { "invalid glow size" };
    }
    auto const max = std::stol(ssv.at(2), &count);
    if (count != ssv.at(2).size() || max <= 0 || max > 255) {
        throw std::invalid_argument { "invalid glowmax" };
    }
    return jt::SpriteFunctions::makeGlowImage(static_cast<float>(s), static_cast<uint8_t>(max));
}

sf::Image createVignetteImage(std::array<std::string, 3> const& ssv)
{
    std::size_t count { 0 };
    auto w = std::stol(ssv.at(1), &count);
    if (count != ssv.at(1).size() || w <= 0) {
        throw std::invalid_argument { "invalid vignette w" };
    }
    auto h = std::stol(ssv.at(2), &count);
    if (count != ssv.at(2).size() || h <= 0) {
        throw std::invalid_argument { "invalid vignette h" };
    }
    return jt::SpriteFunctions::makeVignetteImage(w, h);
}

sf::Image createRingImage(std::array<std::string, 2> const& ssv)
{
    std::size_t count { 0 };
    auto const ringImageSize = std::stol(ssv.at(1), &count);
    if (count != ssv.at(1).size() || ringImageSize <= 0) {
        throw std::invalid_argument { "invalid ring image size" };
    }

    return jt::SpriteFunctions::makeRing(ringImageSize);
}

sf::Image createFlashImage(sf::Image const& in)
{
    sf::Image img { in };

    for (unsigned int i = 0; i != img.getSize().x; ++i) {
        for (unsigned int j = 0; j != img.getSize().y; ++j) {
            if (img.getPixel(i, j).a != 0) {
                img.setPixel(i, j, toLib(jt::Color { 255, 255, 255 }));
            }
        }
    }
    return img;
}

sf::Texture loadTextureFromDisk(std::string const& str)
{
    sf::Texture t {};
    if (!t.loadFromFile(str)) {
        throw std::invalid_argument { "invalid filename, cannot load texture from '" + str + "'" };
    }
    return t;
}
} // namespace

jt::TextureManagerImpl::TextureManagerImpl(std::shared_ptr<jt::RenderTargetLayer> /*renderer*/)
{
    // Nothing to do here
}

sf::Texture& jt::TextureManagerImpl::get(std::string const& str)
{
    if (str.empty()) {
        throw std::invalid_argument { "TextureManager get: string must not be empty" };
    }

    // check if texture is already stored in texture manager
    if (containsTexture(str)) {
        return m_textures[str];
    }
    
    // normal filenames do not start with a '#'
    if (!str.starts_with('#')) {
        m_textures[str] = loadTextureFromDisk(str);
        m_textures[getFlashName(str)].loadFromImage(
            createFlashImage(m_textures[str].copyToImage()));
        return m_textures[str];
    }

    // special type of images
    if (str.at(1) == 'b') {
        auto const ssv = strutil::split<3>(str.substr(1), '#');
        m_textures[str].loadFromImage(createButtonImage(ssv));
    } else if (str.at(1) == 'f') {
        auto const ssv = strutil::split<3>(str.substr(1), '#');
        m_textures[str].loadFromImage(createBlankImage(ssv));
    } else if (str.at(1) == 'g') {
        auto const ssv = strutil::split<3>(str.substr(1), '#');
        m_textures[str].loadFromImage(createGlowImage(ssv));
    } else if (str.at(1) == 'v') {
        auto const ssv = strutil::split<3>(str.substr(1), '#');
        m_textures[str].loadFromImage(createVignetteImage(ssv));
    } else if (str.at(1) == 'r') {
        auto const ssv = strutil::split<2>(str.substr(1), '#');
        m_textures[str].loadFromImage(createRingImage(ssv));
    } else {
        throw std::invalid_argument("ERROR: cannot get texture with name " + str);
    }

    // create Flash Image
    m_textures[getFlashName(str)].loadFromImage(createFlashImage(m_textures[str].copyToImage()));

    return m_textures[str];
}

void jt::TextureManagerImpl::reset() { m_textures.clear(); }

std::string jt::TextureManagerImpl::getFlashName(std::string const& str)
{
    return str + "___flash__";
}

std::size_t jt::TextureManagerImpl::getNumberOfTextures() noexcept { return m_textures.size(); }

bool jt::TextureManagerImpl::containsTexture(std::string const& str) const
{
    return (m_textures.contains(str));
}
