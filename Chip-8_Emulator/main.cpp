//
//  main.cpp
//  Chip-8_Emulator
//
//  Created on 22/01/2020.
//

#include <string>

#include "Chip8.hpp"

int main(int argc, char ** argv) {
    
    std::string configFilename = "Config.txt";
    
    if (argc > 1)
        configFilename = argv[1];
    
    chp::Chip8 emulator;
    emulator.launch(configFilename);
    
    return 0;
}
