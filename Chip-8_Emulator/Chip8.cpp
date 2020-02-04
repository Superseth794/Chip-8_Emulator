//
//  Chip8.cpp
//  Chip-8_Emulator
//
//  Created on 22/01/2020.
//

#include "Chip8.hpp"

namespace chp {

void Chip8::launch(std::string const& configFilename) {
    init(configFilename);
    
    sf::RenderWindow gameWindow(sf::VideoMode(m_windowWidth, m_windowHeight), "Chip-8");
    
    m_executionTimer.restart();
    m_displayTimer.restart();
    
    while (gameWindow.isOpen()) {
        sf::Event event;
        
        while (gameWindow.pollEvent(event)) {
            
            if (event.type == event.Closed) {
                gameWindow.close();
            } else if (event.type == event.KeyPressed) {
                handleKey(event.key.code, true);
                std::cout << ExtendedInputs::getKeyName(event.key.code) << std::endl;
            } else if (event.type == event.KeyReleased) {
                handleKey(event.key.code, false);
            }
            
        }
        
        if (1.f / m_executionTimer.getElapsedTime().asSeconds() <= m_frequency && (!m_isPaused || m_doSingleJump)) {
            update();
            m_executionTimer.restart();
            m_doSingleJump = false;
        }
        
        if (1.f / m_displayTimer.getElapsedTime().asSeconds() <= m_fps) {
            gameWindow.clear(sf::Color::Black);
            
            sf::Sprite sprite;
            auto texture {display()};
            sprite.setTexture(texture->getTexture());
            gameWindow.draw(sprite);
            
            gameWindow.display();
            
            m_displayTimer.restart();
        }
    }
}

void Chip8::init(std::string const& configFilename) {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    loadConfig(configFilename);
    clearScreen();
    loadFont();
    
    loadOpcodes();
    loadActions();
    
    m_windowWidth = m_screenWidth * (m_showOpcodes || m_showMemory ? 1.4f : 1.f);
    m_windowHeight = m_screenHeigth * (m_showDebugInfos || m_showMemory ? 1.7f : 1.f);
    
    m_programCounter = m_memoryBegin;
    m_stackLevel = 0;
    m_gameCounter = 0;
    m_soundCounter = 0;
    m_registerAdress = 0;
    
    m_doSingleJump = false;
    
    m_opcodesDisplayBegining = 0;
    
    std::fill(m_keyPressed.begin(), m_keyPressed.end(), false);
}

void Chip8::loadConfig(std::string const& configFilename) {
    Parser parser;
    if (!parser.loadFile(configFilename))
        throw std::runtime_error("Error: could not load config file " + configFilename);
    
    m_configFilename = configFilename;
    
    m_screenWidth = parser.get<decltype(m_windowWidth)>("screen_width").value_or(m_screenWidth);
    m_screenHeigth = parser.get<decltype(m_windowHeight)>("screen_height").value_or(m_screenHeigth);
    
    loadInputsKeys(parser);
    
    m_frequency = parser.get<decltype(m_frequency)>("update_frequency").value_or(m_frequency);
    m_fps = parser.get<decltype(m_fps)>("framerate").value_or(m_fps);
    
    m_gameFilename = parser.get<decltype(m_gameFilename)>("file").value_or(m_gameFilename);
    m_soundFilename = parser.get<decltype(m_soundFilename)>("sound").value_or(m_soundFilename);
    m_fontFilename = parser.get<decltype(m_fontFilename)>("font").value_or(m_fontFilename);
    
    m_showDebugInfos = parser.get<decltype(m_showDebugInfos)>("show_infos").value_or(m_showDebugInfos);
    m_showOpcodes = parser.get<decltype(m_showOpcodes)>("show_opcodes").value_or(m_showOpcodes);
    m_showMemory = parser.get<decltype(m_showMemory)>("show_memory").value_or(m_showMemory);
    
    m_memorySize = parser.get<decltype(m_memorySize)>("memory_size").value_or(m_memorySize);
    m_memoryBegin = parser.get<decltype(m_memoryBegin)>("memory_start").value_or(m_memoryBegin);
    m_maxStackSize = parser.get<decltype(m_maxStackSize)>("max_stack_depth").value_or(m_maxStackSize);
    
    m_memory.resize(m_memorySize);
    m_stack.resize(m_maxStackSize);
    m_pixels.resize(m_width * m_height);
    
    std::fill(m_memory.begin(), m_memory.end(), 0);
    std::fill(m_registers.begin(), m_registers.end(), 0);
    
    m_width = parser.get<decltype(m_width)>("width_resolution").value_or(m_width);
    m_height = parser.get<decltype(m_height)>("height_resolution").value_or(m_height);
    
    if (!loadFile(m_gameFilename))
        throw std::runtime_error("Error: could not load game from file " + m_gameFilename);
    if (!m_defaultSound.openFromFile(m_soundFilename))
        throw std::runtime_error("Error: could not load sound from file " + m_soundFilename);
    if (!m_defaultFont.loadFromFile(m_fontFilename))
        throw std::runtime_error("Error: could not load font from file " + m_fontFilename);
    
    std::cout << "Succesfully loaded " << m_configFilename << std::endl;
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
    
    while (sourceFile.is_open() && !sourceFile.eof() && bytesRead < m_memorySize - m_memoryBegin) {
        valueRead = sourceFile.get();
        m_memory[m_memoryBegin + bytesRead++] = valueRead;
    }
    
    if (!sourceFile.is_open()) {
        std::cout << "Error: " << strerror(errno) << std::endl;
        return false;
    }
    
    sourceFile.close();
    
    return true;
}

void Chip8::loadOpcodes() {
    m_opcodeIdentifiers[0] = {
        "0NNN",
        0x0000,
        0x0FFF,
        "SYS NNN"
    };
    m_opcodeIdentifiers[1] = {
        "00E0",
        0xFFFF,
        0x00E0,
        "CLS"
    };
    m_opcodeIdentifiers[2] = {
        "00EE",
        0xFFFF,
        0x00EE,
        "RET"
    };
    m_opcodeIdentifiers[3] = {
        "1NNN",
        0xF000,
        0x1000,
        "JP NNN"
    };
    m_opcodeIdentifiers[4] = {
        "2NNN",
        0xF000,
        0x2000,
        "CALL NNN"
    };
    m_opcodeIdentifiers[5] = {
        "3XNN",
        0xF000,
        0x3000,
        "SE VX, NN"
    };
    m_opcodeIdentifiers[6] = {
        "4XNN",
        0xF000,
        0x4000,
        "SNE VX, NN"
    };
    m_opcodeIdentifiers[7] = {
        "5XY0",
        0xF00F,
        0x5000,
        "SE VX, VY"
    };
    m_opcodeIdentifiers[8] = {
        "6XNN",
        0xF000,
        0x6000,
        "LD VX, NN"
    };
    m_opcodeIdentifiers[9] = {
        "7XNN",
        0xF000,
        0x7000,
        "ADD VX, NN"
    };
    m_opcodeIdentifiers[10] = {
        "8XY0",
        0xF00F,
        0x8000,
        "LD VX, VY"
    };
    m_opcodeIdentifiers[11] = {
        "8XY1",
        0xF00F,
        0x8001,
        "OR VX, VY"
    };
    m_opcodeIdentifiers[12] = {
        "8XY2",
        0xF00F,
        0x8002,
        "AND VX, VY"
    };
    m_opcodeIdentifiers[13] = {
        "BXY3",
        0xF00F,
        0x8003,
        "XOR VX, VY"
    };
    m_opcodeIdentifiers[14] = {
        "8XY4",
        0xF00F,
        0x8004,
        "ADD VX, VY"
    };
    m_opcodeIdentifiers[15] = {
        "8XY5",
        0xF00F,
        0x8005,
        "SUB VX, VY"
    };
    m_opcodeIdentifiers[16] = {
        "8XY6",
        0xF00F,
        0x8006,
        "SHR VX"
    };
    m_opcodeIdentifiers[17] = {
        "8XY7",
        0xF00F,
        0x8007,
        "SUBN VX, VY"
    };
    m_opcodeIdentifiers[18] = {
        "8XYE",
        0xF00F,
        0x800E,
        "SHL VX"
    };
    m_opcodeIdentifiers[19] = {
        "9XY0",
        0xF00F,
        0x9000,
        "SNE VX, VY"
    };
    m_opcodeIdentifiers[20] = {
        "ANNN",
        0xF000,
        0xA000,
        "LD I, NNN"
    };
    m_opcodeIdentifiers[21] = {
        "BNNN",
        0xF000,
        0xB000,
        "JP V0, NNN"
    };
    m_opcodeIdentifiers[22] = {
        "CXNN",
        0xF000,
        0xC000,
        "RND VX, NN"
    };
    m_opcodeIdentifiers[23] = {
        "DXYN",
        0xF000,
        0xD000,
        "DRW VX, VY, N"
    };
    m_opcodeIdentifiers[24] = {
        "EX9E",
        0xF0FF,
        0xE09E,
        "SKP VX"
    };
    m_opcodeIdentifiers[25] = {
        "EXA1",
        0xF0FF,
        0xE0A1,
        "SKNP VX"
    };
    m_opcodeIdentifiers[26] = {
        "FX07",
        0xF0FF,
        0xF007,
        "LD VX, DT"
    };
    m_opcodeIdentifiers[27] = {
        "FX0A",
        0xF0FF,
        0xF00A,
        "LD VX, K"
    };
    m_opcodeIdentifiers[28] = {
        "FX15",
        0xF0FF,
        0xF015,
        "LD DT, VX"
    };
    m_opcodeIdentifiers[29] = {
        "FX18",
        0xF0FF,
        0xF018,
        "LD ST, VX"
    };
    m_opcodeIdentifiers[30] = {
        "FX1E",
        0xF0FF,
        0xF01E,
        "ADD I, VX"
    };
    m_opcodeIdentifiers[31] = {
        "FX29",
        0xF0FF,
        0xF029,
        "LD F, VX"
    };
    m_opcodeIdentifiers[32] = {
        "FX33",
        0xF0FF,
        0xF033,
        "BCD VX"
    };
    m_opcodeIdentifiers[33] = {
        "FX55",
        0xF0FF,
        0xF055,
        "LD [I], VX"
    };
    m_opcodeIdentifiers[34] = {
        "FX65",
        0xF0FF,
        0xF065,
        "LD VX, [I]"
    };
}

void Chip8::loadActions() {
    m_actions[0] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        std::cout << "Error: undefined action (id: 0)" << std::endl;
    };
    
