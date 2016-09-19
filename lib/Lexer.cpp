
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <time.h>

#include "Lexer.h"
#include "utilities.h"


#define EPSILON       -1
#define CLOSURE       '*'
#define PLUS_CLOSURE  '+'
#define OPTIONAL      '?'
#define OPEN_PAREN    '('
#define CLOSE_PAREN   ')'
#define OR            '|'
#define EXPLICIT_CONCAT  '&'


Lexer::Lexer() {

}
Lexer::~Lexer() {

}

//Create automata over a given regex string
bool Lexer::construct(std::string strRegex){
    //converts square brackets to parenthesis containing explicit values
    while( strRegex.find("[") != std::string::npos){
        strRegex = bracketPreProcessing(strRegex);
    }

    //if unable to construct NFA, return false
    if (!constructNFA(strRegex)){
        return false;
    }

    //get NFA table and set last state to accepting state
    popTable(m_NFATable);
    m_NFATable[m_NFATable.size()-1] -> m_acceptingState = true;

    //run conversion to DFA
    convertNFAtoDfa();
    return true;

}

//converts a regex to NFA
bool Lexer::constructNFA(std::string strRegex) {
    //convert infix string to postfix
    utilities utils;
    m_postRegex = utils.infix2Postfix(strRegex);
    //push literals onto stack
    for(int i=0; i<(int)m_postRegex.size(); ++i){
        char currentChar = m_postRegex[i];
        if( !isOperator(currentChar)){
            pushOnCharStack(currentChar);
        }
        //When operator found, realize proper action
        else {
            switch (currentChar){
                case CLOSURE:
                    closure();
                    break;
                case PLUS_CLOSURE:
                    closurePlus();
                    break;
                case OPTIONAL:
                    closureOptional();
                    break;
                case OR:
                    Or();
                    break;
                case EXPLICIT_CONCAT:
                    concat();
                    break;
            }
        }
    }
    return true;
}

// Returns true if current character is an operator
bool Lexer::isOperator(char inputChar) {
    return(( inputChar == CLOSURE )        ||
           ( inputChar == OR )             ||
           ( inputChar == OPEN_PAREN )     ||
           ( inputChar == CLOSE_PAREN )    ||
           ( inputChar == PLUS_CLOSURE )   ||
           ( inputChar == EXPLICIT_CONCAT) ||
           ( inputChar == OPTIONAL ));
}

//pushes new character into automata using Thompson method
void Lexer::pushOnCharStack(char inputChar) {
    AutomataState *s0 = new AutomataState(m_nextStateId++);
    AutomataState *s1 = new AutomataState(m_nextStateId++);

    //transition between two new states using character
    s0 -> createTransition(inputChar, s1);
    table subTable;
    subTable.push_back(s0);
    subTable.push_back(s1);
    m_charClassStack.push(subTable);
    //add character to inputset
    m_inputSet.insert(inputChar);
}

//pops last element from table
bool Lexer::popTable(table &NFATable) {
    if(m_charClassStack.size()>0){
        NFATable = m_charClassStack.top();
        m_charClassStack.pop();
        return true;
    }
    return false;
}

// OPERATORS------------------------------------------------------------------------------------------------------------
//take last element, add epsilon transitions from start to end and end to start
bool Lexer::closure() {
    table prevTable;
    if (!popTable(prevTable)){
        return false;
    }
    AutomataState *table0 = new AutomataState(m_nextStateId++);
    AutomataState *table1 = new AutomataState(m_nextStateId++);
    table0 -> createTransition(EPSILON, table1);
    table0 -> createTransition(EPSILON, (*(prevTable.begin())));
    (*(prevTable.rbegin())) -> createTransition(EPSILON, table1);
    (*(prevTable.rbegin())) -> createTransition(EPSILON, (*(prevTable.begin())));

    prevTable.insert(prevTable.begin(), table0);
    prevTable.push_back(table1);

    m_charClassStack.push(prevTable);
    return true;

}

//add epsilon closure from end of last element to its start
bool Lexer::closurePlus() {
    table prevTable;
    if(!popTable(prevTable)){
        return false;
    }
    AutomataState *table0 = new AutomataState(m_nextStateId++);
    AutomataState *table1 = new AutomataState(m_nextStateId++);

    table0 -> createTransition(EPSILON, (*(prevTable.begin())));
    (*(prevTable.rbegin())) -> createTransition(EPSILON, table1);
    (*(prevTable.rbegin())) -> createTransition(EPSILON, (*(prevTable.begin())));

    prevTable.insert(prevTable.begin(), table0);
    prevTable.push_back(table1);
    m_charClassStack.push(prevTable);

    return true;

}

