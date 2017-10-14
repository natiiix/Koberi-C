//
//  translator.cpp
//  koberi-c
//
//  Created by Filip Peterek on 22/07/2017.
//  Copyright © 2017 Filip Peterek. All rights reserved.
//

#include "translator.hpp"

Translator::Translator(TraversableAbstractSyntaxTree & ast) : _ast(ast) {
    
}

void Translator::libraries() {
    
    _output << "/* Libraries */" << "\n\n";
    
    _output << "#include <stdio.h>" << "\n";
    _output << "#include <stdlib.h>" << "\n";
    _output << "#include <time.h>" << "\n";
    _output << "#include <math.h>" << "\n";
    _output << "#include <string.h>" << "\n";
    
}

void Translator::typedefs() {
    
    _output << "\n" << "/* Typedefs */" << "\n\n";
    
    _output << "typedef double num;" << "\n";
    _output << "typedef intmax_t ll;" << "\n";
    _output << "typedef uintmax_t ull;" << "\n";
    _output << "typedef unsigned char uchar;" << "\n";
    
}

void Translator::translateClasses() {
    
    const std::unordered_map<std::string, _class> & classes = _ast.getClasses();
    const std::vector<std::string> & classOrder = _ast.getClassOrder();
    
    _output << "\n" << "/* Classes */" << "\n";
    
    for (const auto & c : classOrder) {
        
        const _class & cls = classes.at(c);
        
        _output << "\n" << "typedef struct " << cls.className << " {\n";
        
        for (const auto & attr : cls.attributes) {
            
            checkIdIsValid(attr.name);
            _output << INDENT << translateType(attr.type) << " " << attr.name << ";\n";
        }
        
        _output << "} " << cls.className << ";\n" << std::endl;
        
    }
    
}

void Translator::translateGlobalVars() {
    
    const size_t nodeCount = _ast.getNodeCount();
    
    _output << "\n" << "/* Global Variables */" << "\n" << "\n";
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Declaration) {
            
            ASTDeclaration * decl = (ASTDeclaration*)node;
            _output << translateType(decl->type) << " " << decl->name << ";\n";
            
        }
        
    }
    
    std::endl(_output);
    
}

void Translator::forwardFunctionDeclarations() {
    
    const size_t nodeCount = _ast.getNodeCount();
    
    _output << "\n" << "/* Function Declarations */" << "\n" << "\n";
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Function) {
            
            ASTFunction * fun = (ASTFunction*)node;
            
            std::string mangledName = NameMangler::mangleName(fun->name, fun->parameters);
            if (fun->className != "") {
                mangledName = NameMangler::premangleMethodName(mangledName, fun->className);
            }
            _output << translateType(fun->type) << " " << mangledName << "(";
            
            if (fun->className != "") {
                _output << fun->className << " * self";
                if (fun->parameters.size()) {
                    _output << ", ";
                }
            }
            
            if (fun->parameters.size()) {
                
                for (size_t iter = 0; iter < fun->parameters.size(); ++iter) {
                    
                    const parameter & param = fun->parameters[iter];
                    _output << translateType(param.type) << " " << param.name
                            << (iter == fun->parameters.size() - 1 ? "" : ", ");
                    
                }
                
            }
            else if (fun->className == ""){
                _output << "void";
            }
            
            _output << ");\n";
            
        }
        
    }
    
    std::endl(_output);
    
}

void Translator::translateFunctions() {
    
    ASTNode * node;
    
    for (size_t i = 0; i < _ast.getNodeCount(); ++i) {
        
        node = _ast.getNodePtr(i);
        
        if (node->nodeType == NodeType::Function) {
            
            ASTFunction & function = *((ASTFunction*)node);
            translateFunction(function);
            
        }
        
    }
    
}

void Translator::main() {
    
    _output << "\n" << "/* C Main Function */" << "\n" << "\n";
    
    _output << "int main(int argc, const char * argv[]) {" << "\n";
    _output << INDENT << "return " << NameMangler::mangleName("main", std::vector<parameter>()) << "();" << "\n";
    _output << "}" << std::endl;
    
}

