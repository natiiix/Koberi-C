//
//  main.cpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#include "koberi-c.hpp"

#ifdef DEBUG_COMPILE_TEST
static const std::string TEST = "test.koberice";
#endif

int main(int argc, const char * argv[]) {
    
#ifndef DEBUG_COMPILE_TEST
    
    /* Parse command line arguments */
    
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    std::string file = "";
    
    for (auto & a : args) {
        if (a == "-v") {
            expr::setVerbose(true);
        }
        else if (file == "") {
			file = a;
        }
		else {
            std::cout << "Unknown parameter: " << a << std::endl;
            return -1;
		}
    }
    
	if (file == "") {
        std::cout << "Error: No file provided. Compilation terminated." << std::endl;
        return -1;
    }

#endif
    
#ifdef SET_VERBOSE
    /* Verbose mode is automatically on when this macro is defined. Used for debugging purposes. */
    expr::setVerbose(true);
#endif
    
    KoberiC kc;
    
    try {
        /* If this macro is defined, compile a testing file, don't parse cl args, used for debugging */
#ifdef DEBUG_COMPILE_TEST
        kc.compile(TEST);
#else
        kc.compile(file);
#endif
    } catch (std::exception & e) {
        std::cout << e.what() << "\n" << std::endl;
        std::cout << "Compilation terminated." << std::endl;
    }
    
#ifdef PRINT_TOKENS
    kc.test();
#endif
    
    return 0;
    
}

