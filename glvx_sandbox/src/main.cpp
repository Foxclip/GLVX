#include "glvx/window.h"
#include "glvx/rectangle.h"
#include "glvx/view.h"
#include "glvx/color.h"

int main() {
    glvx::Window window;
    window.create(800, 600, "GLVX sandbox");

    glvx::View view;
    view.setPosition(window.getCenter());

    glvx::Rectangle rectangle(100.0f, 100.0f);
    rectangle.setColor(glvx::Color::Red);

    while (window.isOpen()) {
        glvx::Event event;
        while (window.pollEvent(event)) {
            if (event.type == glvx::EventType::Closed) {
                window.close();
            }
            if (event.type == glvx::EventType::Resized) {
                view.setPosition(window.getCenter());
            }
        }
        window.setView(view);
        window.clear(glvx::Color::Black);
        window.draw(rectangle);
        window.display();
    }

    return 0;
}