void Translator::setOutputFile(const std::string & filename) {
    
    _output = std::ofstream(filename);
    
    if (not _output) {
        throw file_not_created(filename);
    }
    
}

void Translator::translate() {
    
#ifdef OUTPUT_GLOBAL_SCOPE_NODES
    test();
#endif
    
    kobericMainCheck();
    
    libraries();
    typedefs();
    translateClasses();
    translateGlobalVars();
    forwardFunctionDeclarations();
    translateFunctions();
    
    main();
    
}

void Translator::kobericMainCheck() {
    
    std::string type;
    
    try {
        /* If main doesn't exist, exception is thrown */
        type = _ast.getFunctionReturnType(NameMangler::mangleName("main", std::vector<parameter>()));
    }
    catch (const std::exception & e) {
        throw missing_main();
    }
    
    /* If main exists, check return type */
    if (type != "int") {
        throw invalid_main();
    }
    
}

void Translator::translateFunction(ASTFunction & function) {
    
    _functionName = function.name;
    
    const std::vector<parameter> & params = function.parameters;
    
    std::string name = NameMangler::mangleName(function.name, params);
    
    if (function.className != "") {
        name = NameMangler::premangleMethodName(name, function.className);
    }
    
    _output << "\n" << translateType(function.type) << " " << name << "(";
    
    /* If function is a member function, pass pointer to self as first parameter */
    if (function.className != "") {
        _output << function.className << " * self";
        if (params.size()) {
            _output << ", ";
        }
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        
        _output << translateType(params[i].type) << " " << params[i].name
                << (i == params.size() - 1 ? "" : ", ");
        
    }
    
    _output << ")";
    
    translateScope(function.childNodes);
    
}

parameter Translator::translateFunCall(ASTFunCall & funcall) {
    
    parameter functionCall;
    std::string name = funcall.function;
    
    /* Only used if function is a member function   */
    /* It needs to be declared at this scope though */
    method m;
    
    /* Casting needs to be handled separately, because it contains class names */
    if (name == "cast" and funcall.parameters.size() == 2 and funcall.object == nullptr) {
        
        if (funcall.parameters[1]->nodeType == NodeType::Variable and
            _ast.isDataType(((ASTVariable*)funcall.parameters[1])->name)) {
            
            parameter param = (getFuncallParameter(funcall.parameters[0]));
            std::string type = ((ASTVariable*)funcall.parameters[1])->name;
            
            return cast(param, type);
            
        }
    
    }
    
    std::vector<parameter> params;
    
    for (auto & param : funcall.parameters) {
        params.emplace_back(getFuncallParameter(param));
    }
    
    for (auto & param : params) {
        if (param.type == "") {
            throw invalid_parameter(_functionName, funcall.function, param.value);
        }
        if (param.value == "self") {
            param.value = "(*self)";
        }
    }
    
    if (expr::isOperator(name) and funcall.object == nullptr) {
        return translateOperator(name, params);
    }
    if (name == "print" and funcall.object == nullptr) {
        return translatePrint(params);
    }
    if (name == "c" and funcall.object == nullptr) {
        return inlineC(params);
    }
    
    /* Mangle name and get return type, if function doesn't exist, and exception should be thrown */
    /* If function exists, create valid C function call from provided parameters                  */
    name = NameMangler::mangleName(name, params);
    
    /* Translate parameters first */
    functionCall.value = "(";
    
    if (funcall.object != nullptr) {
        
        parameter object = translateMemberAccess(*funcall.object);
        
        try {
            m = _ast.getMethodReturnType(name, object.type);
        } catch (const undeclared_function_call & e) {
            throw undeclared_function_call(funcall.function, object.type);
        }
        
        /* If method is a member of objects superclass, cast object to that superclass */
        /* Otherwise just access the objects address                                   */
        std::string castedObject = "&(" + object.value + ")";
        if (m.className != object.type) {
            castedObject = "((" + m.className + " *)((void *) " + castedObject + "))";
        }
        functionCall.value += castedObject + (params.size() ? ", " : "");
        functionCall.type = m.type;
        
        
    } else {
        functionCall.type = _ast.getFunctionReturnType(name);
    }
    
    for (size_t i = 0; i < params.size(); ++i) {
        
        functionCall.value += params[i].value + (i == params.size() - 1 ? "" : ", ");
        
    }
    
    functionCall.value += ")";
    
    /* After translating the arguments, prepend mangled name in front of them */
    if (funcall.object != nullptr) {
        name = NameMangler::premangleMethodName(name, m.className);
    }
    
    functionCall.value = name + functionCall.value;
    
    return functionCall;
    
}

