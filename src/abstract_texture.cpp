#include "glvis/abstract_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/utils.h"
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
    std::unique_ptr<unsigned char[]> oldData(new unsigned char[width * height * 4]);
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, oldData.get()));
    std::unique_ptr<unsigned char[]> newData = bilinearInterpolate(
        oldData.get(), width, height, newWidth, newHeight
    );
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, newData.get()));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    this->width = newWidth;
    this->height = newHeight;
}

}
