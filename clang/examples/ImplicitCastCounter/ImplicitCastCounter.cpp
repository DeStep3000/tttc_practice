#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;

class CastVisitor : public RecursiveASTVisitor<CastVisitor> {
public:
    explicit CastVisitor(FunctionDecl *Func) : CurrentFunc(Func) {}

    bool VisitImplicitCastExpr(ImplicitCastExpr *Cast) {
        QualType From = Cast->getSubExpr()->getType();
        QualType To = Cast->getType();
        if (From.getAsString() != To.getAsString()) {
            CastCounts[std::make_pair(From.getAsString(), To.getAsString())]++;
        }
        return true;
    }

    void PrintResults() const {
        llvm::outs() << "Function `" << CurrentFunc->getNameAsString() << "`\n";
        for (const auto &Entry : CastCounts) {
            llvm::outs() << Entry.first.first << " -> " << Entry.first.second << ": " << Entry.second << "\n";
        }
        llvm::outs() << "\n";
    }

private:
    FunctionDecl *CurrentFunc;
    std::map<std::pair<std::string, std::string>, int> CastCounts;
};

class CastConsumer : public ASTConsumer {
public:
    explicit CastConsumer(ASTContext *Context) : Context(Context) {}

    void HandleTranslationUnit(ASTContext &Ctx) override {
        for (auto D : Ctx.getTranslationUnitDecl()->decls()) {
            if (auto *FD = llvm::dyn_cast<FunctionDecl>(D)) {
                if (FD->hasBody()) {
                    CastVisitor Visitor(FD);
                    Visitor.TraverseStmt(FD->getBody());
                    Visitor.PrintResults();
                }
            }
        }
    }

private:
    ASTContext *Context;
};

class CastAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {
        return std::make_unique<CastConsumer>(&CI.getASTContext());
    }

    bool ParseArgs(const CompilerInstance &, const std::vector<std::string> &) override {
        return true;
    }
};

static FrontendPluginRegistry::Add<CastAction> X("count-implicit-casts", "Counts implicit conversions");
