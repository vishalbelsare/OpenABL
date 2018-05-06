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

#include "Backend.hpp"
#include "MasonPrinter.hpp"
#include "FileUtil.hpp"

namespace OpenABL {

static std::string generateMainCode(AST::Script &script) {
  MasonPrinter printer(script);
  printer.print(script);
  return printer.extractStr();
}

static std::string generateAgentCode(AST::Script &script, AST::AgentDeclaration &agent) {
  MasonPrinter printer(script);
  printer.print(agent);
  return printer.extractStr();
}

static std::string generateUICode(AST::Script &script) {
  MasonPrinter printer(script);
  printer.printUI();
  return printer.extractStr();
}

static std::string getClassPathPrefix(const BackendContext &ctx) {
  std::string masonDir = ctx.depsDir + "/mason";
  if (directoryExists(masonDir)) {
    return "CLASSPATH=" + masonDir + ":$CLASSPATH ";
  } else {
    return "";
  }
}

static std::string generateBuildScript(const BackendContext &ctx) {
  return getClassPathPrefix(ctx) + "javac *.java";
}

static std::string generateRunScript(const BackendContext &ctx) {
  bool visualize = ctx.config.getBool("visualize", false);
  const char *simClass = visualize ? "SimWithUI" : "Sim";
  return getClassPathPrefix(ctx) + "java " + simClass;
}

void MasonBackend::generate(
    AST::Script &script, const BackendContext &ctx) {
  bool useFloat = ctx.config.getBool("use_float", false);
  if (useFloat) {
    throw BackendError("Floats are not supported by the Mason backend");
  }

  writeToFile(ctx.outputDir + "/Sim.java", generateMainCode(script));
  writeToFile(ctx.outputDir + "/SimWithUI.java", generateUICode(script));

  for (AST::AgentDeclaration *agent : script.agents) {
    writeToFile(ctx.outputDir + "/" + agent->name + ".java", generateAgentCode(script, *agent));
  }

  copyFile(ctx.assetDir + "/mason/Util.java", ctx.outputDir + "/Util.java");
  writeToFile(ctx.outputDir + "/build.sh", generateBuildScript(ctx));
  writeToFile(ctx.outputDir + "/run.sh", generateRunScript(ctx));
  makeFileExecutable(ctx.outputDir + "/build.sh");
  makeFileExecutable(ctx.outputDir + "/run.sh");
}

}
