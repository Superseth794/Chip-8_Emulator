//
//  Chip8.cpp
//  Chip-8_Emulator
//
//  Created on 22/01/2020.
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
            } else if (event.type == event.KeyPressed) {
                handleKey(event.key.code, true);
            } else if (event.type == event.KeyReleased) {
                handleKey(event.key.code, false);
            }
            
        }
        
        if (1.f / executionTimer.getElapsedTime().asSeconds() <= m_frequency) {
            update();
            
//            std::cout << "fps: " << 1.f / executionTimer.getElapsedTime().asSeconds() << std::endl;
            executionTimer.restart();
        }
        
        if (1.f / displayTimer.getElapsedTime().asSeconds() <= m_fps) {
            gameWindow.clear(sf::Color::Black);
            
            auto texture {display()};
            sf::Sprite sprite;
            sprite.setTexture(texture->getTexture());
            gameWindow.draw(sprite);
            
            gameWindow.display();
            
//            std::cout << "ips: " << 1.f / displayTimer.getElapsedTime().asSeconds() << std::endl;
            displayTimer.restart();
        }
    }
}

void Chip8::init() {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    m_memory.fill(0);
    m_registers.fill(0);
    m_keyPressed.fill(false);
    
    clearScreen();
    
    loadFont();
    
    m_programCounter = MEMORY_BEGIN;
    m_stackLevel = 0;
    m_gameCounter = 0;
    m_soundCounter = 0;
    m_registerAdress = 0;
    
    if (!m_defaultSound.openFromFile("Resources/sfx_sounds_high2.wav"))
        throw std::runtime_error("Error: could not load sound from file Resources/sfx_sounds_high2.wav");
    
    m_opcodeIdentifiers[0].mask = 0x0000; m_opcodeIdentifiers[0].identifier = 0x0FFF;
    m_opcodeIdentifiers[1].mask = 0xFFFF; m_opcodeIdentifiers[1].identifier = 0x00E0;
    m_opcodeIdentifiers[2].mask = 0xFFFF; m_opcodeIdentifiers[2].identifier = 0x00EE;
    m_opcodeIdentifiers[3].mask = 0xF000; m_opcodeIdentifiers[3].identifier = 0x1000;
    m_opcodeIdentifiers[4].mask = 0xF000; m_opcodeIdentifiers[4].identifier = 0x2000;
    m_opcodeIdentifiers[5].mask = 0xF000; m_opcodeIdentifiers[5].identifier = 0x3000;
    m_opcodeIdentifiers[6].mask = 0xF000; m_opcodeIdentifiers[6].identifier = 0x4000;
    m_opcodeIdentifiers[7].mask = 0xF00F; m_opcodeIdentifiers[7].identifier = 0x5000;
    m_opcodeIdentifiers[8].mask = 0xF000; m_opcodeIdentifiers[8].identifier = 0x6000;
    m_opcodeIdentifiers[9].mask = 0xF000; m_opcodeIdentifiers[9].identifier = 0x7000;
    m_opcodeIdentifiers[10].mask = 0xF00F; m_opcodeIdentifiers[10].identifier = 0x8000;
    m_opcodeIdentifiers[11].mask = 0xF00F; m_opcodeIdentifiers[11].identifier = 0x8001;
    m_opcodeIdentifiers[12].mask = 0xF00F; m_opcodeIdentifiers[12].identifier = 0x8002;
    m_opcodeIdentifiers[13].mask = 0xF00F; m_opcodeIdentifiers[13].identifier = 0x8003;
    m_opcodeIdentifiers[14].mask = 0xF00F; m_opcodeIdentifiers[14].identifier = 0x8004;
    m_opcodeIdentifiers[15].mask = 0xF00F; m_opcodeIdentifiers[15].identifier = 0x8005;
    m_opcodeIdentifiers[16].mask = 0xF00F; m_opcodeIdentifiers[16].identifier = 0x8006;
    m_opcodeIdentifiers[17].mask = 0xF00F; m_opcodeIdentifiers[17].identifier = 0x8007;
    m_opcodeIdentifiers[18].mask = 0xF00F; m_opcodeIdentifiers[18].identifier = 0x800E;
    m_opcodeIdentifiers[19].mask = 0xF00F; m_opcodeIdentifiers[19].identifier = 0x9000;
    m_opcodeIdentifiers[20].mask = 0xF000; m_opcodeIdentifiers[20].identifier = 0xA000;
    m_opcodeIdentifiers[21].mask = 0xF000; m_opcodeIdentifiers[21].identifier = 0xB000;
    m_opcodeIdentifiers[22].mask = 0xF000; m_opcodeIdentifiers[22].identifier = 0xC000;
    m_opcodeIdentifiers[23].mask = 0xF000; m_opcodeIdentifiers[23].identifier = 0xD000;
    m_opcodeIdentifiers[24].mask = 0xF0FF; m_opcodeIdentifiers[24].identifier = 0xE09E;
    m_opcodeIdentifiers[25].mask = 0xF0FF; m_opcodeIdentifiers[25].identifier = 0xE0A1;
    m_opcodeIdentifiers[26].mask = 0xF0FF; m_opcodeIdentifiers[26].identifier = 0xF007;
    m_opcodeIdentifiers[27].mask = 0xF0FF; m_opcodeIdentifiers[27].identifier = 0xF00A;
    m_opcodeIdentifiers[28].mask = 0xF0FF; m_opcodeIdentifiers[28].identifier = 0xF015;
    m_opcodeIdentifiers[29].mask = 0xF0FF; m_opcodeIdentifiers[29].identifier = 0xF018;
    m_opcodeIdentifiers[30].mask = 0xF0FF; m_opcodeIdentifiers[30].identifier = 0xF01E;
    m_opcodeIdentifiers[31].mask = 0xF0FF; m_opcodeIdentifiers[31].identifier = 0xF029;
    m_opcodeIdentifiers[32].mask = 0xF0FF; m_opcodeIdentifiers[32].identifier = 0xF033;
    m_opcodeIdentifiers[33].mask = 0xF0FF; m_opcodeIdentifiers[33].identifier = 0xF055;
    m_opcodeIdentifiers[34].mask = 0xF0FF; m_opcodeIdentifiers[34].identifier = 0xF065;
}

