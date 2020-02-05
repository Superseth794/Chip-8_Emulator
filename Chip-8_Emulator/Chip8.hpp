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
# include <cmath>

# include <stdlib.h>
# include <time.h>

# include <SFML/Graphics.hpp>
# include <SFML/Audio.hpp>

# include "ExternalLibrairies/TSL/hopscotch_map.h"

# include "Parser.hpp"
# include "InputNames.hpp"

namespace chp {

/**
 \brief Class to represent Chip8 emulator
 */
class Chip8 {
public:
    
    /**
     \brief Default constructor
     */
    Chip8() = default;
    
    /**
     \brief Default destructor
     */
    ~Chip8() = default;
    
    /**
     \brief Deleted move constructor
     */
    Chip8(Chip8 && other) = delete;
    
    /**
     \brief Deleted copy constructor
     */
    Chip8(Chip8 const& other) = delete;
    
    /**
     \brief Deleted assignement operator
     */
    Chip8 operator=(Chip8 const& other) = delete;
    
    /**
     \brief Launches emulator
     \details The emulator emulates a chip8 environment and provides additional infos. All the configuration deatils must be provided in the configuration file provided
     \param configFilename String storing the path of the configuration file where to find the configuration parameters
     */
    void launch(std::string const& configFilename);
    
private:
    /**
     \brief Initialises emulator
     \details Loads configuration and resources. Setups Chip8 system.
     \param configFilename String storing the path of the configuration file where to find the configuration parameters
     */
    void init(std::string const& configFilename);
    
    /**
     \brief Loads configuration stored in file specified
     \param configFilename String storing the path of the configuration file where to find the configuration parameters
     */
    void loadConfig(std::string const& configFilename);
    
    /**
     \brief Loads font from filename specified in config file
     */
    void loadFont();
    
    /**
     \brief Loads game file
     \details Loads in Chip8 memory the content of game file specified
     \param fileName String storing the path of game file
     */
    bool loadFile(std::string fileName);
    
    
    /**
     \brief Loads all Chip8 opcodes and stores them in a structure
     */
    void loadOpcodes();
    
    /**
     \brief Loads actions associted to all Chip8 opcodes and stores them in a structure
     */
    void loadActions();
    
    
    /**
     \brief Loads input keys from configuration file
     \param parser Parser pointing on the configuration file
     */
    void loadInputsKeys(Parser & parser);
    
    /**
     \brief Handles given key and processes action if needed
     \param key Key to handle
     \param keyPressed True if key given is being pressed, false otherwise
     */
    void handleKey(sf::Keyboard::Key key, bool keyPressed);
    
    
    /**
     \brief Updates emulator (update frequency is specified in configuration file as "update_frequency")
     */
    void update();
    
    
    /**
     \brief Displays emulator window
     \return Ptr on texture storing the emulator window texture
     */
    std::unique_ptr<sf::RenderTexture> display();
    /**
     \brief Displays Chip8 emulator screen
     \return Ptr on texture storing the Chip8 emulator pixels
     */
    std::unique_ptr<sf::RenderTexture> displayScreen();
    /**
     \brief Displays opcodes debugging window
     \details Display is only made if parameter "show_opcodes" is set true in configuration file
     \return Ptr on texture storing the debugging window
     */
    std::unique_ptr<sf::RenderTexture> displayOpcodes();
    /**
    \brief Displays memory debugging window
    \details Display is only made if parameter "show_memory" is set true in configuration file
    \return Ptr on texture storing the debugging window
    */
    std::unique_ptr<sf::RenderTexture> displayMemory();
    /**
    \brief Displays debug infos window
    \details Display is only made if parameter "show_infos" is set true in configuration file
    \return Ptr on texture storing the infos window
    */
    std::unique_ptr<sf::RenderTexture> displayDebugInfos();
    
    
    /**
     \brief Gets opcode located at given position in memory
     \param adress Adress of the opcode to get
     \return Opcode read in memory
     */
    std::uint16_t getOpcodeAt(std::uint16_t adress);
    /**
     \brief Gets opcode located at program counter position in memory
     \return Opcode read in memory
     */
    std::uint16_t getCurrentOpcode();
    /**
     \brief Action id associated to the given opcode
     \param opcode The opcode to get the related action
     \return The id of the action
     */
    std::uint8_t getActionFromOpcode(std::uint16_t opcode);
    /**
     \brief Computes action of given id with given opcode as parameter
     \param actionId Id of action to compute
     \param opcode Opcode to use as parameter for given action
     */
    void computeAction(std::uint8_t actionId, std::uint16_t opcode);
    
    
    /**
     \brief Clears Chip8 emulator screen
     */
    void clearScreen();
    /**
     \brief Draws sprite in Chip8 screen according to paramters given
     \param b1 Four least significant bits
     \param b2 Four middle bits
     \param b3 Four strong bits
     */
    void drawSprite(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3);
    
private:
    static constexpr std::size_t NB_OPCODES_AVAILABLES = 35;
    static constexpr std::size_t NB_CONTROLS_AVAILABLES = 19;
    
    std::string m_configFilename;
    std::string m_gameFilename = "Games/Games/PONG.ch8";
    std::string m_soundFilename = "Resources/Sounds/sfx_sounds_high2.wav";
    std::string m_fontFilename = "Resources/Fonts/ArcadeClassic/ARCADECLASSIC.TTF";
    
    std::size_t m_memorySize = 4096;
    std::size_t m_memoryBegin = 512;
    std::size_t m_maxStackSize = 15;
    
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    unsigned int m_screenWidth = 1600;
    unsigned int m_screenHeigth = 800;
    std::size_t m_width = 64;
    std::size_t m_height = 32;
    
    int m_fps = 60;
    int m_frequency = 250; // Hz
    
    std::vector<std::uint8_t> m_memory;
    std::vector<std::uint16_t> m_stack;
    
    std::array<std::uint8_t, 16> m_registers;
    
    std::array<sf::Keyboard::Key, NB_CONTROLS_AVAILABLES> m_controlKeys;
    std::array<bool, 16> m_keyPressed;
    
    std::vector<bool> m_pixels;
    
    std::uint16_t m_programCounter;
    std::uint8_t m_stackLevel;
    std::uint16_t m_registerAdress;
    
    std::uint8_t m_gameCounter;
    std::uint8_t m_soundCounter;
    
    bool m_isPaused = false;
    bool m_doSingleJump = false;
    
    bool m_showDebugInfos = false;
    bool m_showOpcodes = false;
    bool m_showMemory = false;
    
    std::uint16_t m_opcodesDisplayBegining;
    
    struct OpcodeIdentifier {
        std::string opcode;
        std::uint16_t mask;
        std::uint16_t identifier;
        std::string mnemonic;
    };
    std::array<OpcodeIdentifier, NB_OPCODES_AVAILABLES> m_opcodeIdentifiers;
    
    std::array<std::function<void(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3)>, NB_OPCODES_AVAILABLES> m_actions;
    
    sf::Music m_defaultSound;
    sf::Font m_defaultFont;
    
    sf::Clock m_executionTimer;
    sf::Clock m_displayTimer;
    
};


}


#endif /* Chip8_hpp */
