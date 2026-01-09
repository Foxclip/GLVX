#pragma once

namespace glvis {

    class AbstractTexture {
    public:
        int getID() const;
        int getWidth() const;
        int getHeight() const;
        void bind();
        void unbind();
        virtual void resize(int newWidth, int newHeight);
        ~AbstractTexture();

    protected:
        unsigned int ID = 0;
        int width = 0;
        int height = 0;
        void createEmptyTexture(int width, int height);
        void resizeTexture(int newWidth, int newHeight);

    };

}
