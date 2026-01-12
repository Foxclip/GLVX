#include <iostream>
#include "glvis/app.h"

int main() {

    try {

        glvis::App app(800, 600);

        app.getCamera().setPosition(glm::vec2(400.0f, 300.0f));

        glvis::Texture* boxTexture = app.addTexture("box.jpg");

        glvis::Rectangle* rect1 = app.addRectangle(100.0f, 100.0f);
        rect1->setTexture(boxTexture);
        rect1->setOrigin(50.0f, 50.0f);
        rect1->setPosition(0.0f, 0.0f);

        glvis::Rectangle* rect2 = app.addRectangle(100.0f, 100.0f);
        rect2->setTexture(boxTexture);
        rect2->setPosition(200.0f, 0.0f);
        rect2->setRotation(glvis::degrees(10.0f));

        glvis::Circle* circle1 = app.addCircle(50.0f, 30);
        circle1->setOrigin(50.0f, 50.0f);
        circle1->setPosition(400.0f, 0.0f);
        circle1->setTexture(boxTexture);

        glvis::VertexArray* va = app.addVertexArray(glvis::PrimitiveType::Triangles, 0);
        va->append(glvis::Vertex(glvis::Vector2(0.0f, 200.0f),   glvis::Color::White, glvis::Vector2(0.0f, 0.0f)));
        va->append(glvis::Vertex(glvis::Vector2(100.0f, 200.0f), glvis::Color::White, glvis::Vector2(1.0f, 0.0f)));
        va->append(glvis::Vertex(glvis::Vector2(100.0f, 300.0f), glvis::Color::White, glvis::Vector2(1.0f, 1.0f)));
        va->append(glvis::Vertex(glvis::Vector2(0.0f, 200.0f),   glvis::Color::White, glvis::Vector2(0.0f, 0.0f)));
        va->append(glvis::Vertex(glvis::Vector2(100.0f, 300.0f), glvis::Color::White, glvis::Vector2(1.0f, 1.0f)));
        va->append(glvis::Vertex(glvis::Vector2(0.0f, 300.0f),   glvis::Color::White, glvis::Vector2(0.0f, 1.0f)));
        va->setTexture(boxTexture);

        app.start();

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    } catch (std::string& e) {
        std::cerr << "ERROR: " << e << std::endl;
    }

    return 0;
    
    // TODO: add some preset colors in Color class
    // TODO: render all Drawable objects in App
    // TODO: add tests
    // TODO: text rendering
    // TODO: transparent texture rendering
}
