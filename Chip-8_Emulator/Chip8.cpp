//
//  Chip8.cpp
//  Chip-8_Emulator
//
//  Created by Jaraxus on 22/01/2020.
//

#include "Chip8.hpp"

namespace chp {

void Chip8::launch(unsigned int width, unsigned int height) {
    init();
    
    pixelWidth = width / 64.f;
    pixelHeight = height / 32.f;
    
    sf::RenderWindow gameWindow(sf::VideoMode(pixelWidth * 64, pixelHeight * 32), "Chip-8");
    
    while (gameWindow.isOpen()) {
        sf::Event event;
        
        while (gameWindow.pollEvent(event)) {
            
            if (event.type == event.Closed) {
                gameWindow.close();
            }
            
        }
        
        gameWindow.clear(sf::Color::Black);
        
        clearScreen();
        update();
        
        auto texture {display()};
        sf::Sprite sprite;
        sprite.setTexture(texture->getTexture());
        gameWindow.draw(sprite);
        
        gameWindow.display();
    }
}

void Chip8::init() {
    m_memory.fill(0);
    m_registers.fill(0);
    
    m_programCounter = 512;
    m_stackLevel = 0;
    m_gameCounter = 0;
    m_soundCounter = 0;
    m_registerAdress = 0;
}

void Chip8::update() {
    if (m_gameCounter > 0)
        m_gameCounter--;
    
    if (m_soundCounter > 0)
        m_soundCounter--;
}

std::unique_ptr<sf::RenderTexture> Chip8::display() {
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(64 * pixelWidth, 32 * pixelHeight);
    
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            sf::RectangleShape pixel;
            pixel.setSize({pixelWidth, pixelHeight});
            pixel.setFillColor(m_pixels[y * 64 + x] ? sf::Color::White : sf::Color::Black);
            pixel.setPosition(x * pixelWidth, y * pixelHeight);
            texture->draw(pixel);
        }
    }
    
    return texture;
}

void Chip8::clearScreen() {
    m_pixels.fill(false);
}

}