    m_actions[1] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        clearScreen();
    };
    
    m_actions[2] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_stackLevel > 0)
        m_programCounter = m_stack[--m_stackLevel];
    };
    
    m_actions[3] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_programCounter = (b3 << 8) + (b2 << 4) + b1;
        m_programCounter -= 2;
    };
    
    m_actions[4] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_stack[m_stackLevel] = m_programCounter;
        
        if (m_stackLevel < m_maxStackSize)
            m_stackLevel++;
        
        m_programCounter = (b3 << 8) + (b2 << 4) + b1;
        m_programCounter -= 2;
    };
    
    m_actions[5] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b3] == (b2 << 4) + b1)
            m_programCounter += 2;
    };
    
    m_actions[6] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b3] != (b2 << 4) + b1)
            m_programCounter += 2;
    };
    
    m_actions[7] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b3] == m_registers[b2])
            m_programCounter += 2;
    };
    
    m_actions[8] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] = (b2 << 4) + b1;
    };
    
    m_actions[9] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] += (b2 << 4) + b1;
    };
    
    m_actions[10] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] = m_registers[b2];
    };
    
    m_actions[11] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] |= m_registers[b2];
    };
    
    m_actions[12] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] &= m_registers[b2];
    };
    
    m_actions[13] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] ^= m_registers[b2];
    };
    
    m_actions[14] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b3] + m_registers[b2] > 0xFF)
            m_registers[0xF] = 1;
        else
            m_registers[0xF] = 0;
        m_registers[b3] += m_registers[b2];
    };
    
    m_actions[15] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b2] > m_registers[b3])
            m_registers[0xF] = 0;
        else
            m_registers[0xF] = 1;
        m_registers[b3] -= m_registers[b2];
    };
    
    m_actions[16] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[0xF] = m_registers[b3] & 0x01;
        m_registers[b3] = m_registers[b3] >> 1;
    };
    
    m_actions[17] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b3] > m_registers[b2])
            m_registers[0xF] = 0;
        else
            m_registers[0xF] = 1;
        m_registers[b3] = m_registers[b2] - m_registers[b3];
    };
    
    m_actions[18] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[0xF] = (m_registers[b3] >> 7);
        m_registers[b3] = m_registers[b3] << 1;
    };
    
    m_actions[19] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registers[b3] != m_registers[b2])
            m_programCounter += 2;
    };
    
    m_actions[20] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registerAdress = (b3 << 8) + (b2 << 4) + b1;
    };
    
    m_actions[21] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_programCounter = (b3 << 8) + (b2 << 4) + b1 + m_registers[0];
        m_programCounter -= 2;
    };
    
    m_actions[22] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] = rand() % ((b2 << 4) + b1 + 1);
    };
    
    m_actions[23] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        drawSprite(b1, b2, b3);
    };
    
    m_actions[24] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_keyPressed[m_registers[b3]])
            m_programCounter += 2;
    };
    
    m_actions[25] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (!m_keyPressed[m_registers[b3]])
            m_programCounter += 2;
    };
    
    m_actions[26] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registers[b3] = m_gameCounter;
    };
    
    m_actions[27] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        while (!sf::Event::KeyPressed){}
    };
    
    m_actions[28] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_gameCounter = m_registers[b3];
    };
    
    m_actions[29] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_soundCounter = m_registers[b3];
    };
    
    m_actions[30] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        if (m_registerAdress + m_registers[b3] > 0xFFF)
            m_registers[0xF] = 1;
        else
            m_registers[0xF] = 0;
        m_registerAdress += m_registers[b3];
    };
    
    m_actions[31] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_registerAdress = 5 * m_registers[b3];
    };
    
    m_actions[32] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        m_memory[m_registerAdress] = (m_registers[b3] - m_registers[b3] % 100) / 100;
        m_memory[m_registerAdress + 1] = ((m_registers[b3] - m_registers[b3] % 10) / 10) % 10;
        m_memory[m_registerAdress + 2] = m_registers[b3] - m_registers[m_registerAdress] * 100 - m_memory[m_registerAdress + 1] * 10;
    };
    
    m_actions[33] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        for (std::uint8_t i = 0; i <= b3; ++i) {
            if (m_registerAdress + i < m_memorySize)
                m_registers[m_registerAdress + i] = m_registers[i];
        }
    };
    
    m_actions[34] = [&](std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
        for (std::uint8_t i = 0; i <= b3; ++i) {
            if (m_registerAdress + i < m_memorySize)
                m_registers[i] = m_registers[m_registerAdress + i];
        }
    };
}

