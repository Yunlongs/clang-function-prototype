#include <utility>
#include<sstream>
#include<iostream>
#include<fstream>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include <assert.h>
#include <cstdlib>
#include <vector>
#include <deque>
#include <set>
using namespace clang;

std::string log_path;
template<class Data>
class FIFOWorkList {
    typedef std::set<Data> DataSet;
    typedef std::deque<Data> DataDeque;
public:
    FIFOWorkList() {}

    ~FIFOWorkList() {}

    inline bool empty() const {
        return data_list.empty();
    }

    inline bool find(Data data) const {
        return (data_set.find(data) == data_set.end() ? false : true);
    }

    inline bool push(Data data) {
        if (data_set.find(data) == data_set.end()) {
            data_list.push_back(data);
            data_set.insert(data);
            return true;
        }
        else
            return false;
    }

    inline Data pop() {
        assert(!empty() && "work list is empty");
        Data data = data_list.front();
        data_list.pop_front();
        data_set.erase(data);
        return data;
    }

    inline void clear() {
        data_list.clear();
        data_set.clear();
    }

private:
    DataSet data_set;	///< store all data in the work list.
    DataDeque data_list;	///< work list using std::vector.
};

namespace {

    class PrintFunctionsConsumer : public ASTConsumer {
        CompilerInstance &Instance;
        std::set<std::string> ParsedTemplates;

    public:
        PrintFunctionsConsumer(CompilerInstance &Instance, std::set<std::string> ParsedTemplates)
                : Instance(Instance), ParsedTemplates(std::move(ParsedTemplates)) {}

        typedef FIFOWorkList<const Stmt*> StmtWorkList;

        std::ostringstream PrintCalleeDecl(const FunctionDecl *FD) {
            std::ostringstream ostring;
            ostring<< "\t{'function': '" << FD->getQualifiedNameAsString() <<"'"
                    << ", 'return_type': '" << QualType(FD->getReturnType().getTypePtr()->getUnqualifiedDesugaredType(),0).getAsString() <<"'"
                    << ", 'parms': '";
            for (auto item : FD->parameters()){
                ostring << QualType(item->getOriginalType().getTypePtr()->getUnqualifiedDesugaredType(), 0).getAsString()
                << "@" << item->getNameAsString()  << ",";
            }
            ostring << "'}\n";
            return ostring;
        }

        bool HandleTopLevelDecl(DeclGroupRef DG) override {
            clang::SourceManager &SourceManager = Instance.getSourceManager();
            for (auto D : DG) {
                if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
                    FD = FD->getDefinition() == nullptr ? FD : FD->getDefinition();
                    if (!FD->isThisDeclarationADefinition()) continue;

                    std::ostringstream ostring;
                    SourceLocation begin1 = FD->getSourceRange().getBegin();
                    SourceRange sr = FD->getSourceRange();
                    PresumedLoc begin = SourceManager.getPresumedLoc(sr.getBegin());
                    PresumedLoc end = SourceManager.getPresumedLoc(sr.getEnd());


                    ostring<< "{'function': '" << FD->getQualifiedNameAsString() <<"'"
                    << ", 'return_type': '" << QualType(FD->getReturnType().getTypePtr()->getUnqualifiedDesugaredType(),0).getAsString() <<"'"
                    << ", 'parms': '";
                    for (auto item : FD->parameters()){
                        ostring << QualType(item->getOriginalType().getTypePtr()->getUnqualifiedDesugaredType(), 0).getAsString()
                        << "@" << item->getNameAsString()  << ",";
                    }
                    ostring << "'"
                    << ", 'file' :'" << begin.getFilename() << "'"
                    << ", 'begin': [" << begin.getLine() << ", " << begin.getColumn() << "]"
                    << ", 'end': [" <<end.getLine() << ", " << end.getColumn() << "]"
                    << "}" << "\n";

                    auto funcBody = FD->getBody();
                    if(!funcBody)return true;
                    StmtWorkList worklist;
                    worklist.push(funcBody);
                    while (!worklist.empty()) {
                        auto currentStmt = worklist.pop();
                        if (auto callExpr = dyn_cast<CallExpr>(currentStmt)) {
                            auto CD = callExpr->getCalleeDecl();
                            if (!CD) continue;
                            if (auto calleeFD = dyn_cast<FunctionDecl>(CD))
                                {
                                    std::ostringstream temp_stream = PrintCalleeDecl(calleeFD);
                                    ostring << temp_stream.str();
                                }
                        }
                        for (auto stmt : currentStmt->children())
                            if(stmt)worklist.push(stmt);
                    }
                    std::string result = ostring.str();
                    std::ofstream file;
                    file.open(log_path,std::ios::app);
                    file<<result;
                    file.close();
                    std::cout << result;
                }
            }

            return true;
        }

    };

    class PrintFunctionNamesAction : public PluginASTAction {
        std::set<std::string> ParsedTemplates;
    protected:
        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                       llvm::StringRef) override {
            return std::make_unique<PrintFunctionsConsumer>(CI, ParsedTemplates);
        }

        bool ParseArgs(const CompilerInstance &CI,
                       const std::vector<std::string> &args) override {
            if(args.size() <1)
            {
                llvm::errs()<< "Lack log path!\n";
                return false;
            }
            log_path = args[0];
            return true;
        }
    };

}

static FrontendPluginRegistry::Add<PrintFunctionNamesAction>
        X("print-fns", "print function names");
