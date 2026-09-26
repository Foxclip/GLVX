#include "glvx/window.h"
#include "glvx/rectangle.h"
#include "glvx/circle.h"
#include "glvx/view.h"
#include "glvx/color.h"

int main() {
    glvx::Window window;
    glvx::Rectangle rectangle_red(20.0f, 20.0f);
    glvx::Rectangle rectangle_green(20.0f, 20.0f);
    glvx::Rectangle rectangle_blue(20.0f, 20.0f);
    glvx::Rectangle rectangle_red_transparent(20.0f, 20.0f);
    glvx::Circle circle(10.0f);
    glvx::View view;

    window.create(800, 600, "GLVX sandbox");

    rectangle_red.setColor(glvx::Color::Red);
    rectangle_green.setColor(glvx::Color::Green);
    rectangle_blue.setColor(glvx::Color::Blue);
    rectangle_red.setPosition(10.0f, 10.0f);
    rectangle_green.setPosition(40.0f, 10.0f);
    rectangle_blue.setPosition(70.0f, 10.0f);

    rectangle_red_transparent.setColor(glvx::Color(255, 0, 0, 128));
    rectangle_red_transparent.setPosition(10.0f, 40.0f);

    circle.setColor(glvx::Color::Yellow);
    circle.setPosition(10.0f, 70.0f);

    auto render = [&]() {
        view.setPosition(window.getCenter());
        window.setView(view);
        window.clear(glvx::Color::Black);

        window.draw(rectangle_red);
        window.draw(rectangle_green);
        window.draw(rectangle_blue);

        window.draw(rectangle_red_transparent);

        window.draw(circle);

        window.display();
    };

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