void Chip8::loadInputsKeys(Parser & parser) {
    std::array<std::string, NB_CONTROLS_AVAILABLES> keyNames {"key_1", "key_2", "key_3", "key_4", "key_5", "key_6", "key_7", "key_8", "key_9", "key_A", "key_0", "key_B", "key_C", "key_D", "key_E", "key_F", "pause_key", "reload_key", "jump_key"};
    std::array<std::string, NB_CONTROLS_AVAILABLES> defaultKeys {"A", "Z", "E", "Q", "S", "D", "W", "X", "C", "U", "I", "O", "R", "F", "V", "P", "Enter", "Delete", "Tab"};
    
    for (int keyId = 0; keyId < NB_CONTROLS_AVAILABLES; ++keyId) {
        m_controlKeys[keyId] = ExtendedInputs::getAssociatedKey(parser.get<std::string>(keyNames[keyId]).value_or(defaultKeys[keyId]));
        if (m_controlKeys[keyId] == sf::Keyboard::Unknown) {
            std::cout << "Error: could not get key for identifier " << keyNames[keyId] << " --> default key " << defaultKeys[keyId] << " selected" << std::endl;
            m_controlKeys[keyId] = ExtendedInputs::getAssociatedKey(defaultKeys[keyId]);
        }
    }
}