void Chip8::loadFont() {
    m_memory[0]=0xF0; m_memory[1]=0x90; m_memory[2]=0x90;m_memory[3]=0x90; m_memory[4]=0xF0; // O
    
    m_memory[5]=0x20;m_memory[6]=0x60;m_memory[7]=0x20;m_memory[8]=0x20;m_memory[9]=0x70; // 1
    
    m_memory[10]=0xF0;m_memory[11]=0x10;m_memory[12]=0xF0;m_memory[13]=0x80; m_memory[14]=0xF0; // 2
    
    m_memory[15]=0xF0;m_memory[16]=0x10;m_memory[17]=0xF0;m_memory[18]=0x10;m_memory[19]=0xF0; // 3
    
    m_memory[20]=0x90;m_memory[21]=0x90;m_memory[22]=0xF0;m_memory[23]=0x10;m_memory[24]=0x10; // 4
    
    m_memory[25]=0xF0;m_memory[26]=0x80;m_memory[27]=0xF0;m_memory[28]=0x10;m_memory[29]=0xF0; // 5
    
    m_memory[30]=0xF0;m_memory[31]=0x80;m_memory[32]=0xF0;m_memory[33]=0x90;m_memory[34]=0xF0; // 6
    
    m_memory[35]=0xF0;m_memory[36]=0x10;m_memory[37]=0x20;m_memory[38]=0x40;m_memory[39]=0x40; // 7
    
    m_memory[40]=0xF0;m_memory[41]=0x90;m_memory[42]=0xF0;m_memory[43]=0x90;m_memory[44]=0xF0; // 8
    
    m_memory[45]=0xF0;m_memory[46]=0x90;m_memory[47]=0xF0;m_memory[48]=0x10;m_memory[49]=0xF0; // 9
    
    m_memory[50]=0xF0;m_memory[51]=0x90;m_memory[52]=0xF0;m_memory[53]=0x90;m_memory[54]=0x90; // A
    
    m_memory[55]=0xE0;m_memory[56]=0x90;m_memory[57]=0xE0;m_memory[58]=0x90;m_memory[59]=0xE0; // B
    
    m_memory[60]=0xF0;m_memory[61]=0x80;m_memory[62]=0x80;m_memory[63]=0x80;m_memory[64]=0xF0; // C
    
    m_memory[65]=0xE0;m_memory[66]=0x90;m_memory[67]=0x90;m_memory[68]=0x90;m_memory[69]=0xE0; // D
    
    m_memory[70]=0xF0;m_memory[71]=0x80;m_memory[72]=0xF0;m_memory[73]=0x80;m_memory[74]=0xF0; // E
    
    m_memory[75]=0xF0;m_memory[76]=0x80;m_memory[77]=0xF0;m_memory[78]=0x80;m_memory[79]=0x80; // F
}