parameter Translator::translatePrint(std::vector<parameter> & parameters) {
    
    parameter print;
    
    std::vector<std::string> printStatements = expr::print(parameters);
    
    /* If no parameters are passed print \n */
    if (not printStatements.size()) {
        return parameter("puts(\"\")", "");
    }
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        
        const parameter & statement = printStatements[i];
        
        if (not i) {
            
            print.value += statement.value;
            
        } else {
            
            for (unsigned short il = 0; il < _indentLevel; ++il) {
                print.value += INDENT;
            }
            
            print.value += statement.value;
            
        }
        
        if (i != parameters.size() - 1) {
            print.value += ";\n";
        }
        
    }
    
    return print;
    
}

parameter Translator::inlineC(std::vector<parameter> & params) {
    
    parameter c;
    
    std::vector<std::string> cStatements = expr::inlineC(params);
    
    /* If no parameters are passed do nothing */
    if (not cStatements.size()) {
        return parameter("", "");
    }
    
    for (size_t i = 0; i < cStatements.size(); ++i) {
        
        const std::string & statement = cStatements[i];
        
        if (not i) {
            
            c.value += statement;
            
        } else {
            
            for (unsigned short il = 0; il < _indentLevel; ++il) {
                c.value += INDENT;
            }
            
            c.value += statement;
            
        }
        
        if (i != params.size() - 1) {
            c.value += "\n";
        }
        
    }
    
    return c;
    
}

parameter Translator::translateOperator(std::string & op, std::vector<parameter> & params) {
    
    const bool isParamless  = expr::isParameterlessOperator(op);
    const bool isUnary      = expr::isUnaryOperator(op);
    const bool isBinary     = expr::isBinaryOperator(op);
    
    if (isParamless) {
        
        if (params.size() and not (isUnary or isBinary)) {
            throw invalid_call(op, _functionName, " Too many parameters");
        }
        
        if (not params.size()) {
            return expr::parameterless_operator(op);
        }
        
    }
    
    if (isUnary) {
        
        if (not params.size()) {
            throw invalid_call(op, _functionName, " Too few parameters");
        } else if (params.size() > 1 and not isBinary) {
            throw invalid_call(op, _functionName, " Too many parameters");
        }
        
        if (params.size() == 1) {
            return expr::unaryOperator(params[0], op);
        }
        
    }
    
    if (isBinary) {
        
        if (params.size() < 2) {
            throw invalid_call(op, _functionName, " Too few parameters");
        }
        
        return expr::binaryOperator(params, op);
        
    }
    
    /* Should never occur */
    return parameter(op, "");
    
}

parameter Translator::cast(const parameter & valueToCast, const std::string & type) {
    
    parameter castedObject;
    
    if (valueToCast.type == type) {
        castedObject.type = valueToCast.type;
        castedObject.value = valueToCast.value;
    }
    else if (expr::isNumericalType(type) and expr::isNumericalType(valueToCast.type)) {
        castedObject.type = type;
        castedObject.value = "((" + translateType(type) + ")" + valueToCast.value + ")";
    }
    /* Check if classes are related, doesn't matter which one inherits from which */
    else if (_ast.hasSuperclass(valueToCast.type, type) or
             _ast.hasSuperclass(type, valueToCast.type)) {
        
        castedObject.type = type;
        
        /* Access the object's address */
        castedObject.value = "&(" + valueToCast.value + ")";
        /* Cast address to void* */
        castedObject.value = "((void *)" + castedObject.value + ")";
        /* Cast void* to desired_type* */
        castedObject.value = "((" + type + "*)" + castedObject.value + ")";
        /* Dereference pointer and access value at pointer */
        castedObject.value = "(*" + castedObject.value + ")";
        
    }
    else {
        throw invalid_cast(valueToCast.type, type);
    }
    
    return castedObject;
    
}

