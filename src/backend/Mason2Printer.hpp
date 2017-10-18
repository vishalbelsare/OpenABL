#pragma once
#include "GenericPrinter.hpp"

namespace OpenABL {

struct Mason2Printer : public GenericPrinter {
  using GenericPrinter::print;

  Mason2Printer(AST::Script &script)
    : GenericPrinter(script, true) {}

  void print(const AST::VarExpression &);
  void print(const AST::UnaryOpExpression &);
  void print(const AST::CallExpression &);
  void print(const AST::MemberInitEntry &);
  void print(const AST::AgentCreationExpression &);
  void print(const AST::NewArrayExpression &);
  void print(const AST::AssignStatement &);
  void print(const AST::AssignOpStatement &);
  void print(const AST::VarDeclarationStatement &);
  void print(const AST::ForStatement &);
  void print(const AST::SimulateStatement &);
  void print(const AST::FunctionDeclaration &);
  void print(const AST::AgentMember &);
  void print(const AST::AgentDeclaration &);
  void print(const AST::ConstDeclaration &);
  void print(const AST::Script &);

  void printType(Type t);

  void printSpecialBinaryOp(
      const AST::BinaryOp, const AST::Expression &, const AST::Expression &);
  bool isSpecialBinaryOp(
      const AST::BinaryOp, const AST::Expression &, const AST::Expression &);

  virtual void printAgentImports();
  virtual void printAgentExtends(const AST::AgentDeclaration &);
  virtual void printAgentExtraCode(const AST::AgentDeclaration &);
  virtual void printAgentExtraCtorArgs();
  virtual void printAgentExtraCtorCode();
  virtual void printStepDefaultCode(const AST::FunctionDeclaration &);
  virtual void printUIExtraImports();
  virtual void printUICtors();

  void printUI();

protected:
  const char *getSimVarName() const {
    return inAgent ? "_sim" : "this";
  }

  // Current input and output variables inside a step function
  VarId currentInVar;
  VarId currentOutVar;
  // Whether we're in agent code
  bool inAgent = false;
  // Whether we're in the main function
  bool inMain = false;
};

}