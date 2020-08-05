#include <stdio.h>
#include <iostream>
#include "llvm/Support/Host.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringRef.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include <iostream>
#include <vector>
#include <sstream> 
#include <fstream>
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class FindFuncVisitor
        : public RecursiveASTVisitor<FindFuncVisitor> {

clang::SourceManager &SourceManager;


public:
  FindFuncVisitor(clang::SourceManager &SourceManager)
      : SourceManager(SourceManager) {}

    bool shouldTraversePostOrder()
    {
        return true;
    }
    bool shouldVisitTemplateInstantiations(){
        return true;
    }

    bool shouldVisitImplicitCode(){
        return true;
    } 

  bool VisitFunctionDecl(const FunctionDecl *fd) {
    

    if (fd->isFirstDecl())
    {
        SourceLocation begin = fd->getSourceRange().getBegin();
        if (!SourceManager.isInSystemHeader(begin))
        {
            SourceRange sr = fd->getSourceRange();
            PresumedLoc begin = SourceManager.getPresumedLoc(sr.getBegin());
            PresumedLoc end = SourceManager.getPresumedLoc(sr.getEnd());

            std::vector<std::vector<std::string>> args;

            for( FunctionDecl::param_const_iterator it = fd->param_begin();it!=fd->param_end();it++)
            {
                std::vector<std::string> temp;
                ParmVarDecl* param = *it;
                QualType type = param->getOriginalType();
                std::string argtypename = type.getAsString();
                temp.push_back(argtypename);
                llvm::StringRef argname = param->getName();
                if(argname.empty())
                {
                    temp.push_back("");
                }
                else
                {
                    temp.push_back(argname.str());
                }
                args.push_back(temp);
            }

            std::string params = "";
            for(auto it : args)
            {
                std::string argt = it[0];
                std::string argn = it[1];
                params += argt + "@" + argn + ",";
            }

            std::ostringstream ostring;

            ostring << "{'function': '" <<  fd->getDeclName().getAsString() << "'"
            << ", 'return_type': '" << fd->getReturnType().getAsString() << "'"
            << ", 'parms': '" << params << "'"
            << ", 'file': '" << begin.getFilename() << "'"
            << ", 'begin': [" << begin.getLine() << ", " << begin.getColumn() << "]"
            << ", 'end': [" << end.getLine() << ", " << end.getColumn() << "]"
            << "}" << "\n";

            std::string result = ostring.str();

            std::ofstream file;
            file.open("prototype.result",std::ios::app);
            file << result;
            file.close();

            std::cout << result;
        }
    }

    return true;
}

};

class FindFuncConsumer : public clang::ASTConsumer {
public:
    explicit FindFuncConsumer(clang::SourceManager &SM)
            : Visitor(SM) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    FindFuncVisitor Visitor;
};


class FindFuncAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new FindFuncConsumer(Compiler.getSourceManager()));
    }
};

static cl::OptionCategory FindFunc("function-prototype options");


int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, FindFunc);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<FindFuncAction>().get());

}
