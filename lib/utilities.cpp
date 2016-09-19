
#include "utilities.h"
#include <iostream>
#include <fstream>

#include <cstring>

//Whole process works with recursive calls to ensure proper order
//Most to least important (called in opposite order):
//    Parenthesis
//    closures
//    Concatenation
//    Or

std::string utilities::infix2Postfix(std::string infixRegex){
    //convert to char string
    m_preRegEx = const_cast<char*>(infixRegex.c_str());
    m_currentPreProcChar = *(m_preRegEx++);
    std::cout << m_preRegEx << std::endl;
    std::cout << m_currentPreProcChar << std::endl;
    processOr();
    std::cout << "Infix: " << infixRegex << std::endl;
    std::cout << "postFix: " << m_postFixRegex << std::endl;

    return m_postFixRegex;
}

int utilities::processOr(){
    processConcat();
    while (m_currentPreProcChar == '|'){
        m_currentPreProcChar = *(m_preRegEx++);
        processConcat();
        m_postFixRegex += "|";
    }
    return 0;
}
int utilities::processConcat() {
    processClosure();
    while (strchr(")|*+?", m_currentPreProcChar) == NULL){
        processClosure();
        m_postFixRegex+="&";
    }
    return 0;
}

int utilities::processClosure() {
    processLiteral();
    while (m_currentPreProcChar != '\0' && strchr("*+?", m_currentPreProcChar) != NULL){
        m_postFixRegex += m_currentPreProcChar;
        m_currentPreProcChar = *(m_preRegEx++);
    }
    return 0;
}

//Processes the actual literals
int utilities::processLiteral() {
    //for end of line
    if (m_currentPreProcChar=='\0'){
        return -1;
    }
    //checks if special char is end of line
    if (m_currentPreProcChar=='\\'){
        m_currentPreProcChar = *(m_preRegEx++);
        if(m_currentPreProcChar=='\0'){
            return -1;
        }
        m_postFixRegex += m_currentPreProcChar;
        m_currentPreProcChar = *(m_preRegEx++);
    }
    //if current character not ()|*+?
    else if (strchr("()|*+?", m_currentPreProcChar) == NULL){
        //if it's a concats
        if (m_currentPreProcChar == '&'){
            m_postFixRegex += "\\&";
        }
        else {
            //else it's a literal, so just add it
            m_postFixRegex += m_currentPreProcChar;
        }
        m_currentPreProcChar = *(m_preRegEx++);
    }
    //if it is open parenthesis, process what is inside
    else if (m_currentPreProcChar == '('){
        m_currentPreProcChar = *(m_preRegEx++);
        processOr();
        //if there is no close parenthesis, fail
        if (m_currentPreProcChar != ')'){
            return -1;
        }
        m_currentPreProcChar = *(m_preRegEx++);
    }
    else {
        return -1;
    }
    return 0;
}