//add episolon transition from start to end of last element("jumping" last element)
bool Lexer::closureOptional() {
    table prevTable;
    if(!popTable(prevTable)){
        return false;
    }
    AutomataState *table0 = new AutomataState(m_nextStateId++);
    AutomataState *table1 = new AutomataState(m_nextStateId++);

    table0 -> createTransition(EPSILON, table1);
    table0 -> createTransition(EPSILON, (*(prevTable.begin())));
    (*(prevTable.rbegin())) -> createTransition(EPSILON, table1);

    prevTable.insert(prevTable.begin(), table0);
    prevTable.push_back(table1);
    m_charClassStack.push(prevTable);

    return true;
}

//add transitions to last to elements, enabling and reclosing a "fork"
bool Lexer::Or() {
    table table0, table1;

    if(!popTable(table0) || !popTable(table1)){
        return false;
    }

    AutomataState *nTable0 = new AutomataState(m_nextStateId++);
    AutomataState *nTable1 = new AutomataState(m_nextStateId++);

    nTable0 -> createTransition(EPSILON, (*(table0.begin())));
    nTable0 -> createTransition(EPSILON, (*(table1.begin())));
    (*(table0.rbegin())) -> createTransition(EPSILON, nTable1);
    (*(table1.rbegin())) -> createTransition(EPSILON, nTable1);

    table1.push_back(nTable1);
    table0.insert(table0.begin(), nTable0);
    table0.insert(table0.end(), table1.begin(), table1.end());
    m_charClassStack.push(table0);

    return true;
}

//transition directly between last two elements
bool Lexer::concat() {
    table table0, table1;

    if(!popTable(table1) || !popTable(table0)){
        return false;
    }

    (*(table0.rbegin())) -> createTransition(EPSILON, (*(table1.begin())));
    table0.insert(table0.end(), table1.begin(), table1.end());
    m_charClassStack.push(table0);
    return true;
}

//given an input and current state, update current state to new states when a transition available
void Lexer::move(char charInput, std::set<AutomataState *> NFAState, std::set<AutomataState *> &Res) {
    Res.clear();
    StateIterator iterator;
    //get transition from each state and add new state to results
    for (iterator =NFAState.begin(); iterator!=NFAState.end(); ++iterator){
        table states;
        (*iterator) -> getTransition(charInput, states);
        for (int i=0; i<(int)states.size(); ++i){
            Res.insert(states[i]);
        }
    }
}

//epsilon closure on current states, writing to results
void Lexer::epsilonClosure(std::set<AutomataState *> startStates, std::set<AutomataState *> &Res) {
    std::stack<AutomataState*> unvistedStates;
    Res.clear();
    Res = startStates;
    StateIterator iterator;
    //push all current states onto unvisitedStates stack
    for (iterator=startStates.begin(); iterator!=startStates.end(); ++iterator){
        unvistedStates.push(*iterator);
    }
    //while unvisitedStates still exist
    while(!unvistedStates.empty()){
        //visit all espilon transitions from an unvisited state
        AutomataState* currentState = unvistedStates.top();
        unvistedStates.pop();
        table epsilonStates;
        currentState -> getTransition(EPSILON, epsilonStates);
        tableIterator epsilonIterator;
        //if found states not already found, add them to unvisited and results
        for(epsilonIterator=epsilonStates.begin(); epsilonIterator!=epsilonStates.end(); ++epsilonIterator){
            if (Res.find(*epsilonIterator)==Res.end()){
                Res.insert(*epsilonIterator);
                unvistedStates.push(*epsilonIterator);
            }
        }
    }
}

// NFA -> DFA ----------------------------------------------------------------------------------------------------------

