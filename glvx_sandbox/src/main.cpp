#include "glvx/window.h"
#include "glvx/rectangle.h"
#include "glvx/view.h"
#include "glvx/color.h"

glvx::Window window;
glvx::Rectangle rectangle_red(20.0f, 20.0f);
glvx::Rectangle rectangle_green(20.0f, 20.0f);
glvx::Rectangle rectangle_blue(20.0f, 20.0f);
glvx::View view;

void render() {
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(glvx::Color::Black);

    window.draw(rectangle_red);
    window.draw(rectangle_green);
    window.draw(rectangle_blue);

    window.display();
}

int main() {
    window.create(800, 600, "GLVX sandbox");

    rectangle_red.setColor(glvx::Color::Red);
    rectangle_green.setColor(glvx::Color::Green);
    rectangle_blue.setColor(glvx::Color::Blue);
    rectangle_red.setPosition(10.0f, 10.0f);
    rectangle_green.setPosition(40.0f, 10.0f);
    rectangle_blue.setPosition(70.0f, 10.0f);

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

        render();
    }

    return 0;
}