void Chip8::handleKey(sf::Keyboard::Key key, bool keyPressed) {
    for (int keyId = 0; keyId < 16; ++keyId) {
        if (m_controlKeys[keyId] == key)
            m_keyPressed[keyId] = keyPressed;
    }
    
    if (key == m_controlKeys[16] && keyPressed) { // Handles pause
        m_isPaused = !m_isPaused;
    } else if (key == m_controlKeys[17] && keyPressed) { // Handles reload
        init(m_configFilename);
    } else if (key == m_controlKeys[18]) { // Handles step-by-step execution
        if (keyPressed && m_isPaused)
            m_doSingleJump = true;
    }
}

void Chip8::update() {
    
    auto const opcode {getCurrentOpcode()};
    auto const actionId {getActionFromOpcode(opcode)};
    
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
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(m_windowWidth, m_windowHeight);
    texture->clear(sf::Color::Red);
    
    sf::Sprite screenSprite {};
    auto screenTexture {displayScreen()};
    screenSprite.setTexture(screenTexture->getTexture());
    screenSprite.setPosition(0.f, 0.f);
    texture->draw(screenSprite);
    
    if (m_showOpcodes) {
        sf::Sprite opcodesSprite {};
        auto opcodesTexture {displayOpcodes()};
        opcodesSprite.setTexture(opcodesTexture->getTexture());
        opcodesSprite.setPosition(m_screenWidth, 0.f);
        texture->draw(opcodesSprite);
    }
    
    if (m_showDebugInfos) {
        sf::Sprite debugSprite {};
        auto debugTexture {displayDebugInfos()};
        debugSprite.setTexture(debugTexture->getTexture());
        debugSprite.setPosition(0.f, m_screenHeigth);
        texture->draw(debugSprite);
    }
    
    if (m_showMemory) {
        sf::Sprite memorySprite {};
        auto memoryTexture {displayMemory()};
        memorySprite.setTexture(memoryTexture->getTexture());
        memorySprite.setPosition(m_screenWidth, m_screenHeigth);
        texture->draw(memorySprite);
        
        if (!m_showOpcodes) {
            sf::RenderTexture opcodesTexture;
            opcodesTexture.create(m_screenWidth * 0.4f, m_screenHeigth);
            opcodesTexture.clear(sf::Color(61, 75, 105));
            opcodesTexture.display();
            
            sf::Sprite opcodesSprite {};
            opcodesSprite.setTexture(opcodesTexture.getTexture());
            opcodesSprite.setPosition(m_screenWidth, 0.f);
            texture->draw(opcodesSprite);
        }
        if (!m_showDebugInfos) {
            sf::RenderTexture debugTexture;
            debugTexture.create(m_screenWidth, m_screenHeigth * 0.7f);
            debugTexture.clear(sf::Color(59, 81, 128));
            debugTexture.display();
            
            sf::Sprite debugSprite {};
            debugSprite.setTexture(debugTexture.getTexture());
            debugSprite.setPosition(0.f, m_screenHeigth);
            texture->draw(debugSprite);
        }
    }
    
    texture->display();
    
    return texture;
}

