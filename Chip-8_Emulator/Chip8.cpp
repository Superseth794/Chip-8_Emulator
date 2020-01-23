//
//  Chip8.cpp
//  Chip-8_Emulator
//
//  Created by Jaraxus on 22/01/2020.
//

#include "Chip8.hpp"

namespace chp {

void Chip8::launch(unsigned int width, unsigned int height, std::string fileName) {
    init();
    
    if (!loadFile(fileName)) {
        throw std::runtime_error("Error: could not load file " + fileName);
        return;
    }
    
    m_pixelWidth = static_cast<float>(width) / WIDTH;
    m_pixelHeight = static_cast<float>(height) / HEIGHT;
    
    sf::RenderWindow gameWindow(sf::VideoMode(m_pixelWidth * WIDTH, m_pixelHeight * HEIGHT), "Chip-8");
    
    sf::Clock executionTimer;
    executionTimer.restart();
    
    sf::Clock displayTimer;
    displayTimer.restart();
    
    while (gameWindow.isOpen()) {
        sf::Event event;
        
        while (gameWindow.pollEvent(event)) {
            
            if (event.type == event.Closed) {
                gameWindow.close();
            }
            
        }
        
        if (1.f / executionTimer.getElapsedTime().asSeconds() <= m_frequency) {
            clearScreen();
            update();
            
            std::cout << "fps: " << 1.f / executionTimer.getElapsedTime().asSeconds() << std::endl;
            executionTimer.restart();
        }
        
        if (1.f / displayTimer.getElapsedTime().asSeconds() <= m_fps) {
            gameWindow.clear(sf::Color::Black);
            
            auto texture {display()};
            sf::Sprite sprite;
            sprite.setTexture(texture->getTexture());
            gameWindow.draw(sprite);
            
            gameWindow.display();
            
            std::cout << "ips: " << 1.f / displayTimer.getElapsedTime().asSeconds() << std::endl;
            displayTimer.restart();
        }
    }
}

void Chip8::init() {
    m_memory.fill(0);
    m_registers.fill(0);
    
    m_programCounter = MEMORY_BEGIN;
    m_stackLevel = 0;
    m_gameCounter = 0;
    m_soundCounter = 0;
    m_registerAdress = 0;
    
    m_opcodeIdentifiers[0].identifier = 0x0000; m_opcodeIdentifiers[0].mask = 0x0FFF;
    m_opcodeIdentifiers[1].identifier = 0xFFFF; m_opcodeIdentifiers[1].mask = 0x00E0;
    m_opcodeIdentifiers[2].identifier = 0xFFFF; m_opcodeIdentifiers[2].mask = 0x00EE;
    m_opcodeIdentifiers[3].identifier = 0xF000; m_opcodeIdentifiers[3].mask = 0x1000;
    m_opcodeIdentifiers[4].identifier = 0xF000; m_opcodeIdentifiers[4].mask = 0x2000;
    m_opcodeIdentifiers[5].identifier = 0xF000; m_opcodeIdentifiers[5].mask = 0x3000;
    m_opcodeIdentifiers[6].identifier = 0xF000; m_opcodeIdentifiers[6].mask = 0x4000;
    m_opcodeIdentifiers[7].identifier = 0xF00F; m_opcodeIdentifiers[7].mask = 0x5000;
    m_opcodeIdentifiers[8].identifier = 0xF000; m_opcodeIdentifiers[8].mask = 0x6000;
    m_opcodeIdentifiers[9].identifier = 0xF000; m_opcodeIdentifiers[9].mask = 0x7000;
    m_opcodeIdentifiers[10].identifier = 0xF00F; m_opcodeIdentifiers[10].mask = 0x8000;
    m_opcodeIdentifiers[11].identifier = 0xF00F; m_opcodeIdentifiers[11].mask = 0x8001;
    m_opcodeIdentifiers[12].identifier = 0xF00F; m_opcodeIdentifiers[12].mask = 0x8002;
    m_opcodeIdentifiers[13].identifier = 0xF00F; m_opcodeIdentifiers[13].mask = 0x8003;
    m_opcodeIdentifiers[14].identifier = 0xF00F; m_opcodeIdentifiers[14].mask = 0x8004;
    m_opcodeIdentifiers[15].identifier = 0xF00F; m_opcodeIdentifiers[15].mask = 0x8005;
    m_opcodeIdentifiers[16].identifier = 0xF00F; m_opcodeIdentifiers[16].mask = 0x8006;
    m_opcodeIdentifiers[17].identifier = 0xF00F; m_opcodeIdentifiers[17].mask = 0x8007;
    m_opcodeIdentifiers[18].identifier = 0xF00F; m_opcodeIdentifiers[18].mask = 0x800E;
    m_opcodeIdentifiers[19].identifier = 0xF00F; m_opcodeIdentifiers[19].mask = 0x9000;
    m_opcodeIdentifiers[20].identifier = 0xF000; m_opcodeIdentifiers[20].mask = 0xA000;
    m_opcodeIdentifiers[21].identifier = 0xF000; m_opcodeIdentifiers[21].mask = 0xB000;
    m_opcodeIdentifiers[22].identifier = 0xF000; m_opcodeIdentifiers[22].mask = 0xC000;
    m_opcodeIdentifiers[23].identifier = 0xF000; m_opcodeIdentifiers[23].mask = 0xD000;
    m_opcodeIdentifiers[24].identifier = 0xF0FF; m_opcodeIdentifiers[24].mask = 0xE09E;
    m_opcodeIdentifiers[25].identifier = 0xF0FF; m_opcodeIdentifiers[25].mask = 0xE0A1;
    m_opcodeIdentifiers[26].identifier = 0xF0FF; m_opcodeIdentifiers[26].mask = 0xF007;
    m_opcodeIdentifiers[27].identifier = 0xF0FF; m_opcodeIdentifiers[27].mask = 0xF00A;
    m_opcodeIdentifiers[28].identifier = 0xF0FF; m_opcodeIdentifiers[28].mask = 0xF015;
    m_opcodeIdentifiers[29].identifier = 0xF0FF; m_opcodeIdentifiers[29].mask = 0xF018;
    m_opcodeIdentifiers[30].identifier = 0xF0FF; m_opcodeIdentifiers[30].mask = 0xF01E;
    m_opcodeIdentifiers[31].identifier = 0xF0FF; m_opcodeIdentifiers[31].mask = 0xF029;
    m_opcodeIdentifiers[32].identifier = 0xF0FF; m_opcodeIdentifiers[32].mask = 0xF033;
    m_opcodeIdentifiers[33].identifier = 0xF0FF; m_opcodeIdentifiers[33].mask = 0xF055;
    m_opcodeIdentifiers[34].identifier = 0xF0FF; m_opcodeIdentifiers[34].mask = 0xF065;
}

bool Chip8::loadFile(std::string fileName) {
    std::ifstream sourceFile;
    sourceFile.open(fileName, std::ifstream::in);
    
    std::uint8_t valueRead;
    std::uint8_t bytesRead = 0;
    
    while (sourceFile.is_open() && !sourceFile.eof() && bytesRead < static_cast<std::uint8_t>(MEMORY_SIZE - MEMORY_BEGIN)) {
        valueRead = sourceFile.get();
        m_memory[MEMORY_BEGIN + bytesRead++] = valueRead;
    }
    
    if (!sourceFile.is_open()) {
        std::cout << "Error: " << strerror(errno) << std::endl;
        return false;
    }
    
    sourceFile.close();
    
    return true;
}

void Chip8::update() {
    if (m_gameCounter > 0)
        m_gameCounter--;
    
    if (m_soundCounter > 0)
        m_soundCounter--;
    
    auto const opcode {getCurrentOpcode()};
    auto const actionId {getActionFromOpcode(opcode)};
    computeAction(actionId, opcode);
    m_programCounter += 2;
}

std::unique_ptr<sf::RenderTexture> Chip8::display() {
    
    const int textureWidth = WIDTH * m_pixelWidth;
    const int textureHeight = HEIGHT * m_pixelHeight;
    
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(textureWidth, textureHeight);
    
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            sf::RectangleShape pixel;
            pixel.setSize({m_pixelWidth, m_pixelHeight});
//            pixel.setFillColor(m_pixels[y * HEIGHT + x] ? sf::Color::White : sf::Color::Black);
            pixel.setFillColor((x + y) % 2 == 0 ? sf::Color::White : sf::Color::Black);
            pixel.setPosition(x * m_pixelWidth, textureHeight - (y + 1) * m_pixelHeight);
            texture->draw(pixel);
        }
    }
    
    return texture;
}

