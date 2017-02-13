#pragma once

#include <memory>
#include "Analysis.hpp"
#include "location.hh"

namespace OpenABL {

struct Printer;

namespace AST {

using Location = OpenABL::location;

struct Type;
using TypePtr = std::unique_ptr<Type>;

struct Visitor;

struct Node {
  Location loc;

  Node(Location loc) : loc{loc} {}

  virtual void accept(Visitor &) = 0;
  virtual void print(Printer &) = 0;
};

struct Var : public Node {
  std::string name;
  VarId id;

  Var(std::string name, Location loc)
    : Node{loc}, name{name} {}

  void accept(Visitor &);
  void print(Printer &);
};

using VarPtr = std::unique_ptr<Var>;

struct Expression : public Node {
  Expression(Location loc)
    : Node{loc}, type{OpenABL::Type::INVALID} {}
  OpenABL::Type type;
};

using ExpressionPtr = std::unique_ptr<Expression>;
using ExpressionList = std::vector<ExpressionPtr>;
using ExpressionListPtr = std::unique_ptr<ExpressionList>;

struct Literal : public Expression {
  Literal(Location loc) : Expression{loc} {}

  void accept(Visitor &);
  void print(Printer &);
};

using LiteralPtr = std::unique_ptr<Literal>;

struct BoolLiteral : public Literal {
  bool value;

  BoolLiteral(bool value, Location loc)
    : Literal{loc}, value{value} {}
};

struct IntLiteral : public Literal {
  long value;

  IntLiteral(long value, Location loc)
    : Literal{loc}, value{value} {}
};

struct FloatLiteral : public Literal {
  double value;

  FloatLiteral(double value, Location loc)
    : Literal{loc}, value{value} {}
};

struct VarExpression : public Expression {
  VarPtr var;

  VarExpression(Var *var, Location loc)
    : Expression{loc}, var{var} {}

  void accept(Visitor &);
  void print(Printer &);
};

enum class UnaryOp {
  MINUS,
  PLUS,
  LOGICAL_NOT,
  BITWISE_NOT,
};

static inline const char *getUnaryOpSigil(UnaryOp op) {
  switch (op) {
    case UnaryOp::MINUS:       return "-";
    case UnaryOp::PLUS:        return "+";
    case UnaryOp::LOGICAL_NOT: return "!";
    case UnaryOp::BITWISE_NOT: return "~";
  }
}

struct UnaryOpExpression : public Expression {
  UnaryOp op;
  ExpressionPtr expr;

  UnaryOpExpression(UnaryOp op, Expression *expr, Location loc)
    : Expression{loc}, op{op}, expr{expr} {}

  void accept(Visitor &);
  void print(Printer &);
};

enum class BinaryOp {
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  BITWISE_AND,
  BITWISE_XOR,
  BITWISE_OR,
  SHIFT_LEFT,
  SHIFT_RIGHT,
  EQUALS,
  NOT_EQUALS,
  SMALLER,
  SMALLER_EQUALS,
  GREATER,
  GREATER_EQUALS,
  LOGICAL_AND,
  LOGICAL_OR,
  RANGE,
};

static inline const char *getBinaryOpSigil(BinaryOp op) {
  switch (op) {
    case BinaryOp::ADD:            return "+";
    case BinaryOp::SUB:            return "-";
    case BinaryOp::MUL:            return "*";
    case BinaryOp::DIV:            return "/";
    case BinaryOp::MOD:            return "%";
    case BinaryOp::BITWISE_AND:    return "&";
    case BinaryOp::BITWISE_XOR:    return "^";
    case BinaryOp::BITWISE_OR:     return "|";
    case BinaryOp::SHIFT_LEFT:     return "<<";
    case BinaryOp::SHIFT_RIGHT:    return ">>";
    case BinaryOp::EQUALS:         return "==";
    case BinaryOp::NOT_EQUALS:     return "!=";
    case BinaryOp::SMALLER:        return "<";
    case BinaryOp::SMALLER_EQUALS: return "<=";
    case BinaryOp::GREATER:        return ">";
    case BinaryOp::GREATER_EQUALS: return ">=";
    case BinaryOp::LOGICAL_AND:    return "&&";
    case BinaryOp::LOGICAL_OR:     return "||";
    case BinaryOp::RANGE:          return "..";
  }
}

struct BinaryOpExpression : public Expression {
  BinaryOp op;
  ExpressionPtr left;
  ExpressionPtr right;