std::unique_ptr<sf::RenderTexture> Chip8::displayScreen() {
    const float pixelWidth = static_cast<float>(m_screenWidth) / m_width;
    const float pixelHeight = static_cast<float>(m_screenHeigth) / m_height;
    
    const int textureWidth = m_width * pixelWidth;
    const int textureHeight = m_height * pixelHeight;
    
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(textureWidth, textureHeight);
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            sf::RectangleShape pixel;
            pixel.setSize({pixelWidth, pixelHeight});
            pixel.setFillColor(m_pixels[y * m_width + x] ? sf::Color::White : sf::Color::Black);
            pixel.setPosition(x * pixelWidth, y * pixelHeight);
            texture->draw(pixel);
        }
    }
    
    texture->display();
    
    return texture;
}

std::unique_ptr<sf::RenderTexture> Chip8::displayOpcodes() {
    float subViewWidth = m_screenWidth * 0.4f;
    float subViewHeight = m_screenHeigth;
    
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(subViewWidth, subViewHeight);
    texture->clear(sf::Color(61, 75, 105));
    
    std::ostringstream stream;
    
    int nbRows = 19;
    unsigned int characterSize = static_cast<unsigned int>(m_screenHeigth * 0.98f / (nbRows * 1.5f));
    unsigned int outlineThickness = 6;
    float lineSpacing = 1.55f * m_screenHeigth / 800.f;
    
    if (m_programCounter < m_opcodesDisplayBegining || m_programCounter - m_opcodesDisplayBegining >= (nbRows - 1) * 2) {
        m_opcodesDisplayBegining = m_programCounter - 6;
    }
    
    for (int adress = m_opcodesDisplayBegining; adress < m_opcodesDisplayBegining + nbRows * 2; adress += 2) {
        stream << std::hex << static_cast<int>(adress);
        stream << " - ";
        
        std::uint16_t opcode = getOpcodeAt(adress);
        int opcodeId = static_cast<int>(getActionFromOpcode(opcode));
        if (0 <= opcodeId && opcodeId < NB_OPCODES_AVAILABLES) {
            std::string mnemonnic {m_opcodeIdentifiers[opcodeId].mnemonic};
            
            auto delimIt = std::find(mnemonnic.begin(), mnemonnic.end(), ' ');
            std::string sub = std::string(mnemonnic.begin(), delimIt);
            stream << sub;
            
            for (int delta = 0; delta < 4 - sub.size(); ++delta)
                stream << " "; // Adds spaces to uniform values manipulated display
            
            std::uint8_t b3 = (opcode >> 8) & 0xF, b2 = (opcode >> 4) & 0xF, b1 = opcode & 0xF;
            
            std::string toParse = std::string(delimIt, mnemonnic.end());
            for (int i = 0; i < toParse.size(); ++i) {
                switch (toParse[i]) {
                    case 'I':
                        stream << static_cast<int>(m_registerAdress);
                        break;
                    case 'X':
                        stream << static_cast<int>(b3);
                        break;
                    case 'Y':
                        stream << static_cast<int>(b2);
                        break;
                    case 'N':
                    {
                        int addr = b3;
                        if (++i < toParse.size() && toParse[i] == 'N') {
                            addr = (addr << 4) + b2;
                            if (++i < toParse.size() && toParse[i] == 'N')
                            {
                                addr = (addr << 4) + b1;
                                ++i;
                            }
                        }
                        --i;
                        stream << "#" << static_cast<int>(addr);
                    }
                        break;
                    default:
                        stream << toParse[i];
                        break;
                }
            }
        }
        
        stream << "\n";
    }
    
    sf::Text text(stream.str(), m_defaultFont);
    text.setCharacterSize(characterSize);
    text.setLineSpacing(lineSpacing);
    text.setFillColor(sf::Color::White);
    text.setPosition(20.f, 6.f);
    
    sf::RectangleShape highlightShape {};
    highlightShape.setSize(sf::Vector2f(subViewWidth - outlineThickness * 2, characterSize * 1.5f - outlineThickness * 2));
    highlightShape.setFillColor(sf::Color::Red);
    highlightShape.setOutlineColor(sf::Color(255, 128, 0));
    highlightShape.setOutlineThickness(outlineThickness);
    highlightShape.setPosition(outlineThickness, subViewHeight / nbRows * ((m_programCounter - m_opcodesDisplayBegining) / 2) + 6.f);
    texture->draw(highlightShape);
    
    texture->draw(text);
    
    texture->display();
    return texture;
    
}