bool Chip8::loadFile(std::string fileName) {
    std::ifstream sourceFile;
    sourceFile.open(fileName, std::ifstream::in);
    
    std::uint8_t valueRead;
    std::size_t bytesRead = 0;
    
    while (sourceFile.is_open() && !sourceFile.eof() && bytesRead < MEMORY_SIZE - MEMORY_BEGIN) {
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

void Chip8::handleKey(sf::Keyboard::Key key, bool keyPressed) {
    switch (key) {
        case sf::Keyboard::A:
            m_keyPressed[0] = keyPressed;
            break;
            
        case sf::Keyboard::Z:
            m_keyPressed[1] = keyPressed;
        break;
            
        case sf::Keyboard::E:
            m_keyPressed[2] = keyPressed;
        break;
            
        case sf::Keyboard::R:
            m_keyPressed[3] = keyPressed;
        break;
            
        case sf::Keyboard::Q:
            m_keyPressed[4] = keyPressed;
        break;
            
        case sf::Keyboard::S:
            m_keyPressed[5] = keyPressed;
        break;
            
        case sf::Keyboard::D:
            m_keyPressed[6] = keyPressed;
        break;
            
        case sf::Keyboard::F:
            m_keyPressed[7] = keyPressed;
        break;
            
        case sf::Keyboard::W:
            m_keyPressed[8] = keyPressed;
        break;
            
        case sf::Keyboard::X:
            m_keyPressed[9] = keyPressed;
        break;
            
        case sf::Keyboard::C:
            m_keyPressed[10] = keyPressed;
        break;
            
        case sf::Keyboard::V:
            m_keyPressed[11] = keyPressed;
        break;
            
        case sf::Keyboard::U:
            m_keyPressed[12] = keyPressed;
        break;
            
        case sf::Keyboard::I:
            m_keyPressed[13] = keyPressed;
        break;
            
        case sf::Keyboard::O:
            m_keyPressed[14] = keyPressed;
        break;
            
        case sf::Keyboard::P:
            m_keyPressed[15] = keyPressed;
        break;
            
        default:
            // Key event is not handled by Chip8
            break;
    }
}

void Chip8::update() {
    
//    std::cout << m_programCounter << std::endl;
    auto const opcode {getCurrentOpcode()};
    std::cout << "# " << opcode << std::endl;
    auto const actionId {getActionFromOpcode(opcode)};
    std::cout << "> " << actionId << std::endl;
    computeAction(actionId, opcode);
    m_programCounter += 2;
    
    if (m_gameCounter > 0)
        m_gameCounter--;
    
    // https://opengameart.org/content/512-sound-effects-8-bit-style
    
    if (m_soundCounter > 0) {
        m_defaultSound.stop();
        m_defaultSound.play();
        m_soundCounter--;
    }
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
            pixel.setFillColor(m_pixels[y * WIDTH + x] ? sf::Color::White : sf::Color::Black);
//            pixel.setFillColor((x + y) % 2 == 0 ? sf::Color::White : sf::Color::Black);
//            pixel.setPosition(x * m_pixelWidth, textureHeight - (y + 1) * m_pixelHeight);
            pixel.setPosition(x * m_pixelWidth, y * m_pixelHeight);
            texture->draw(pixel);
        }
    }
    
    texture->display();
    
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

void printBinairy(std::uint8_t number) {
    std::bitset<8> n(number);
    std::cout << n << " ";
}

void Chip8::computeAction(std::uint8_t actionId, std::uint16_t opcode) {
    
    std::uint8_t b3,b2,b1;
    
    b3 = (opcode&(0x0F00)) >> 8;
    b2 = (opcode&(0x00F0)) >> 4;
    b1 = (opcode&(0x000F));
    
//    b3 = (opcode >> 8) & 0xF;
//    b2 = (opcode >> 4) & 0xF;
//    b1 = opcode & 0xF;
    
//    std::cout << std::bitset<16>(opcode) << " ";
//    printBinairy(b1);
//    printBinairy(b2);
//    printBinairy(b3);
//    std::cout << std::endl;
    
    std::cout << "-> " << std::hex << actionId << " <-" << std::endl;
    
    switch (actionId) {
        case 0:
            std::cout << "error" << std::endl;
            break;
        
        case 1:
            clearScreen();
            break;
        
        case 2:
            if (m_stackLevel > 0)
                m_programCounter = m_stack[--m_stackLevel];
            break;
        
        case 3:
            m_programCounter = (b3 << 8) + (b2 << 4) + b1;
            m_programCounter -= 2;
            break;
        
        case 4:
            m_stack[m_stackLevel] = m_programCounter;
            
            if (m_stackLevel < MAX_STACK_SIZE)
                m_stackLevel++;
            
            m_programCounter = (b3 << 8) + (b2 << 4) + b1;
            m_programCounter -= 2;
            break;
        
        case 5:
            if (m_registers[b3] == (b2 << 4) + b1)
                m_programCounter += 2;
            break;
        
        case 6:
            if (m_registers[b3] != (b2 << 4) + b1)
                m_programCounter += 2;
            break;
        
        case 7:
            if (m_registers[b3] == m_registers[b2])
                m_programCounter += 2;
            break;
        
        case 8:
            m_registers[b3] = (b2 << 4) + b1;
            break;
        
        case 9:
            m_registers[b3] += (b2 << 4) + b1;
            break;
        
        case 10:
            m_registers[b3] = m_registers[b2];
            break;
        
        case 11:
            m_registers[b3] |= m_registers[b2];
            break;
        
        case 12:
            m_registers[b3] &= m_registers[b2];
            break;
        
        case 13:
            m_registers[b3] ^= m_registers[b2];
            break;
        
        case 14:
            if (m_registers[b3] + m_registers[b2] > 0xFF)
                m_registers[0xF] = 1;
            else
                m_registers[0xF] = 0;
            m_registers[b3] += m_registers[b2];
            break;
        
        case 15:
            if (m_registers[b2] > m_registers[b3])
                m_registers[0xF] = 0;
            else
                m_registers[0xF] = 1;
            m_registers[b3] -= m_registers[b2];
            break;
        
        case 16:
            m_registers[0xF] = m_registers[b3] & 0x01;
            m_registers[b3] = m_registers[b3] >> 1;
            break;
        
        case 17:
            if (m_registers[b3] > m_registers[b2])
                m_registers[0xF] = 0;
            else
                m_registers[0xF] = 1;
            m_registers[b3] = m_registers[b2] - m_registers[b3];
            break;
        
        case 18:
            m_registers[0xF] = (m_registers[b3] >> 7);
            m_registers[b3] = m_registers[b3] << 1;
            break;
        
        case 19:
            if (m_registers[b3] != m_registers[b2])
                m_programCounter += 2;
            break;
        
        case 20:
            m_registerAdress = (b3 << 8) + (b2 << 4) + b1;
            break;
        
        case 21:
            m_programCounter = (b3 << 8) + (b2 << 4) + b1 + m_registers[0];
            m_programCounter -= 2;
            break;
        
        case 22:
            m_registers[b3] = rand() % ((b2 << 4) + b1 + 1);
            break;
        
        case 23:
            drawSprite(b1, b2, b3);
            break;
        
        case 24:
            if (m_keyPressed[m_registers[b3]])
                m_programCounter += 2;
            break;
        
        case 25:
            if (!m_keyPressed[m_registers[b3]])
                m_programCounter += 2;
            break;
        
        case 26:
            m_registers[b3] = m_gameCounter;
            break;
        
        case 27:
            while (!sf::Event::KeyPressed){}
            break;
        
        case 28:
            m_gameCounter = m_registers[b3];
            break;
        
        case 29:
            m_soundCounter = m_registers[b3];
            break;
        
        case 30:
            if (m_registerAdress + m_registers[b3] > 0xFFF)
                m_registers[0xF] = 1;
            else
                m_registers[0xF] = 0;
            m_registerAdress += m_registers[b3];
            break;
        
        case 31:
            m_registerAdress = 5 * m_registers[b3];
            break;
        
        case 32:
            m_memory[m_registerAdress] = (m_registers[b3] - m_registers[b3] % 100) / 100;
            m_memory[m_registerAdress + 1] = ((m_registers[b3] - m_registers[b3] % 10) / 10) % 10;
            m_memory[m_registerAdress + 2] = m_registers[b3] - m_registers[m_registerAdress] * 100 - m_memory[m_registerAdress + 1] * 10;
            break;
        case 33:
            for (std::uint8_t i = 0; i <= b3; ++i) {
                if (m_registerAdress + i < MEMORY_SIZE)
                    m_registers[m_registerAdress + i] = m_registers[i];
            }
            break;
        
        case 34:
            for (std::uint8_t i = 0; i <= b3; ++i) {
                if (m_registerAdress + i < MEMORY_SIZE)
                    m_registers[i] = m_registers[m_registerAdress + i];
            }
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
