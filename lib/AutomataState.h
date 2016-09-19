
#ifndef PROYECT1_AUTOMATASTATE_H
#define PROYECT1_AUTOMATASTATE_H

#include <map>
#include <set>
#include <vector>
#include <string>

class AutomataState {

private:
    std::set<AutomataState*> m_NFAStates;

public:
    typedef std::vector<AutomataState*> table;
    typedef std::set<AutomataState*>::iterator stateIterator;
    std::multimap<char, AutomataState*> m_transition;
    std::multimap<char, AutomataState*> m_transitionDFA;
    int m_stateId;
    bool m_acceptingState;
    bool m_marked;
    int m_groupId;

    AutomataState() : m_stateId(-1), m_acceptingState(false){};
    AutomataState(int sID) : m_stateId(sID), m_acceptingState(false), m_groupId(0) {};
    AutomataState(std::set<AutomataState*> NFAState, int sID);
    AutomataState(const AutomataState &other);

    virtual ~AutomataState();

    void createTransition(char inputChar, AutomataState *pState);
    void removeTransition(AutomataState* pState);
    void getTransition(char inputChar, table &States);
    std::set<AutomataState*>& getNFAState();

    bool isDeadEnd();
    AutomataState& operator=(const AutomataState& other);
    bool operator==(const AutomataState& other);
    std::string getStringId();

};


#endif //PROYECT1_AUTOMATA_H