parameter Translator::getFuncallParameter(ASTNode * node) {
    
    if (node->nodeType == NodeType::FunCall) {
        
        ASTFunCall * funcall = (ASTFunCall*)node;
        
        /* Recurse if funcall is passed as a parameter to a funcall */
        parameter fcall = translateFunCall(*funcall);
        
        return fcall;
        
    } else if (node->nodeType == NodeType::Variable) {
        
        ASTVariable & variable = *((ASTVariable*)node);
        
        /* Get var name and type */
        parameter var = getVariable(variable);
        
        return var;
        
    } else if (node->nodeType == NodeType::Literal) {
        
        ASTLiteral & literal = *((ASTLiteral*)node);
        
        /* Get literal type and value */
        
        parameter lit;
        
        lit.type = literal.type;
        
        /* If literal is a character literal, surround character literal with single quotes ' */
        if (literal.type == "char") {
            lit.value = "\'" + literal.value + "\'";
        } else {
            lit.value = literal.value;
        }
        
        return lit;
        
    } else if (node->nodeType == NodeType::MemberAccess) {
        
        ASTMemberAccess & attribute = *((ASTMemberAccess*)node);
        return translateMemberAccess(attribute);
        
    }
    
    /* A lazy solution, it will throw but display no useful info */
    return parameter();
    
}

void Translator::translateScope(std::vector<ASTNode *> scopeNodes) {
    
    _output << " {" << "\n";
    ++_indentLevel;
    
    for (ASTNode * node : scopeNodes) {
        translateFunctionNode(node);
    }
    
    --_indentLevel;
    indent();
    _output << "}" << "\n" << std::endl;
    
    
}

void Translator::translateFunctionNode(ASTNode * node) {
    
    if (node->nodeType == NodeType::FunCall) {
        
        ASTFunCall * funcall = (ASTFunCall*)node;
        parameter fcall = translateFunCall(*funcall);
        
        indent();
        _output << fcall.value << ";\n";
        
    } else if (node->nodeType == NodeType::Construct) {
        
        ASTConstruct * construct = (ASTConstruct*)node;
        translateConstruct(*construct);
        
    } else if (node->nodeType == NodeType::Declaration) {
        
        ASTDeclaration * declaration = (ASTDeclaration*)node;
        std::string declAsStr = translateDeclaration(*declaration);
        
        indent();
        _output << declAsStr << "\n";
        
        
    } else {
        
        throw invalid_statement(_functionName);
        
    }
    
}

void Translator::translateConstruct(ASTConstruct & construct) {
    
    std::string constructStr;
    
    if (construct.construct == "if" or construct.construct == "elif" or construct.construct == "while") {
        constructStr = translateIfWhile(construct);
    }
    else if (construct.construct == "else") {
        constructStr = translateElse(construct);
    }
    
    indent();
    _output << constructStr;
    
    translateScope(construct.childNodes);
    
    
}

std::string Translator::translateIfWhile(ASTConstruct & construct) {
    
    std::string _if = construct.construct + " (";
    
    
    // parameter condition = translateFunCall(construct.condition);
    
    parameter condition;
    
    if (construct.condition->nodeType == NodeType::Literal) {
        
        ASTLiteral & lit = *((ASTLiteral*)construct.condition);
        condition = parameter(lit.value, lit.type);
        
    }
    else if (construct.condition->nodeType == NodeType::MemberAccess) {
    
        ASTMemberAccess & attr = *((ASTMemberAccess*)construct.condition);
        condition = translateMemberAccess(attr);
        
    }
    else if (construct.condition->nodeType == NodeType::Variable) {
        
        ASTVariable & var = *((ASTVariable*)construct.condition);
        condition = parameter(var.name, _ast.getVarType(var.name, var.parentScope));
        
    }
    else {
        
        ASTFunCall & fcall = *((ASTFunCall*)construct.condition);
        condition = translateFunCall(fcall);
        
    }
    
    if (not expr::isNumericalType(condition.type)) {
        throw bad_type("Error: Construct conditions must be of numerical type. ");
    }
    
    _if += condition.value;
    
    _if += ") ";
    
    return _if;
    
}

