//
//  Chip8.hpp
//  Chip-8_Emulator
//
//  Created on 22/01/2020.
//

#ifndef Chip8_hpp
#define Chip8_hpp

# include <iostream>
# include <string>
# include <array>
# include <vector>
# include <algorithm>
# include <memory>
# include <fstream>
# include <limits>

# include <stdlib.h>
# include <time.h>

# include <SFML/Graphics.hpp>
# include <SFML/Audio.hpp>

# include "ExternalLibrairies/TSL/hopscotch_map.h"

# include "Parser.hpp"

namespace chp {

class Chip8 {
public:
    Chip8() = default;
    ~Chip8() = default;
    
    Chip8(Chip8 && other) = delete;
    Chip8(Chip8 const& other) = delete;
    Chip8 operator=(Chip8 const& other) = delete;
    
    void launch(std::string const& configFilename);
    
private:
    void init(std::string const& configFilename);
    void loadConfig(std::string const& configFilename);
    void loadFont();
    bool loadFile(std::string fileName);
    void loadOpcodes();
    void loadActions();
    
    void handleKey(sf::Keyboard::Key key, bool keyPressed);
    
    void update();
    std::unique_ptr<sf::RenderTexture> display();
    
    std::uint16_t getCurrentOpcode();
    std::uint8_t getActionFromOpcode(std::uint16_t opcode);
    void computeAction(std::uint8_t actionId, std::uint16_t opcode);
    
    void clearScreen();
    void drawSprite(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3);
    
private:
    static constexpr std::size_t NB_OPCODES_AVAILABLES = 35;
    
    std::string m_configFilename;
    
    std::size_t m_memorySize = 4096;
    std::size_t m_memoryBegin = 512;
    std::size_t m_maxStackSize = 15;
    
    unsigned int m_windowWidth = 1400;
    unsigned int m_windowHeight = 800;
    std::size_t m_width = 64;
    std::size_t m_height = 32;
    
    int m_fps = 60;
    int m_frequency = 250; // Hz
    
    std::vector<std::uint8_t> m_memory;
    std::vector<std::uint16_t> m_stack;
    
    std::array<std::uint8_t, 16> m_registers;
    std::array<bool, 16> m_keyPressed;
    
    std::vector<bool> m_pixels;
    
    std::uint16_t m_programCounter;
    std::uint8_t m_stackLevel;
    std::uint16_t m_registerAdress;
    
    std::uint8_t m_gameCounter;
    std::uint8_t m_soundCounter;
    
    struct OpcodeIdentifier {
        std::uint16_t mask;
        std::uint16_t identifier;
    };
    std::array<OpcodeIdentifier, NB_OPCODES_AVAILABLES> m_opcodeIdentifiers;
    
    std::array<std::function<void(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3)>, NB_OPCODES_AVAILABLES> m_actions;
    
    sf::Music m_defaultSound;
    sf::Font m_defaultFont;
    
};


}


#endif /* Chip8_hpp */
