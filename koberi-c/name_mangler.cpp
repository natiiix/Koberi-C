//
//  name_mangler.cpp
//  koberi-c
//
//  Created by Filip Peterek on 23/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "name_mangler.hpp"

const std::string NameMangler::prefix = "_koberic_";

std::string NameMangler::mangleName(const std::string & name, const std::vector<parameter> & params) {
    
    std::string mangledName = prefix + name;
    
    for (const auto & param : params) {
        
        mangledName.append("_" + param.type);
        
    }
    
    return mangledName;
    
    
}

std::string NameMangler::mangleName(const std::string & name, const std::vector<std::string> & paramTypes) {
    
    std::string mangledName = prefix + name;
    
    for (const auto & param : paramTypes) {
        
        mangledName.append("_" + param);
        
    }
    
    return mangledName;
    
}

std::string NameMangler::premangleMethodName(const std::string & name, const std::string & className) {
    
    std::string premangledName = className + "_m_" + className;
    return premangledName;
    
}
