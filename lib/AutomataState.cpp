
#include "AutomataState.h"
#include <sstream>
#include <iostream>

AutomataState::~AutomataState() {
    m_NFAStates.clear();
    m_transition.clear();
}

//create new automata state with current states, id, accepting state and groupid
AutomataState::AutomataState(std::set<AutomataState *> NFAState, int sID) {
    m_NFAStates = NFAState;
    m_stateId = sID;
    m_acceptingState = false;
    m_groupId = 0;
    stateIterator iterator;
    //if state contains accepting state, it IS an accepting state
    for (iterator = NFAState.begin(); iterator!= NFAState.end(); ++iterator) {
        if((*iterator) -> m_acceptingState){
            m_acceptingState = true;
        }
    }
}
//copies a state
AutomataState::AutomataState(const AutomataState &other) {
    *this = other;
}

//transition is a char, nextStatePoint pair
void AutomataState::createTransition(char inputChar, AutomataState *pState) {
    m_transition.insert(std::make_pair(inputChar, pState));
}

//erases transitions to a state
void AutomataState::removeTransition(AutomataState *pState) {
    std::multimap<char, AutomataState*>::iterator iterator;
    for (iterator = m_transition.begin(); iterator!= m_transition.end();){
        AutomataState *toState = iterator -> second;

        if (toState == pState){
            m_transition.erase(iterator++);
        }
        else {
            ++iterator;
        }
    }
}

//returns a transition
void AutomataState::getTransition(char inputChar, table &States) {
    States.clear();
    std::multimap<char, AutomataState*>::iterator iterator;
    for (iterator = m_transition.lower_bound(inputChar); iterator != m_transition.upper_bound(inputChar); ++iterator){
        AutomataState *pState = iterator -> second;
        States.push_back(pState);
    }
}

//returns current states
std::set<AutomataState*>& AutomataState::getNFAState() {
    return m_NFAStates;
}

//logic for finding dead ends
bool AutomataState::isDeadEnd() {
    //trivial cases
    if(m_acceptingState){
        return false;
    }
    if(m_transition.empty()){
        return true;
    }
    //iterates over a char, state map
    std::multimap<char, AutomataState*>::iterator iterator;
    for (iterator=m_transition.begin(); iterator != m_transition.end(); ++iterator){
        //value is the to state
        AutomataState *toState = iterator->second;
        //if it goes to a state besides itself, it's not a dead end
        if (toState!= this){
            return false;
        }
    }
    //otherwise (only leads to itself) it's a dead end
    return true;
}

AutomataState& AutomataState::operator=(const AutomataState &other) {
    this->m_transition = other.m_transition;
    this->m_stateId = other.m_stateId;
    this->m_NFAStates = other.m_NFAStates;
    return *this;
}

bool AutomataState::operator==(const AutomataState &other) {
    if(m_NFAStates.empty()){
        return(m_stateId == other.m_stateId);
    }
    else{
        return(m_NFAStates == other.m_NFAStates);
    }
}

std::string AutomataState::getStringId() {
    std::stringstream out;
    out << m_stateId;
    return out.str();
}
