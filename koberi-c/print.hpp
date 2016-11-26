//
//  print.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef print_h
#define print_h

#include <iostream>

template <class T>
void print(const T & param, std::ostream & os = std::cout) {
    os << param << std::endl;
}


#endif /* print_h */
