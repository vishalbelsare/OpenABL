#pragma once

#include "AST.hpp"

namespace OpenABL {

struct NotSupportedError : public std::logic_error {
  NotSupportedError(const std::string &msg) : std::logic_error(msg) {}
};

struct Backend {
  virtual void generate(AST::Script &script, const std::string &targetDir,
                        const std::string &assetDir) = 0;
};

struct CBackend : public Backend {
  void generate(AST::Script &script, const std::string &targetDir,
                const std::string &assetDir);
};

struct FlameBackend : public Backend {
  void generate(AST::Script &script, const std::string &targetDir,
                const std::string &assetDir);
};

struct FlameGPUBackend : public Backend {
  void generate(AST::Script &script, const std::string &targetDir,
                const std::string &assetDir);
};

struct MasonBackend : public Backend {
  void generate(AST::Script &script, const std::string &targetDir,
                const std::string &assetDir);
};

struct DMasonBackend : public Backend {
  void generate(AST::Script &script, const std::string &targetDir,
                const std::string &assetDir);
};

}