std::string Translator::translateElse(ASTConstruct & construct) {
    
    /* Well, nothing more complicated is really needed, the function only exists to keep things consistent */
    std::string _else = "else ";
    return _else;
    
}

std::string Translator::translateDeclaration(ASTDeclaration & declaration) {
    
    std::string decl;
    
    checkIdIsValid(declaration.name);
    
    decl = translateType(declaration.type) + " " + declaration.name;
    
    if (declaration.value != nullptr) {
        
        if (declaration.value->nodeType == NodeType::FunCall) {
            
            ASTFunCall * funcall = (ASTFunCall*)declaration.value;
            
            parameter fcall = translateFunCall(*funcall);
            
            if ((not (expr::isNumericalType(declaration.type) and expr::isNumericalType(fcall.type)))
                and declaration.type != fcall.type) {
                throw type_mismatch("Error: Type mismatch in declaration of variable ("
                                    + declaration.type + " " + declaration.name + "). Expected: "
                                    + declaration.type +
                                    " Got: " + fcall.type);
            }
            
            decl += " = " + fcall.value;
            
        } else if (declaration.value->nodeType == NodeType::Variable) {
            
            ASTVariable & variable = *((ASTVariable*)declaration.value);
            
            parameter var = getVariable(variable);
            
            if ((not (expr::isNumericalType(declaration.type) and expr::isNumericalType(var.type)))
                and declaration.type != var.type) {
                throw type_mismatch("Error: Type mismatch in declaration of variable ("
                                    + declaration.type + " " + declaration.name + "). Expected: "
                                    + declaration.type +
                                    " Got: " + var.type);
            }
            
            decl += " = " + var.name;
            
        } else if (declaration.value->nodeType == NodeType::Literal) {
            
            ASTLiteral & literal = *((ASTLiteral*)declaration.value);
            
            if ((not (expr::isNumericalType(declaration.type) and expr::isNumericalType(literal.type)))
                and declaration.type != literal.type) {
                throw type_mismatch("Error: Type mismatch in declaration of variable ("
                                    + declaration.type + " " + declaration.name + "). Expected: "
                                    + declaration.type +
                                    " Got: " + literal.type);
            }
            
            decl += " = " + literal.value;
            
        } else if (declaration.value->nodeType == NodeType::MemberAccess) {
            
            ASTMemberAccess & attribute = *((ASTMemberAccess*)declaration.value);
            decl += " = " + translateMemberAccess(attribute).value;
            
        }
        
    }
    
    decl += ";";
    
    _ast.emplaceVariableIntoScope(parameter(declaration.name, declaration.type), declaration.parentScope);
    
    return decl;
    
}

parameter Translator::translateMemberAccess(ASTMemberAccess & attribute) {
    
    if (attribute.accessOrder.size() < 1) {
        throw invalid_attribute_access(_functionName, "Not enough parameters.");
    }
    
    parameter attr;
    
    parameter baseVar;
    
    if ((*attribute.accessOrder[0]).nodeType == NodeType::FunCall) {
        
        ASTFunCall & fcall = *((ASTFunCall*)attribute.accessOrder[0]);
        baseVar = translateFunCall(fcall);
        
    } else {
    
        baseVar.name = ((ASTVariable*)attribute.accessOrder[0])->name;
        baseVar.type = _ast.getVarType(baseVar.name, attribute.parentScope);
        
    }
    
    if (attribute.accessOrder.size() > 1) {
        attr.type = checkAttributesAndReturnType(baseVar, attribute.accessOrder);
    } else {
        attr.type = baseVar.type;
    }
    
    attr.value += (baseVar.name == "self" ? "(*self)" : baseVar.name) + ".";
    
    for (size_t i = 1; i < attribute.accessOrder.size(); ++i) {
        
        std::string & a = ((ASTVariable*)attribute.accessOrder[i])->name;
        
        attr.value += a + ".";
    
    }
    
    attr.value.pop_back();
    
    return attr;
    
}

