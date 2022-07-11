#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class InitVariableRule : public AbstractASTVisitorRule<InitVariableRule>
{
private:
    vector<VarDecl*> noInit;
    vector<VarDecl*> exceptions;

    bool contains(vector<VarDecl*> vec, VarDecl* varDecl)
    {
        for (VarDecl *currDecl : vec)
        {
            if (currDecl == varDecl)
            {
                return true;
            }
        }

        return false;
    }
public:
    virtual const string name() const override
    {
        return "init variable";
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
        for (VarDecl *varDecl : noInit)
        {
            if (!contains(exceptions, varDecl))
            {
                string description = "The variable '" + varDecl->getNameAsString() + "' should be initialised.";
                addViolation(varDecl, this, description);
            }
        }  
    }

    bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr *callExpr)
    {
        DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(callExpr->getArg(1));

        if (callExpr->getOperator() == OverloadedOperatorKind::OO_GreaterGreater)
        {
            if (VarDecl *varDecl = dyn_cast<VarDecl>(declRefExpr->getFoundDecl()))   
            {
                exceptions.push_back(varDecl);
            }
        }

        return true;
    }

    bool VisitCXXCatchStmt(CXXCatchStmt *node)
    {
        exceptions.push_back(node->getExceptionDecl());

        return true;
    }

    bool VisitVarDecl(VarDecl *node)
    {
        if (!node->hasInit() && node->isLocalVarDecl())
        {
            noInit.push_back(node);
        }

        return true;
    }
};

static RuleSet rules(new InitVariableRule());
