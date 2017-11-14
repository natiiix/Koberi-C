//
//  AnalyzedAbstractSyntaxTree.hpp
//  koberi-c
//
//  Created by Filip Peterek on 30/10/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef AnalyzedAbstractSyntaxTree_hpp
#define AnalyzedAbstractSyntaxTree_hpp

#include <stdio.h>
#include <vector>

#include "aast_node.hpp"

class AnalyzedAbstractSyntaxTree {
    
    std::vector<AASTClass *> _classes;
    std::vector<AASTDeclaration *> _globalDeclarations;
    std::vector<AASTFunction *> _functions;
    
public:
    
    void emplaceClass(AASTClass * c);
    void emplaceGlobalDeclaration(AASTDeclaration * declaration);
    void emplaceFunction(AASTFunction * function);
    
    const std::vector<AASTClass *> & getClasses();
    const std::vector<AASTDeclaration *> & getDeclarations();
    const std::vector<AASTFunction *> & getFunctions();
    
    ~AnalyzedAbstractSyntaxTree();
    
};

#endif /* AnalyzedAbstractSyntaxTree_hpp */
