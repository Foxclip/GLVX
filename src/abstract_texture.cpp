#include "glvis/abstract_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"

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

    void AbstractTexture::bind() {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
    }

    void AbstractTexture::unbind() {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void AbstractTexture::resize(int newWidth, int newHeight) {
        resizeTexture(newWidth, newHeight);
    }

    AbstractTexture::~AbstractTexture() {
        GL_CALL(glDeleteTextures(1, &ID));
    }

    void AbstractTexture::createEmptyTexture(int width, int height) {
        GL_CALL(glGenTextures(1, &ID));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void AbstractTexture::resizeTexture(int newWidth, int newHeight) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
}
