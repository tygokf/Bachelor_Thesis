#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "clang/AST/ParentMapContext.h"
#include <stack>
#include <map>

using namespace std;
using namespace clang;
using namespace oclint;

//TODO: Add to references if var is assigned to
class MostLocalScopeRule : public AbstractASTVisitorRule<MostLocalScopeRule>
{
private:
    class StmtAnalyzer : public RecursiveASTVisitor<StmtAnalyzer>
        {
        private:
            vector<DeclRefExpr*> declRef;
            vector<VarDecl*> varDecl;
        public:
            vector<DeclRefExpr*> getDeclRefExpr(Stmt *stmt)
            {
                declRef.clear();
                (void) TraverseStmt(stmt);

                return declRef;
            }

            vector<VarDecl*> getVarDecl(Stmt *stmt)
            {
                varDecl.clear();
                (void) TraverseStmt(stmt);

                return varDecl;
            }

            bool VisitVarDecl(VarDecl *node)
            {
                varDecl.push_back(node);
                return true;
            }

            bool VisitDeclRefExpr(DeclRefExpr *node)
            {
                declRef.push_back(node);
                return true;
            }
    };

    stack<CompoundStmt*> compounds;
    vector<VarDecl*> allDeclarations;
    vector<DeclRefExpr*> allReferences;

    template <typename T>
    vector<T> getNewElements(vector<T> old, vector<T> possibleNew)
    {
        vector<T> newElements;

        for (T newElem : possibleNew)
        {   
            for (T oldElem : old)
            {
                if (oldElem == newElem) continue;
            }

            old.push_back(newElem);
            newElements.push_back(newElem);
        }

        return newElements;
    }

    vector<VarDecl*> getUnusedDeclarations(vector<VarDecl*> foundDecl, vector<DeclRefExpr*> foundRef)
    {
        vector<VarDecl*> unused;

        for (VarDecl *decl : foundDecl)
        {
            bool isUsed = false;

            for (DeclRefExpr *ref : foundRef)
            {
                if (decl == ref->getFoundDecl()) isUsed = true;
            }

            if (!isUsed) unused.push_back(decl);
        }

        return unused;
    }

    // void recursiveUpdateReferences(Expr *expr)
    // {
    //     if (expr == nullptr) return;

    //     if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(expr)) references.push_back(declRefExpr);
    //     else if (CastExpr *castExpr = dyn_cast<CastExpr>(expr)) recursiveUpdateReferences(castExpr->getSubExpr());
    //     else updateReferences(expr);
    // }

    // void updateReferences(Expr *expr)
    // {
    //     if (expr == nullptr) return;

    //     if (BinaryOperator *binaryOp = dyn_cast<BinaryOperator>(expr))
    //     {
    //         recursiveUpdateReferences(binaryOp->getLHS());
    //         recursiveUpdateReferences(binaryOp->getRHS());
    //     }
        
    //     if (UnaryOperator *unaryOp = dyn_cast<UnaryOperator>(expr))
    //     {
    //         recursiveUpdateReferences(unaryOp->getSubExpr());
    //     }

    //     if (CallExpr *callExpr = dyn_cast<CallExpr>(expr))
    //     {
    //         for (Expr *arg : callExpr->arguments())
    //         {
    //             recursiveUpdateReferences(arg);
    //         }
    //     }
    // }

    // void removeFromUnusedIfDeclRef(DeclRefExpr *declRef)
    // {
    //     if (VarDecl *varDecl = dyn_cast<VarDecl>(declRef->getFoundDecl()))
    //     {
    //         for (vector<VarDecl*>::iterator it = unusedDecl.begin(); it != unusedDecl.end(); it++)
    //         {                   
    //             if (varDecl == *it)
    //             {
    //                 unusedDecl.erase(it);
    //                 break;
    //             }
    //         }
    //     }
    // }

    // bool addToUnusedIfVarDecl(Stmt *stmt)
    // {
    //     if (DeclStmt *declStmt = dyn_cast<DeclStmt>(stmt))
    //     {
    //         if (declStmt->isSingleDecl())
    //         {
    //             if (VarDecl *varDecl = dyn_cast<VarDecl>(declStmt->getSingleDecl()))
    //             {
    //                 unusedDecl.push_back(varDecl);
    //                 updateReferences(varDecl->getInit());

    //                 return true;
    //             }
    //         }
    //     }

    //     return false;
    // }

    // vector<VarDecl*> getVarsWithIncorrectScope(CompoundStmt *compoundStmt)
    // {
    //     StmtAnalyzer Analyzer;
    //     vector<VarDecl*> incorrectScope;

    //     addViolation(compoundStmt, this, "inside compoundstmt");

    //     for (CompoundStmt::body_iterator body = compoundStmt->body_begin(),
    //         bodyEnd = compoundStmt->body_end(); body != bodyEnd; body++)
    //     {
    //         Stmt *stmt = *body;
    //         vector<VarDecl*> foundDecl;
    //         vector<DeclRefExpr*> foundRef;

