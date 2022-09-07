#include "Driver.hpp"

Driver::Driver()
    : Driver{ 400, 300, 3 }
{
}

Driver::Driver(int width, int height, int scale)
    : width{ width }, height{ height }, scale{ scale },
      windowWidth{ width * scale }, windowHeight{ height * scale }
{
}

void Driver::start()
{
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "3D Software Renderer", sf::Style::Close);
    window.setMouseCursorVisible(false);

    Raster raster{ width, height };
    sf::Texture texture;
    texture.create(width, height);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(scale, scale);

    Renderer renderer{ &raster };

    Mesh bricks = Mesh::loadFromFile("bricks.obj", Mesh::Shading::KEEP_NORMALS);
    Raster bricksTex{ 728, 473 };

    sf::Image image;
    image.loadFromFile("bricks.jpg");
    bricksTex.loadFromBuffer(image.getPixelsPtr());

    Camera camera(false, 1.57, width / (double) height, 0.1, Vector3{ 0.0, 0.0, 5.0 });

    std::vector<LightSource> lights;
    lights.push_back(LightSource{ AmbientLight{ Vector3{ 0.4, 0.4, 0.5 } } });
    lights.push_back(LightSource{ DirectionalLight{ Vector3{ 0.6, 0.6, 0.8 }, Vector3{ 1.0, -1.0, -1.0 } } });

    sf::Clock timer;
    sf::Time lastTime = timer.getElapsedTime();

    bool cursorFree = true;
    bool wDown = false;
    bool aDown = false;
    bool sDown = false;
    bool dDown = false;
    bool qDown = false;
    bool eDown = false;
    double dmx;
    double dmy;
    sf::Mouse::setPosition(sf::Vector2i{ windowWidth / 2, windowHeight / 2 }, window);

    sf::Time fpsTimer;
    int frames = 0;

    while (window.isOpen())
    {
        sf::Time now = timer.getElapsedTime();
        sf::Time delta = now - lastTime;
        lastTime = now;


        frames++;
        fpsTimer += delta;
        while (fpsTimer.asSeconds() >= 1.0)
        {
            std::cout << "FPS: " << (int) (frames / fpsTimer.asSeconds()) << std::endl;
            frames = 0;
            fpsTimer = sf::Time::Zero;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W)
                    wDown = true;
                if (event.key.code == sf::Keyboard::A)
                    aDown = true;
                if (event.key.code == sf::Keyboard::S)
                    sDown = true;
                if (event.key.code == sf::Keyboard::D)
                    dDown = true;
                if (event.key.code == sf::Keyboard::Q)
                    qDown = true;
                if (event.key.code == sf::Keyboard::E)
                    eDown = true;
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::W)
                    wDown = false;
                if (event.key.code == sf::Keyboard::A)
                    aDown = false;
                if (event.key.code == sf::Keyboard::S)
                    sDown = false;
                if (event.key.code == sf::Keyboard::D)
                    dDown = false;
                if (event.key.code == sf::Keyboard::Q)
                    qDown = false;
                if (event.key.code == sf::Keyboard::E)
                    eDown = false;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                cursorFree = !cursorFree;
                if (!cursorFree)
                    sf::Mouse::setPosition(sf::Vector2i{ windowWidth / 2, windowHeight / 2 }, window);
            }
        }

        if (!cursorFree)
        {
            dmx = sf::Mouse::getPosition(window).x - windowWidth / 2;
            dmy = sf::Mouse::getPosition(window).y - windowHeight / 2;
            sf::Mouse::setPosition(sf::Vector2i{ windowWidth / 2, windowHeight / 2 }, window);
            window.setMouseCursorVisible(false);
        }
        else
        {
            window.setMouseCursorVisible(true);
        }

        double camSpeed = 3.0;
        if (wDown)
        {
            Vector3 moveVec = camera.getFrontVec();
            moveVec.scl(camSpeed * delta.asSeconds());
            camera.translate(moveVec);
        }
        if (aDown)
        {
            Vector3 moveVec = camera.getRightVec();
            moveVec.scl(-camSpeed * delta.asSeconds());
            camera.translate(moveVec);
        }
        if (sDown)
        {
            Vector3 moveVec = camera.getFrontVec();
            moveVec.scl(-camSpeed * delta.asSeconds());
            camera.translate(moveVec);
        }
        if (dDown)
        {
            Vector3 moveVec = camera.getRightVec();
            moveVec.scl(camSpeed * delta.asSeconds());
            camera.translate(moveVec);
        }
        if (qDown)
        {
            Vector3 moveVec = Vector3{ 0.0, 1.0, 0.0 };
            moveVec.scl(-camSpeed * delta.asSeconds());
            camera.translate(moveVec);
        }
        if (eDown)
        {
            Vector3 moveVec = Vector3{ 0.0, 1.0, 0.0 };
            moveVec.scl(camSpeed * delta.asSeconds());
            camera.translate(moveVec);
        }
        double camRotSpeed = 0.5;
        camera.rotateYaw(camRotSpeed * -dmx * 0.016);
        camera.rotatePitch(camRotSpeed * -dmy * 0.016);

        renderer.clearColorDepth(Color{ 0, 0, 0, 255 });

        Combined c;
        renderer.renderMesh(bricks, bricksTex, c, camera, lights, Renderer::Lighting::DIFFUSE);

        texture.update(raster.getData().data());

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
}
