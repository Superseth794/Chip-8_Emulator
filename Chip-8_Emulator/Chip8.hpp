//
//  Chip8.hpp
//  Chip-8_Emulator
//
//  Created by Jaraxus on 22/01/2020.
//

#ifndef Chip8_hpp
#define Chip8_hpp

# include <iostream>
# include <string>
# include <array>
# include <memory>
# include <fstream>
# include <limits>

# include <stdlib.h>
# include <time.h>

# include <SFML/Graphics.hpp>

# include "ExternalLibrairies/TSL/hopscotch_map.h"

namespace chp {

class Chip8 {
public:
    Chip8() = default;
    ~Chip8() = default;
    
    Chip8(Chip8 && other) = delete;
    Chip8(Chip8 const& other) = delete;
    Chip8 operator=(Chip8 const& other) = delete;
    
    void launch(unsigned int width, unsigned int height, std::string fileName);
    
private:
    void init();
    void loadFont();
    bool loadFile(std::string fileName);
    
    void update();
    std::unique_ptr<sf::RenderTexture> display();
    
    std::uint16_t getCurrentOpcode();
    std::uint8_t getActionFromOpcode(std::uint16_t opcode);
    void computeAction(std::uint8_t actionId, std::uint16_t opcode);
    
    void clearScreen();
    void drawSprite(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3);
    
private:
    
    static constexpr std::size_t MEMORY_SIZE = 4096;
    static constexpr std::size_t MEMORY_BEGIN = 512;
    static constexpr std::size_t MAX_STACK_SIZE = 15;
    
    static constexpr int WIDTH = 64;
    static constexpr int HEIGHT = 32;
    float m_pixelWidth = 0;
    float m_pixelHeight = 0;
    
    int m_fps = 60;
    int m_frequency = 250; // Hz
    
    std::array<std::uint8_t, MEMORY_SIZE> m_memory;
    std::array<std::uint8_t, 16> m_registers;
    std::array<std::uint16_t, MAX_STACK_SIZE> m_stack;
    
    std::array<bool, WIDTH * HEIGHT> m_pixels;
    
    std::uint16_t m_programCounter;
    std::uint8_t m_stackLevel;
    std::uint16_t m_registerAdress;
    
    std::uint8_t m_gameCounter;
    std::uint8_t m_soundCounter;
    
    struct OpcodeIdentifier {
        std::uint16_t mask;
        std::uint16_t identifier;
    };
    std::array<OpcodeIdentifier, 35> m_opcodeIdentifiers;
    
};


}


#endif /* Chip8_hpp */