std::uint16_t Chip8::getCurrentOpcode() {
    return ((m_memory[m_programCounter] << 8) + m_memory[m_programCounter + 1]);
}

std::uint8_t Chip8::getActionFromOpcode(std::uint16_t opcode) {
    for (int i = 0; i < m_opcodeIdentifiers.size(); ++i) {
        auto const& id {m_opcodeIdentifiers[i]};
        if (id.identifier == (id.mask & opcode))
            return i;
    }
    return 0;
}

void Chip8::computeAction(std::uint8_t actionId, std::uint16_t opcode) {
    
    std::uint8_t b3,b2,b1;
    
    b3 = (opcode&(0x0F00)) >> 8;
    b2 = (opcode&(0x00F0)) >> 4;
    b1 = (opcode&(0x000F));
    
    switch (actionId) {
        case 0:
            
            break;
        
        case 1:
            clearScreen();
            break;
        
        case 2:
            
            break;
        
        case 3:
            
            break;
        
        case 4:
            
            break;
        
        case 5:
            
            break;
        
        case 6:
            
            break;
        
        case 7:
            
            break;
        
        case 8:
            
            break;
        
        case 9:
            
            break;
        
        case 10:
            
            break;
        
        case 11:
            
            break;
        
        case 12:
            
            break;
        
        case 13:
            
            break;
        
        case 14:
            
            break;
        
        case 15:
            
            break;
        
        case 16:
            
            break;
        
        case 17:
            
            break;
        
        case 18:
            
            break;
        
        case 19:
            
            break;
        
        case 20:
            
            break;
        
        case 21:
            
            break;
        
        case 22:
            
            break;
        
        case 23:
            drawSprite(b1, b2, b3);
            break;
        
        case 24:
            
            break;
        
        case 25:
            
            break;
        
        case 26:
            
            break;
        
        case 27:
            
            break;
        
        case 28:
            
            break;
        
        case 29:
            
            break;
        
        case 30:
            
            break;
        
        case 31:
            
            break;
        
        case 32:
            
            break;
        
        case 33:
            
            break;
        
        case 34:
            
            break;
        
        case 35:
        
        break;
        
        default:
            throw std::runtime_error("Action id " + std::to_string(actionId) + " could not be recognised neither handled");
            break;
    }
}

void Chip8::clearScreen() {
    m_pixels.fill(false);
}

void Chip8::drawSprite(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
    m_registers[0xF] = 0;
    
    for (int dY = 0; dY < b1; ++dY) {
        const std::uint8_t rowDescription = m_memory[m_registerAdress + dY];
        const int y = (m_registers[b2] + dY) % HEIGHT;
        for (int bitId = 0; bitId < 8; ++bitId) {
            bool pixelOn = rowDescription & (0x1 << (7 - bitId));
            if (!pixelOn)
                continue;
            const int x = (m_registers[b3] + bitId) % WIDTH;
            if (m_pixels[y * WIDTH + x]) {
                m_registers[0xF] = 1;
            }
            m_pixels[y * WIDTH + x] = !m_pixels[y * WIDTH + x];
        }
    }
}

}
