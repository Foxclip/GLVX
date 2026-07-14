#pragma once

#include <filesystem>
#include "glvis/abstract_texture.h"

namespace glvis {

class Texture : public AbstractTexture {
public:
    Texture() = default;
    Texture(int width, int height, InterpolationType interp = InterpolationType::Nearest);
    Texture(unsigned char* data, int width, int height, int channels = 4, InterpolationType interp = InterpolationType::Nearest);
    Texture(const std::filesystem::path& path, InterpolationType interp = InterpolationType::Nearest);
    Image readPixels() const override;

};

}
