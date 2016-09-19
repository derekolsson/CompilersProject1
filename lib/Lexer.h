
#ifndef PROYECT1_LEXER_H
#define PROYECT1_LEXER_H

#include <string>
#include <stack>
#include <iostream>
#include <fstream>
#include "AutomataState.h"
struct NFA{

};

struct DFA{

};


class Lexer {

public:
    Lexer();
    ~Lexer();
    bool construct(std::string strRegex);
    bool simulateDFA(std::string strText);
    bool simulataNFA(std::string strText);
    typedef std::vector<AutomataState*> table;
    typedef table::reverse_iterator tableReverIterator;
    typedef table::iterator tableIterator;
    typedef std::set<AutomataState*>::iterator StateIterator;
    void printNFA();
    void printDFA();
    void writeDFA();
    void writeNFA();


private:
    table m_NFATable;
    table m_DFATable;
    std::string m_postRegex;
    std::stack<table> m_charClassStack;
    std::stack<char> m_expressionStack;
    std::set<char> m_inputSet;
    std::string m_strText;
    std::string m_infixStr;
    int m_nextStateId;

    std::string bracketPreProcessing(std::string strRegex);

    bool constructNFA(std::string strRegex);
    void pushOnCharStack(char inputChar);
    bool popTable(table &NFATable);
    bool concat();
    bool closure();
    bool closureOptional();
    bool closurePlus();
    bool Or();
    bool isOperator(char inputChar);
    bool isOpenParen(char inputChar);
    bool isCloseParen(char inputChar);
    void epsilonClosure(std::set<AutomataState*> startStates, std::set<AutomataState*> &Res);
    void NFAepsilonClosure(std::set<AutomataState*> currentStates, std::set<AutomataState*> &Res);
    void move(char charInput, std::set<AutomataState*> NFAState, std::set<AutomataState*> &Res);
    void convertNFAtoDfa();
    void reduceDFA();
    void destroy();

    void minimizeDFA();

    void printAutomata(table &table);
    void writeAutomata(table &table, std::string filename);









};


#endif //PROYECT1_LEXER_H
