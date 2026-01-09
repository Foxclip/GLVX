#include "glvis/texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "glvis/stb_image.h"
#include "glvis/glvis_common.h"

namespace glvis {

    Texture::Texture(int width, int height) {
        createEmptyTexture(width, height);
    }

    Texture::Texture(const std::filesystem::path &path) {
        START_TRY
            GL_CALL(glGenTextures(1, &ID));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
            int width, height, nrChannels;
            unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
                GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
            } else {
                throw std::runtime_error("Failed to load texture: " + path.string() + "\n");;
            }
            stbi_image_free(data);
            GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        END_TRY
    }

    const std::filesystem::path& Texture::getPath() const {
        return path;
    }
}
