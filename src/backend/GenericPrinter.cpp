/* Copyright 2017 OpenABL Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include <cmath>
#include "GenericPrinter.hpp"

namespace OpenABL {

static void printStringLiteral(Printer &p, const std::string &str) {
  p << '"';
  for (char c : str) {
    if (c == '"' || c == '\\') {
      p << '\\' << c;
    } else {
      p << c;
    }
  }
  p << '"';
}

void GenericPrinter::print(const AST::SimpleType &type) {
  printType(type.resolved);
}

void GenericPrinter::print(const AST::Var &var) {
  *this << var.name;
}
void GenericPrinter::print(const AST::Literal &lit) {
  if (const AST::IntLiteral *ilit = dynamic_cast<const AST::IntLiteral *>(&lit)) {
    *this << ilit->value;
  } else if (const AST::FloatLiteral *flit = dynamic_cast<const AST::FloatLiteral *>(&lit)) {
    std::ostringstream s;
    s << flit->value;
    std::string str = s.str();
    if (str.find(".") == std::string::npos && std::isfinite(flit->value)) {
      // Make sure it looks like a floating point number...
      str += ".0";
    }
    *this << str;
  } else if (const AST::BoolLiteral *blit = dynamic_cast<const AST::BoolLiteral *>(&lit)) {
    *this << (blit->value ? "true" : "false");
  } else if (const AST::StringLiteral *slit = dynamic_cast<const AST::StringLiteral *>(&lit)) {
    printStringLiteral(*this, slit->value);
  } else {
    assert(0);
  }
}

void GenericPrinter::print(const AST::VarExpression &expr) {
  *this << *expr.var;
}
void GenericPrinter::print(const AST::UnaryOpExpression &expr) {
  *this << "(" << AST::getUnaryOpSigil(expr.op) << *expr.expr << ")";
}
void GenericPrinter::print(const AST::BinaryOpExpression &expr) {
  if (isSpecialBinaryOp(expr.op, *expr.left, *expr.right)) {
    printSpecialBinaryOp(expr.op, *expr.left, *expr.right);
    return;
  }

  *this << "(" << *expr.left << " "
        << AST::getBinaryOpSigil(expr.op) << " " << *expr.right << ")";
}
void GenericPrinter::print(const AST::TernaryExpression &expr) {
  *this << "(" << *expr.condExpr << " ? " << *expr.ifExpr << " : " << *expr.elseExpr << ")";
}
void GenericPrinter::print(const AST::MemberAccessExpression &expr) {
  *this << *expr.expr << "." << expr.member;
}
void GenericPrinter::print(const AST::ArrayAccessExpression &expr) {
  *this << *expr.arrayExpr << "[" << *expr.offsetExpr << "]";
}

void GenericPrinter::print(const AST::ArrayInitExpression &expr) {
  *this << "{ ";
  printCommaSeparated(*expr.exprs, [&](const AST::ExpressionPtr &expr) {
    *this << *expr;
  });
  *this << " }";
}

void GenericPrinter::printArgs(const AST::CallExpression &expr) {
  printCommaSeparated(*expr.args, [&](const AST::ExpressionPtr &arg) {
    *this << *arg;
  });
}

void GenericPrinter::print(const AST::ExpressionStatement &stmt) {
  *this << *stmt.expr << ";";
}
void GenericPrinter::print(const AST::AssignStatement &expr) {
  *this << *expr.left << " = " << *expr.right << ";";
}

void GenericPrinter::print(const AST::AssignOpStatement &stmt) {
  if (isSpecialBinaryOp(stmt.op, *stmt.left, *stmt.right)) {
    *this << *stmt.left << " = ";
    printSpecialBinaryOp(stmt.op, *stmt.left, *stmt.right);
    *this << ";";
    return;
  }

  *this << *stmt.left << " " << AST::getBinaryOpSigil(stmt.op)
        << "= " << *stmt.right << ";";
}

void GenericPrinter::print(const AST::BlockStatement &stmt) {
  *this << "{" << indent << *stmt.stmts << outdent << nl << "}";
}

void GenericPrinter::print(const AST::IfStatement &stmt) {
  *this << "if (" << *stmt.condExpr << ") " << *stmt.ifStmt;
  if (stmt.elseStmt) {
    *this << " else " << *stmt.elseStmt;
  }
}
void GenericPrinter::print(const AST::WhileStatement &stmt) {
  *this << "while (" << *stmt.expr << ") " << *stmt.stmt;
}

void GenericPrinter::print(const AST::VarDeclarationStatement &stmt) {
    *this << *stmt.type << " " << *stmt.var;
    if (stmt.initializer) {
      *this << " = " << *stmt.initializer;
    }
    *this << ";";
}
void GenericPrinter::print(const AST::ReturnStatement &stmt) {
  if (stmt.expr) {
    *this << "return " << *stmt.expr << ";";
  } else {
    *this << "return;";
  }
}
void GenericPrinter::print(const AST::BreakStatement &) {
  *this << "break;";
}
void GenericPrinter::print(const AST::ContinueStatement &) {
  *this << "continue;";
}
void GenericPrinter::print(const AST::ConstDeclaration &decl) {
  *this << *decl.type << " " << *decl.var
        << (decl.isArray ? "[]" : "")
        << " = " << *decl.expr << ";";
}

void GenericPrinter::print(const AST::Param &param) {
  *this << *param.type << " " << *param.var;
  if (param.outVar) {
    *this << ", " << *param.type << " " << *param.outVar;
  }
}

void GenericPrinter::printParams(const AST::FunctionDeclaration &decl) {
  printCommaSeparated(*decl.params, [&](const AST::ParamPtr &param) {
    *this << *param;
  });
}

void GenericPrinter::print(const AST::FunctionDeclaration &decl) {
  const std::string &name = supportsOverloads ? decl.name : decl.sig.name;
  *this << *decl.returnType << " " << name << "(";
  printParams(decl);
  *this << ") {" << indent << *decl.stmts << outdent << nl << "}";
}

}