std::unique_ptr<sf::RenderTexture> Chip8::displayMemory() {
    float subViewWidth = m_screenWidth * 0.4f;
    float subViewHeight = m_screenHeigth * 0.7f;
    
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(subViewWidth, subViewHeight);
    texture->clear(sf::Color(38, 52, 82));
    
    const float characterSize = 28.f;
    
    std::stringstream stream;
    
    for (int registerId = 0; registerId < 11; ++registerId) {
        stream << "V" << std::hex << registerId << " = #";
        if (m_registers[registerId] < 16) {
            stream << "0";
        }
        stream << static_cast<int>(m_registers[registerId]) << "    ";
        
        int value = -1;
        int zeroRefValue = -1;
        
        if (registerId < 4) {
            stream << "V" << std::hex << registerId + 11 << " = #";
            if (m_registers[registerId + 11] < 16) {
                stream << "0";
            }
            stream << static_cast<int>(m_registers[registerId]) << "\n";
            continue;
        } else if (registerId == 5) {
            stream << "GC";
            value = m_gameCounter;
            zeroRefValue = 16;
        } else if (registerId == 6) {
            stream << "SC";
            value = m_soundCounter;
            zeroRefValue = 16;
        } else if (registerId == 8) {
            stream << "I ";
            value = m_registerAdress;
            zeroRefValue = 4096;
        } else if (registerId == 9) {
            stream << "PC";
            value = m_programCounter;
            zeroRefValue = 4096;
        } else if (registerId == 10) {
            stream << "SL";
            value = m_stackLevel;
            zeroRefValue = 16;
        }
        
        if (value != -1) {
            stream << " = #";
            if (value < zeroRefValue)
                stream << "0";
            stream << std::hex << value;
        }
        
        stream << "\n";
    }
        
    sf::Text text;
    text.setFont(m_defaultFont);
    text.setCharacterSize(characterSize);
    text.setFillColor(sf::Color::White);
    text.setPosition(10.f, 7.f);
    text.setString(stream.str());
    text.setLineSpacing((1.8f - (m_screenHeigth <= 700 ? 0.1f : 0.f)) * m_screenHeigth / 800.f);
    
    texture->draw(text);
    
    texture->display();
    return texture;
}

