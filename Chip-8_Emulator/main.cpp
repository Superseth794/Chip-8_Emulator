//
//  main.cpp
//  Chip-8_Emulator
//
//  Created on 22/01/2020.
//

#include <iostream>

#include "Chip8.hpp"

int main(int argc, const char * argv[]) {
    
    chp::Chip8 emulator;
    emulator.init();
    emulator.launch();
    
    return 0;
}
