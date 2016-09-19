
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>

#include "testSuite.h"
#include "../lib/Lexer.h"

bool testSuite::runTests() {
    Lexer lexer;
    lexer.construct("abcd");
//    Simulate NFA
    assert ( true == lexer.simulataNFA("abcd"));
    assert ( true == lexer.simulataNFA("abcd"));
    assert ( false == lexer.simulataNFA("acd"));
//    Simulate DFA
    assert ( true == lexer.simulateDFA("abcd"));
    assert ( true == lexer.simulateDFA("abcd"));
    assert ( false == lexer.simulateDFA("acd"));

    lexer.construct("(a|b|c|d)+a*cedf");
//    Simulate NFA
    assert ( true == lexer.simulataNFA("bcedf"));
    assert ( true == lexer.simulataNFA("dacedf"));
//    Simulate DFA
    assert ( true == lexer.simulateDFA("bcedf"));
    assert ( true == lexer.simulateDFA("dacedf"));

    lexer.construct("(m|mano)*(hola)+([asdfg])");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("holaholaholaholad"));
    assert ( true == lexer.simulataNFA("manomanomanoholag"));
    assert ( true == lexer.simulataNFA("mmmmmmmmmmholaa"));
//    Simulate DFA
    assert ( true == lexer.simulateDFA("holaholaholaholad"));
    assert ( true == lexer.simulateDFA("manomanomanoholag"));
    assert ( true == lexer.simulateDFA("mmmmmmmmmmholaa"));

    lexer.construct("([abcdefgh])m");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("am"));
    assert ( true == lexer.simulataNFA("cm"));
    //    Simulate DFA
    assert ( true == lexer.simulateDFA("am"));
    assert ( true == lexer.simulateDFA("cm"));

    lexer.construct("(a|b)c(a|n)*m(hola|he)k");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("acmhek"));
    assert ( true == lexer.simulataNFA("bcmholak"));
    //    Simulate DFA
    assert ( true == lexer.simulateDFA("acmhek"));
    assert ( true == lexer.simulateDFA("bcmholak"));

    lexer.construct("([wertyuik])hol*a+");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("whoa"));
    assert ( true == lexer.simulataNFA("ehollllllllllllllllllllllllaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    //    Simulate DFA
    assert ( true == lexer.simulateDFA("whoa"));
    assert ( true == lexer.simulateDFA("ehollllllllllllllllllllllllaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));

    lexer.construct("(b|b)*abb(a|b)*");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("babbaaaaa"));
    assert ( true == lexer.simulataNFA("abb"));
    //    Simulate DFA
    assert ( true == lexer.simulateDFA("babbaaaaa"));
    assert ( true == lexer.simulateDFA("abb"));

    lexer.construct("a*b+c?([amc])");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("aaaaaaaaaaaaaabbbbbbbbbbbbba"));
    assert ( true == lexer.simulataNFA("bcc"));
    //    Simulate DFA
    assert ( true == lexer.simulateDFA("aaaaaaaaaaaaaabbbbbbbbbbbbba"));
    assert ( true == lexer.simulateDFA("bcc"));

    lexer.construct("([abc])*([edf])+(hola)?h*e+j?no");
    //    Simulate NFA
    assert ( true == lexer.simulataNFA("deno"));
    assert ( true == lexer.simulataNFA("bdddddddddddddddddholaejno"));
    //    Simulate DFA
    assert ( true == lexer.simulateDFA("deno"));
    assert ( true == lexer.simulateDFA("bdddddddddddddddddholaejno"));




}
