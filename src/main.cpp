#include <iostream>
#include <fstream>
#include <stack>
#include "../lib/utilities.h"
#include "../lib/Lexer.h"
#include "testSuite.h"
int main() {


  utilities utils;
  Lexer lexer;

  //enter infix regex here
  lexer.construct("(a|b)*");

  lexer.printNFA();
  lexer.printDFA();
  lexer.writeNFA();
  lexer.writeDFA();


  bool a = lexer.simulateDFA("a");
  bool b = lexer.simulateDFA("b");
  bool c = lexer.simulateDFA("c");
  bool d = lexer.simulataNFA("a");
  bool e = lexer.simulataNFA("b");
  bool f = lexer.simulataNFA("c");

  std::cout << a << std::endl;
  std::cout << b << std::endl;
  std::cout << c << std::endl;
  std::cout << d << std::endl;
  std::cout << e << std::endl;
  std::cout << f << std::endl;
  //testSuite::runTests();
  return 0;
}