void Lexer::convertNFAtoDfa() {
  //clean current table
    for (int i=0; i<(int)m_DFATable.size(); ++i){
        delete m_DFATable[i];
    }
    m_DFATable.clear();

    if (m_NFATable.size()==0){
        return;
    }

    //Start states of DFA are the espilon closure of NFA
    std::set<AutomataState*> NFAStartStates;
    NFAStartStates.insert(m_NFATable[0]);
    std::set<AutomataState*> DFAStartStates;
    epsilonClosure(NFAStartStates, DFAStartStates);
    //save as first state
    m_nextStateId = 0;
    AutomataState *DFAStartState = new AutomataState(DFAStartStates, m_nextStateId++);

    //add first state to new dfa table
    m_DFATable.push_back(DFAStartState);

    //add first state to unvisitedStates
    table unvisitedStates;
    unvisitedStates.push_back(DFAStartState);

    //while some states still unvisited...
    while (!unvisitedStates.empty()){
        AutomataState* currentDFAState = unvisitedStates[unvisitedStates.size()-1];
        unvisitedStates.pop_back();
        std::set<char>::iterator iterator;
        //iterate over the inputSet, moving and epsilonClosuring each current state
        for (iterator=m_inputSet.begin(); iterator!=m_inputSet.end(); ++iterator){
            std::set<AutomataState*> moveResult, epsilonClosureResult;
            move(*iterator, currentDFAState->getNFAState(), moveResult);
            epsilonClosure(moveResult, epsilonClosureResult);

            StateIterator moveResultIterator;
            StateIterator epsilonClosureIterator;

            bool alreadyExists = false;

            //check if resulting state already exists, if so just pass
            AutomataState *s = NULL;
            for (int i = 0; i < (int)m_DFATable.size(); ++i) {
                s = m_DFATable[i];
                if (s->getNFAState()==epsilonClosureResult){
                    alreadyExists = true;
                    break;
                }
            }
            //otherwise, save as a new state with transition of current iterator
            if(!alreadyExists){
                AutomataState* newState = new AutomataState(epsilonClosureResult, m_nextStateId++);
                unvisitedStates.push_back(newState);
                m_DFATable.push_back(newState);
                currentDFAState -> createTransition(*iterator, newState);
            }
            else
            {
                //if it does already exist, add the new transition to the current state
                currentDFAState -> createTransition(*iterator, s);
            }
        }
    }
    reduceDFA();
}

//removes any dead ends that get left from transition
void Lexer::reduceDFA() {
    std::set<AutomataState*> deadEnds;
    for (int i = 0; i < (int)m_DFATable.size(); ++i) {
        if (m_DFATable[i]-> isDeadEnd()){
            deadEnds.insert(m_NFATable[i]);
        }
    }
    if (deadEnds.empty()){
        return;
    }
    StateIterator iterator;
    for (iterator=deadEnds.begin(); iterator!=deadEnds.end(); ++iterator){
        for (int i = 0; i < (int)m_DFATable.size(); ++i) {
            m_DFATable[i] -> removeTransition(*iterator);
        }
        tableIterator position;
        for (position=m_DFATable.begin(); position!=m_DFATable.end(); ++position){
            if (*position == *iterator){
                m_DFATable.erase(position);
                delete (*iterator);
                break;
            }
        }

    }
}



// Simulate ------------------------------------------------------------------------------------------------------------
void getTime(clock_t Start, std::string type){
    clock_t End = clock();
    double elapsedTime = double(End - Start) / CLOCKS_PER_SEC;
    elapsedTime = elapsedTime * 1000000;

    std::cout << type << " Simulation Execution Time: \t" << elapsedTime <<"micro seconds" << std::endl;
}
// Simulate DFA
// Fails on no transition available or if final state not an accepting state
bool Lexer::simulateDFA(std::string strText) {
    clock_t Start = clock();
    m_strText = strText;
    AutomataState *pState = m_DFATable[0];
    std::vector<AutomataState*> transition;
    for (int i = 0; i < (int) m_strText.size(); ++i) {
        char currentChar = m_strText[i];
        pState ->getTransition(currentChar, transition);
        if (transition.empty()){
            getTime(Start, "DFA");
            return false;
        }
        pState = transition[0];

    }
    if (pState->m_acceptingState){
        getTime(Start, "DFA");
        return true;
    }
    getTime(Start, "DFA");
    return false;

}

//moves and espilon closes on input
//fails on empty table or if results don't contain an accepting state
bool Lexer::simulataNFA(std::string strText) {
    clock_t Start = clock();
    if (m_NFATable.size()==0){
        return false;
    }
    std::set<AutomataState*> startStates;
    startStates.insert(m_NFATable[0]);

    std::set<AutomataState*> currentStates;

    epsilonClosure(startStates, currentStates);

    std::string::iterator iterator;
    for (iterator=strText.begin(); iterator!=strText.end(); ++iterator){
        std::set<AutomataState*> previousStates = currentStates;
        move(*iterator, previousStates, currentStates);
        if (currentStates.empty() ){
            getTime(Start, "NFA");
            return false;
        }
        std::set<AutomataState*> previousEpsilonStates = currentStates;
        epsilonClosure(previousEpsilonStates, currentStates);
    }
    StateIterator finalStateIterator;

    for (finalStateIterator = currentStates.begin(); finalStateIterator != currentStates.end(); ++finalStateIterator){
        if ((*finalStateIterator)->m_acceptingState){
            getTime(Start, "NFA");
            return true;
        }
    }
    getTime(Start, "NFA");
    return false;




}

// Private Methods Go Here ---------------------------------------------------------------------------------------------

