
#ifndef PROYECT1_UTILITIES_H
#define PROYECT1_UTILITIES_H

#include <string>
#include <iostream>
#include <fstream>


class utilities {
private:
    char* m_preRegEx;
    char m_currentPreProcChar;
    std::string m_postFixRegex;

    int processOr();
    int processConcat();
    int processClosure();
    int processLiteral();
public:
    std::string infix2Postfix(std::string infixRegEx);

};


#endif //PROYECT1_UTILITIES_H
