#include "glvis/abstract_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace glvis {

int AbstractTexture::getID() const {
    return ID;
}

int AbstractTexture::getWidth() const {
    return width;
}

int AbstractTexture::getHeight() const {
    return height;
}

void AbstractTexture::bind() const {
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
}

void AbstractTexture::unbind() const {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

Image AbstractTexture::readPixels() const {
    std::vector<unsigned char> data(width * height * 4);
    bind();
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
    unbind();
    return Image(width, height, std::move(data));
}

void AbstractTexture::resize(int newWidth, int newHeight) {
    resizeTexture(newWidth, newHeight);
}

AbstractTexture::~AbstractTexture() {
    GL_CALL(glDeleteTextures(1, &ID));
}

void AbstractTexture::createTexture(int width, int height, unsigned char* data) {
    START_TRY
    if (data && glfwGetCurrentContext() == nullptr) {
        throw std::runtime_error("Texture::create called outside of GLFW context");
    }
    GL_CALL(glGenTextures(1, &ID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    this->width = width;
    this->height = height;
    END_TRY
}

void AbstractTexture::resizeTexture(int newWidth, int newHeight) {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
    if (newWidth == width && newHeight == height) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        return;
    }
    // Read old data
    std::unique_ptr<unsigned char[]> oldData(new unsigned char[width * height * 4]);
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, oldData.get()));
    // Create new data with bilinear interpolation
    std::unique_ptr<unsigned char[]> newData(new unsigned char[newWidth * newHeight * 4]);
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            float srcX = (newWidth == 1) ? 0.0f : (x * (width - 1.0f) / (newWidth - 1.0f));
            float srcY = (newHeight == 1) ? 0.0f : (y * (height - 1.0f) / (newHeight - 1.0f));
            int x0 = static_cast<int>(floor(srcX));
            int y0 = static_cast<int>(floor(srcY));
            int x1 = std::min(x0 + 1, width - 1);
            int y1 = std::min(y0 + 1, height - 1);
            float factorX = srcX - x0;
            float factorY = srcY - y0;
            for (int channel = 0; channel < 4; channel++) {
                float v00 = oldData[(y0 * width + x0) * 4 + channel];
                float v01 = oldData[(y0 * width + x1) * 4 + channel];
                float v10 = oldData[(y1 * width + x0) * 4 + channel];
                float v11 = oldData[(y1 * width + x1) * 4 + channel];
                float v0 = v00 * (1.0f - factorX) + v01 * factorX;
                float v1 = v10 * (1.0f - factorX) + v11 * factorX;
                newData[(y * newWidth + x) * 4 + channel] = static_cast<unsigned char>(v0 * (1.0f - factorY) + v1 * factorY);
            }
        }
    }
    // Upload new data
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, newData.get()));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    this->width = newWidth;
    this->height = newHeight;
}

}
