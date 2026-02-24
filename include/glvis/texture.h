#pragma once

#include <filesystem>
#include "glvis/abstract_texture.h"

namespace glvis {

class Texture : public AbstractTexture {
public:
    Texture() = default;
    Texture(int width, int height);
    Texture(unsigned char* data, int width, int height, int channels = 4);
    Texture(const std::filesystem::path& path);
    const std::filesystem::path& getPath() const;

private:
    std::filesystem::path path;

};

}