std::string Translator::checkAttributesAndReturnType(parameter & var, std::vector<ASTNode*> & attributes, unsigned int iter) {
    
    /* Probably unnecessary */
    /* if (iter != attributes.size() - 1) {
        checkIsClass(var.type);
    } */
    
    checkIsClass(var.type);
    
    std::string & attribute = ((ASTVariable*)attributes[iter])->name;
    
    const _class & cls = _ast.getClass(var.type);
    
    if (not cls.hasVar(attribute)) {
        throw invalid_attribute_access(_functionName, "Class " + cls.className + " has no attribute " + attribute);
    }
    
    std::string type = cls.getVarType(attribute);
    
    if (iter == attributes.size() - 1) {
        return type;
    }
    
    parameter baseVar(attribute, type);
    
    return checkAttributesAndReturnType(baseVar, attributes, iter + 1);
    
}

parameter Translator::getVariable(ASTVariable & variable) {
    
    parameter var;
    
    var.name = variable.name;
    var.type = _ast.getVarType(variable.name, variable.parentScope);
    
    return var;
    
}

std::string Translator::translateType(const std::string & type) {
    
    std::string t = type;
    
    if (type == "int") {
        t = "ll";
    } else if (type == "uint") {
        t = "ull";
    }
    
    return t;
    
}

void Translator::checkIsClass(std::string & className) {
    
    if (not _ast.isClass(className)) {
        throw invalid_attribute_access(_functionName, className + " is not a class.");
    }
    
}

void Translator::checkIdIsValid(const std::string & id) {
    
    auto isLetterOrUnderscore = [] (const char c) -> bool {
        return ('A' <= c and c <= 'Z') or ('a' <= c and c <= 'z') or c == '_';
    };
    
    auto isValidIdChar = [&] (const char c) -> bool {
        return isLetterOrUnderscore(c) or ('0' <= c and c <= '9');
    };
    
    if (not isLetterOrUnderscore(id[0])) {
        throw invalid_identifier(id);
    }
    
    for (size_t i = 1; i < id.length(); ++i) {
        
        if (not isValidIdChar(id[i])) {
            throw invalid_identifier(id);
        }
        
    }
    
}

void Translator::indent() {
    
    for (unsigned short i = 0; i < _indentLevel; ++i) {
        _output << INDENT;
    }
    
}

void Translator::test() {
    
    size_t nodeCount = _ast.getNodeCount();
    
    for (size_t i = 0; i < nodeCount; ++i) {
        
        ASTNode * node = _ast.getNodePtr(i);
        
        switch (node -> nodeType) {
                
            case NodeType::Variable: {
                ASTVariable * var = (ASTVariable*)node;
                std::cout << "Variable: " << var->name << std::endl;
                break;
            }
                
            case NodeType::Literal: {
                ASTLiteral * lit = (ASTLiteral*)node;
                std::cout << "Literal: " << lit->type << ": " << lit->value << std::endl;
                break;
            }
                
            case NodeType::Scope: {
                ASTScope * scope = (ASTScope*)node;
                std::cout << "Scope: nodes: " << scope->childNodes.size() << "vars: " << scope->vars.size() << std::endl;
                break;
            }
                
            case NodeType::FunCall: {
                ASTFunCall * funcall = (ASTFunCall*)node;
                std::cout << "Funcall: " << funcall->function << std::endl;
                break;
            }
                
            case NodeType::Declaration: {
                ASTDeclaration * decl = (ASTDeclaration*)node;
                std::cout << "Declaration: " << decl->type << " " << decl->name << std::endl;
                break;
            }
                
            case NodeType::Function: {
                ASTFunction * fun = (ASTFunction*)node;
                std::cout << "Function: " << fun->type << " " << fun->name << "(";
                for (auto & i : fun->parameters) {
                    std::cout << i.type << " " << i.name << ",";
                }
                std::cout << ")" << std::endl;
                break;
            }
                
            case NodeType::Construct: {
                ASTConstruct * con = (ASTConstruct*)node;
                std::cout << "Construct: " << con->construct << std::endl;
                break;
            }
                
            case NodeType::MemberAccess: {
                std::cout << "Member access: " << "too lazy, shouldn't happen anyway" << std::endl;
                break;
            }
                
            default:
                
                break;
                
        }
        
    }
    
}
