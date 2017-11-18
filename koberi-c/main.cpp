//
//  main.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"

static const std::string TEST = "testing_script.koberice";

int main(int argc, const char * argv[]) {
    
#ifndef DEBUG_COMPILE_TEST
    if (argc < 2) {
        std::cout << "Error: No file provided. Compilation terminated. " << std::endl;
        return -1;
    }
    
    std::string file = argv[1];
#endif
    
    KoberiC ks;
    
    try {
#ifdef DEBUG_COMPILE_TEST
        ks.compile(TEST);
#else
        ks.compile(file);
#endif
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
    }
    
#ifdef PRINT_TOKENS
    ks.test();
#endif
    
     // */
     
    return 0;
    
}
