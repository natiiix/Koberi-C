//
//  koberi-c.hpp
//  koberi-c
//
//  Created by Filip Peterek on 31/10/2016.
//  Copyright © 2016 Filip Peterek. All rights reserved.
//

#ifndef koberi_c_hpp
#define koberi_c_hpp

/* Most of these are most likely included already, but whatever */
/*                                                              */
/*                    ( [compiler do-magic] )                   */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <cctype>
#include <array>

#include "tokenizer.hpp"
#include "expressions.hpp"
#include "print.hpp"
#include "exceptions.hpp"
#include "token.hpp"
#include "syntax.hpp"
#include "parser.hpp"
#include "ast.hpp"

#include "debug_macro_definitions.h"


class KoberiC {
    
    std::vector<token> _tokens;
    Tokenizer _tokenizer;
    Parser _parser;
    AbstractSyntaxTree _ast;
    
public:
    
    KoberiC();
    
    void tokenize(const std::string & filename);
    void parse();
    
    void test();
    
};

#endif /* koberi_c_hpp */