//replaces [X] and [x0-xn] with (x0|x1|...|xn)
std::string Lexer::bracketPreProcessing(std::string strRegEx){
    std::string::size_type startPos, endPos, separatorPos;
    std::string ReplacedStrRegEx;

    startPos = strRegEx.find_first_of("[");
    endPos   = strRegEx.find_first_of("]");
    separatorPos = strRegEx.find_first_of("-");

    if ( startPos == std::string::npos || endPos == std::string::npos )
        return strRegEx;

    //Replacing with (
    ReplacedStrRegEx += strRegEx.substr( 0, startPos );
    ReplacedStrRegEx.push_back('(');
    std::string result = strRegEx.substr( startPos + 1, endPos - startPos - 1);
    char first = result[0];
    char last  = result[result.size() - 1 ];

    //if not a range, just returns elements with OR operator
    if ( separatorPos != std::string::npos ) {
        while ( first != last ) {
            ReplacedStrRegEx.push_back(first);
            ReplacedStrRegEx += "|";
            first++;
        }
        ReplacedStrRegEx.push_back(first);
    } else {
        //this baby works with ranges because of implementing over ascii values
        startPos++;
        while ( startPos != endPos - 1) {
            ReplacedStrRegEx.push_back(strRegEx[startPos]);
            ReplacedStrRegEx += "|";
            startPos++;
        }
        ReplacedStrRegEx.push_back(strRegEx[endPos - 1]);
    }
    ReplacedStrRegEx += strRegEx.substr( endPos + 1, strRegEx.size() - endPos );
    ReplacedStrRegEx += ")";

    return ReplacedStrRegEx;
}


//output automata
void Lexer::printAutomata(table &table) {
    std::string tableString;
    bool initialState = true;
    tableString+= "Acceptance States: \n";
    for (int i = 0; i < (int)table.size(); ++i) {
        AutomataState *pState = table[i];
        if(pState-> m_acceptingState){
            tableString+= "\t state: " + pState->getStringId() + "\t\n";
        }
    }
    tableString += "\n";
    for (int i = 0; i < (int)table.size(); ++i) {
        AutomataState *pState = table[i];
        std::vector<AutomataState*> State;
        pState -> getTransition(EPSILON, State);
        for (int j=0; j< (int)State.size(); ++j){
            if (j==0 && initialState){
                tableString += "Initial State: \n \tstate: "+ pState->getStringId() + "\n \n";
                initialState = false;
            }
            tableString += "\t" + pState->getStringId() + "->" + State[j]->getStringId();
            tableString += "\t[Label = \"epsilon \"]\n";
        }
        std:: set<char>::iterator iterator;
        for(iterator = m_inputSet.begin(); iterator!=m_inputSet.end(); ++iterator){
            pState -> getTransition(*iterator, State);
            for (int j=0; j< (int)State.size(); ++j){
                tableString += "\t" + pState->getStringId() + "->" + State[j]->getStringId();
                std::stringstream out;
                out << *iterator;
                tableString += "\t[Label = \"" + out.str() + " \"]\n";
            }
        }
    }
    std::cout << tableString;
}

void Lexer::writeAutomata(table &table, std::string filename) {

    std::ofstream outputFile;
    outputFile.open(filename.c_str());



    std::string tableString;
    bool initialState = true;
    tableString+= "Acceptance States: \n";
    for (int i = 0; i < (int)table.size(); ++i) {
        AutomataState *pState = table[i];
        if(pState-> m_acceptingState){
            tableString+= "\t state: " + pState->getStringId() + "\t\n";
        }
    }
    tableString += "\n";
    for (int i = 0; i < (int)table.size(); ++i) {
        AutomataState *pState = table[i];
        std::vector<AutomataState*> State;
        pState -> getTransition(EPSILON, State);
        for (int j=0; j< (int)State.size(); ++j){
            if (j==0 && initialState){
                tableString += "Initial State: \n \tstate: "+ pState->getStringId() + "\n \n";
                initialState = false;
            }
            tableString += "\t" + pState->getStringId() + "->" + State[j]->getStringId();
            tableString += "\t[Label = \"epsilon \"]\n";
        }
        std:: set<char>::iterator iterator;
        for(iterator = m_inputSet.begin(); iterator!=m_inputSet.end(); ++iterator){
            pState -> getTransition(*iterator, State);
            for (int j=0; j< (int)State.size(); ++j){
                tableString += "\t" + pState->getStringId() + "->" + State[j]->getStringId();
                std::stringstream out;
                out << *iterator;
                tableString += "\t[Label = \"" + out.str() + " \"]\n";
            }
        }
    }

    outputFile << tableString << std::endl;
    outputFile.close();
}

void Lexer::writeNFA() {
    writeAutomata(m_NFATable, "nfaAutomata.txt");
}

void Lexer::writeDFA(){
    writeAutomata(m_DFATable, "dfaAutomata.txt");
}


void Lexer::printNFA() {
    printAutomata(m_NFATable);
}

void Lexer::printDFA(){
    printAutomata(m_DFATable);
}
