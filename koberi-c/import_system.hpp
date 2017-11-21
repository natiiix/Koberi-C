//
//  import_system.hpp
//  koberi-c
//
//  Created by Filip Peterek on 14/09/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#ifndef import_system_hpp
#define import_system_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

#include "contains.hpp"
#include "exceptions.hpp"
#include "syntax.hpp"

class ImportSystem {
    
    std::vector<std::string> _importedFiles;
    std::vector<std::string> _externTypes;
    
    std::vector<std::string> _cLibs = { "<stdio.h>", "<stdlib.h>", "<time.h>", "<math.h>", "<string.h>", "<stdint.h>" };
    
    bool isImported(const std::string & filename);
    
    void importType(const std::string & type);
    void importLib(const std::string & lib);
    
    void importInto(std::vector<std::string> & collection, const std::string & value);
    
    std::vector<std::string> importFiles(const std::vector<std::string> & files);
    std::vector<std::string> importTypes(const std::vector<std::string> & types);
    std::vector<std::string> importLibraries(const std::vector<std::string> & libs, bool isStdLib = false);
    
public:
    
    void parseImports(const std::string & filename);
    
    void appendExtensions();
    
    const std::vector<std::string> & getImportedFiles();
    std::vector<std::string> & getExternTypes();
    std::vector<std::string> & getCLibs();
    
};

#endif /* import_system_hpp */
