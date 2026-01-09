#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace glvis {
    
    const float CAMERA_ZOOM_FACTOR = 1.2f;
    
    class Camera {
        public:
        const glm::vec2& getPosition() const;
        float getZoom() const;
        void setPosition(const glm::vec2& pos);
        void setZoom(float zoom);
        
        private:
        glm::vec2 pos = glm::vec2(0.0f, 0.0f);
        float zoom = 1.0f;
    };

}