std::unique_ptr<sf::RenderTexture> Chip8::displayDebugInfos() {
    float subViewWidth = m_screenWidth;
    float subViewHeight = m_screenHeigth * 0.7f;
    
    auto texture {std::make_unique<sf::RenderTexture>()};
    texture->create(subViewWidth, subViewHeight);
    texture->clear(sf::Color(59, 81, 128));
    
    std::ostringstream stream;
    
    stream << "\n" << "Chip 8" << "\n\n\n";
    
    stream << "Config file: " << m_configFilename << "\n\n";
    stream << "Game file: " << m_gameFilename << "\n\n";
    stream << "Sound file: " << m_soundFilename << "\n\n";
    stream << "Font file: " << m_fontFilename << "\n\n";
    
    stream << "\n";
    
    stream << "fps: " << (m_isPaused ? m_fps : std::round(1.f / m_displayTimer.getElapsedTime().asSeconds())) << "\n";
    
    stream << "\n\n";
    
    stream << "Pause  : " << ExtendedInputs::getKeyName(m_controlKeys[16]) << "\n";
    stream << "Reload : " << ExtendedInputs::getKeyName(m_controlKeys[17]) << "\n";
    stream << "Jump   : " << ExtendedInputs::getKeyName(m_controlKeys[18]) << "\n";
    
    sf::Text text(stream.str(), m_defaultFont);
    text.setCharacterSize(25);
    text.setLineSpacing(1.1f * m_screenHeigth / 800.f);
    text.setFillColor(sf::Color::White);
    text.setPosition(20.f, 0.f);
    
    texture->draw(text);
    
    texture->display();
    return texture;
}

std::uint16_t Chip8::getOpcodeAt(std::uint16_t adress) {
    return ((m_memory[adress] << 8) + m_memory[adress + 1]);
}

std::uint16_t Chip8::getCurrentOpcode() {
    return getOpcodeAt(m_programCounter);
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
    
    if (actionId >= NB_OPCODES_AVAILABLES)
        throw std::runtime_error("Action id " + std::to_string(actionId) + " could not be recognised neither handled");
    
    m_actions[actionId](b1, b2, b3);
}

void Chip8::clearScreen() {
    std::fill(m_pixels.begin(), m_pixels.end(), false);
}

void Chip8::drawSprite(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3) {
    m_registers[0xF] = 0;
    
    for (int dY = 0; dY < b1; ++dY) {
        const std::uint8_t rowDescription = m_memory[m_registerAdress + dY];
        const int y = (m_registers[b2] + dY) % m_height;
        for (int bitId = 0; bitId < 8; ++bitId) {
            bool pixelOn = rowDescription & (0x1 << (7 - bitId));
            if (!pixelOn)
                continue;
            const int x = (m_registers[b3] + bitId) % m_width;
            if (m_pixels[y * m_width + x]) {
                m_registers[0xF] = 1;
            }
            m_pixels[y * m_width + x] = !m_pixels[y * m_width + x];
        }
    }
}

}
