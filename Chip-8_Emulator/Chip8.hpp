//
//  Chip8.hpp
//  Chip-8_Emulator
//
//  Created by Jaraxus on 22/01/2020.
//

#ifndef Chip8_hpp
#define Chip8_hpp

# include <iostream>
# include <array>
# include <memory>
# include <SFML/Graphics.hpp>

namespace chp {

class Chip8 {
public:
    Chip8() = default;
    ~Chip8() = default;
    
    Chip8(Chip8 && other) = delete;
    Chip8(Chip8 const& other) = delete;
    Chip8 operator=(Chip8 const& other) = delete;
    
    void launch(unsigned int width, unsigned int height);
    
private:
    void init();
    void update();
    std::unique_ptr<sf::RenderTexture> display();
    
    void clearScreen();
    
    std::uint16_t getCurrentOpcode();
    std::uint8_t getActionFromOpcode(std::uint16_t opcode);
    void computeAction(std::uint8_t actionId);
    
private:
    
    static constexpr int WIDTH = 64;
    static constexpr int HEIGHT = 32;
    float m_pixelWidth = 0;
    float m_pixelHeight = 0;
    
    int m_fps = 60;
    int m_frequency = 250; // Hz
    
    std::array<std::uint8_t, 4096> m_memory;
    std::array<std::uint8_t, 16> m_registers;
    std::array<std::uint16_t, 16> m_stack;
    
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