    //         if (stmt == nullptr) continue;

    //         if (isa<WhileStmt>(stmt))
    //         {
    //             //TODO: Loop through whilestmt
    //         }

    //         if (isa<CompoundStmt>(*body))
    //         {
    //             addViolation(stmt, this, "Found compoundstmt");
    //             continue;
    //         }

    //         foundDecl = Analyzer.getVarDecl(stmt);
    //         unusedDecl.insert(unusedDecl.end(), foundDecl.begin(), foundDecl.end()); 

    //         foundRef = Analyzer.getDeclRefExpr(stmt);
    //         references.insert(references.end(), foundRef.begin(), foundRef.end()); 

    //         // addToUnusedIfVarDecl(stmt);
            
    //         // if (Expr *expr = dyn_cast<Expr>(stmt))
    //         // {
    //         //     updateReferences(expr);
    //         // }

    //         // if (ReturnStmt *returnStmt = dyn_cast<ReturnStmt>(stmt))
    //         // {
    //         //     updateReferences(returnStmt->getRetValue());
    //         // }
    //     }



    //     for (VarDecl *decl : unusedDecl)
    //     {
    //         addViolation(decl, this, "decl: " + decl->getNameAsString());
    //     }
    //     for (DeclRefExpr *reference : references)
    //     {
    //         addViolation(reference, this, "ref: " + reference->getFoundDecl()->getNameAsString());
    //         removeFromUnusedIfDeclRef(reference);
    //     }

    //     incorrectScope = unusedDecl;
    //     references.clear();
    //     unusedDecl.clear();

    //     return incorrectScope;
    // }

    vector<VarDecl*> getDeclarations(CompoundStmt *compoundStmt)
    {
        StmtAnalyzer Analyzer;
        vector<VarDecl*> foundDeclarations;
        
        for (CompoundStmt::body_iterator body = compoundStmt->body_begin(),
            bodyEnd = compoundStmt->body_end(); body != bodyEnd; body++)
        {
            Stmt *stmt = *body;
            vector<VarDecl*> newDeclarations;

            if (stmt == nullptr) continue;

            newDeclarations = getNewElements(allDeclarations, Analyzer.getVarDecl(stmt));
            foundDeclarations.insert(foundDeclarations.end(), newDeclarations.begin(), newDeclarations.end()); 
        }

        return foundDeclarations;
    }

    vector<DeclRefExpr*> getReferences(CompoundStmt *compoundStmt)
    {
        StmtAnalyzer Analyzer;
        vector<DeclRefExpr*> foundReferences;
        
        for (CompoundStmt::body_iterator body = compoundStmt->body_begin(),
            bodyEnd = compoundStmt->body_end(); body != bodyEnd; body++)
        {
            Stmt *stmt = *body;
            vector<DeclRefExpr*> newReferences;


            if (stmt == nullptr) continue;

            newReferences = getNewElements(allReferences, Analyzer.getDeclRefExpr(stmt));
            foundReferences.insert(foundReferences.end(), newReferences.begin(), newReferences.end()); 
        }

        return foundReferences;
    }

public:
    virtual const string name() const override
    {
        return "most local scope";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return "basic";
    }

#ifdef DOCGEN
    virtual const std::string since() const override
    {
        return "22.02";
    }

    virtual const std::string description() const override
    {
        return ""; // TODO: fill in the description of the rule.
    }

    virtual const std::string example() const override
    {
        return R"rst(
.. code-block:: cpp

    void example()
    {
        // TODO: modify the example for this rule.
    }
        )rst";
    }

    /* fill in the file name only when it does not match the rule identifier
    virtual const std::string fileName() const override
    {
        return "";
    }
    */

    /* add each threshold's key, description, and its default value to the map.
    virtual const std::map<std::string, std::string> thresholds() const override
    {
        std::map<std::string, std::string> thresholdMapping;
        return thresholdMapping;
    }
    */

    /* add additional document for the rule, like references, notes, etc.
    virtual const std::string additionalDocument() const override
    {
        return "";
    }
    */

    /* uncomment this method when the rule is enabled to be suppressed.
    virtual bool enableSuppress() const override
    {
        return true;
    }
    */
#endif

    virtual void setUp() override {}
    virtual void tearDown() override 
    {
        while (!compounds.empty())
        {
            CompoundStmt *currentCompound = compounds.top();

            vector<VarDecl*> declarations = getDeclarations(currentCompound);
            vector<DeclRefExpr*> references = getReferences(currentCompound);

            compounds.pop();

            for (VarDecl* violation : getUnusedDeclarations(declarations, references))
            {
                if (violation->isUsed())
                {
                    addViolation(violation, this, "The variable '" + violation->getNameAsString() + "' should be declared as local as possible.");
                }
            }

            // unusedDecl.clear();
        }
    }

    bool VisitCompoundStmt(CompoundStmt *node)
    {
        compounds.push(node);

        return true;
    }
};

static RuleSet rules(new MostLocalScopeRule());