  BinaryOpExpression(BinaryOp op, Expression *left, Expression *right, Location loc)
    : Expression{loc}, op{op}, left{left}, right{right} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct AssignOpExpression : public Expression {
  BinaryOp op; // Note: Not all binary ops are allowed here!
  ExpressionPtr left;
  ExpressionPtr right;

  AssignOpExpression(BinaryOp op, Expression *left, Expression *right, Location loc)
    : Expression{loc}, op{op}, left{left}, right{right} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct AssignExpression : public Expression {
  ExpressionPtr left;
  ExpressionPtr right;

  AssignExpression(Expression *left, Expression *right, Location loc)
    : Expression{loc}, left{left}, right{right} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct Arg : public Node {
  ExpressionPtr expr;
  ExpressionPtr outExpr;

  Arg(Expression *expr, Expression *outExpr, Location loc)
    : Node{loc}, expr{expr}, outExpr{outExpr} {}

  void accept(Visitor &);
  void print(Printer &);
};

using ArgPtr = std::unique_ptr<Arg>;
using ArgList = std::vector<ArgPtr>;
using ArgListPtr = std::unique_ptr<ArgList>;

struct CallExpression : public Expression {
  std::string name;
  ArgListPtr args;

  CallExpression(std::string name, ArgList *args, Location loc)
    : Expression{loc}, name{name}, args{args} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct MemberAccessExpression : public Expression {
  ExpressionPtr expr;
  std::string member;

  MemberAccessExpression(Expression *expr, std::string member, Location loc)
    : Expression{loc}, expr{expr}, member{member} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct TernaryExpression : public Expression {
  ExpressionPtr condExpr;
  ExpressionPtr ifExpr;
  ExpressionPtr elseExpr;

  TernaryExpression(Expression *condExpr, Expression *ifExpr, Expression *elseExpr, Location loc)
    : Expression{loc}, condExpr{condExpr}, ifExpr{ifExpr}, elseExpr{elseExpr} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct Statement : public Node {
  Statement(Location loc) : Node{loc} {}
};

using StatementPtr = std::unique_ptr<Statement>;
using StatementList = std::vector<StatementPtr>;
using StatementListPtr = std::unique_ptr<StatementList>;

struct ExpressionStatement : public Statement {
  ExpressionPtr expr;

  ExpressionStatement(Expression *expr, Location loc)
    : Statement{loc}, expr{expr} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct BlockStatement : public Statement {
  StatementListPtr stmts;

  BlockStatement(StatementList *stmts, Location loc)
    : Statement{loc}, stmts{stmts} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct VarDeclarationStatement : public Statement {
  TypePtr type;
  VarPtr var;
  ExpressionPtr initializer;

  VarDeclarationStatement(Type *type, Var *var, Expression *initializer, Location loc)
    : Statement{loc}, type{type}, var{var}, initializer{initializer} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct IfStatement : public Statement {
  ExpressionPtr condExpr;
  StatementPtr ifStmt;
  StatementPtr elseStmt;

  IfStatement(Expression *condExpr, Statement *ifStmt, Statement *elseStmt, Location loc)
    : Statement{loc}, condExpr{condExpr}, ifStmt{ifStmt}, elseStmt{elseStmt} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct ForStatement : public Statement {
  TypePtr type;
  VarPtr var;
  ExpressionPtr expr;
  StatementPtr stmt;

  ForStatement(Type *type, Var *var, Expression *expr, Statement *stmt, Location loc)
    : Statement{loc}, type{type}, var{var}, expr{expr}, stmt{stmt} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct ParallelForStatement : public Statement {
  TypePtr type;
  VarPtr var;
  VarPtr outVar;
  ExpressionPtr expr;
  StatementPtr stmt;

  ParallelForStatement(Type *type, Var *var, Var *outVar,
               Expression *expr, Statement *stmt, Location loc)
    : Statement{loc}, type{type}, var{var}, outVar{outVar},
      expr{expr}, stmt{stmt} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct Type : public Node {
  Type(Location loc) : Node{loc} {}
};

struct SimpleType : public Type {
  std::string name;

  SimpleType(std::string name, Location loc)
    : Type{loc}, name{name} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct ArrayType : public Type {
  TypePtr type;

  ArrayType(Type *type, Location loc)
    : Type{loc}, type{type} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct Param : public Node {
  TypePtr type;
  VarPtr var;
  VarPtr outVar;

  Param(Type *type, Var *var, Var *outVar, Location loc)
    : Node{loc}, type{type}, var{var}, outVar{outVar} {}

  void accept(Visitor &);
  void print(Printer &);
};

using ParamPtr = std::unique_ptr<Param>;
using ParamList = std::vector<ParamPtr>;
using ParamListPtr = std::unique_ptr<ParamList>;

/* Top-level declaration */
struct Declaration : public Node {
  Declaration(Location loc) : Node{loc} {}
};

using DeclarationPtr = std::unique_ptr<Declaration>;
using DeclarationList = std::vector<DeclarationPtr>;
using DeclarationListPtr = std::unique_ptr<DeclarationList>;

struct FunctionDeclaration : public Declaration {
  bool isInteract;
  TypePtr returnType;
  std::string name;
  ParamListPtr params;
  StatementListPtr stmts;

  FunctionDeclaration(bool isInteract, Type *returnType, std::string name,
                      ParamList *params, StatementList *stmts, Location loc)
    : Declaration{loc}, isInteract{isInteract}, returnType{returnType},
      name{name}, params{params}, stmts{stmts} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct AgentMember : public Node {
  bool isPosition;
  TypePtr type;
  std::string name;

  AgentMember(bool isPosition, Type *type, std::string name, Location loc)
    : Node{loc}, isPosition{isPosition}, type{type}, name{name} {}

  void accept(Visitor &);
  void print(Printer &);
};

using AgentMemberPtr = std::unique_ptr<AgentMember>;
using AgentMemberList = std::vector<AgentMemberPtr>;
using AgentMemberListPtr = std::unique_ptr<AgentMemberList>;

struct AgentDeclaration : public Declaration {
  std::string name;
  AgentMemberListPtr members;

  AgentDeclaration(std::string name, AgentMemberList *members, Location loc)
    : Declaration{loc}, name{name}, members{members} {}

  void accept(Visitor &);
  void print(Printer &);
};

struct ConstDeclaration : public Declaration {
  TypePtr type;
  VarPtr var;
  LiteralPtr value;

  ConstDeclaration(Type *type, Var *var, Literal *value, Location loc)
    : Declaration{loc}, type{type}, var{var}, value{value} {}

  void accept(Visitor &);
  void print(Printer &);
};

/* AST root node */
struct Script : public Node {
  DeclarationListPtr decls;

  Script(DeclarationList *decls, Location loc)
    : Node{loc}, decls{decls} {}

  void accept(Visitor &);
  void print(Printer &);
};

}
}
