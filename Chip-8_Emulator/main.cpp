//
//  main.cpp
//  Chip-8_Emulator
//
//  Created on 22/01/2020.
//

#include <iostream>

#include "Chip8.hpp"

#include <stdio.h>
#include <filesystem>
using namespace std;
using namespace filesystem;

int main(int argc, char ** argv) {
    
//    std::string filename = "BC_Chip8Test/BC_test.ch8";
    std::string filename = "Games/Games/Breakout.ch8";
//    std::string filename = "Games/Games/AIRPLANE.ch8";
    
    chp::Chip8 emulator;
    emulator.launch(1400, 800, filename);
    
    return 0;
}
