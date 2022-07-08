#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class DeclareConstRule : public AbstractASTVisitorRule<DeclareConstRule>
{
private:
    class ConstantAnalyzer : public RecursiveASTVisitor<ConstantAnalyzer>
    {
    private:
        bool _isConstant = true;
        VarDecl *_target;

    public:
        bool isConstant(FunctionDecl *decl, VarDecl* variable)
        {
            _target = variable;
            (void) TraverseDecl(decl);

            return _isConstant;
        }

        void setConstantFalseIfTarget(DeclRefExpr *expr)
        {
            if (expr->getFoundDecl() == _target)
            {
                _isConstant = false;
            }
        }

        bool VisitUnaryOperator(UnaryOperator *op) {
            if (op->isIncrementDecrementOp()) {
                if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(op->getSubExpr()))
                {
                    setConstantFalseIfTarget(declRefExpr);
                }
            }

            return true;
        }

        bool VisitBinaryOperator(BinaryOperator *op)
        {
            Expr *leftExpr = op->getLHS();

            if (op->getOpcode() == BO_Assign && leftExpr && isa<DeclRefExpr>(leftExpr))
            {
                DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(leftExpr);

                setConstantFalseIfTarget(declRefExpr);
            }

            return true;
        }

        bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr *callExpr)
        {
            if (callExpr->getOperator() == OverloadedOperatorKind::OO_GreaterGreater)
            {
                for (Expr *arg : callExpr->arguments())
                {
                    if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(arg))
                    {
                        setConstantFalseIfTarget(declRefExpr);
                    }
                    else if (CastExpr *castExpr = dyn_cast<CastExpr>(arg))
                    {
                        if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(castExpr->getSubExpr()))
                        {
                           setConstantFalseIfTarget(declRefExpr);
                        }
                    }
                    
                }
            }

            return true;
        }

        bool VisitCallExpr(CallExpr *callExpr)
        {
            for (Expr *arg : callExpr->arguments())
            {
                if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(arg))
                {
                    setConstantFalseIfTarget(declRefExpr);
                }                
            }

            return true;
        }

        bool VisitCXXMemberCallExpr(CXXMemberCallExpr *node)
        {
            CXXMethodDecl *methodDecl = node->getMethodDecl();

            if (!methodDecl->getMethodQualifiers().hasConst())
            {
                if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(node->getImplicitObjectArgument()))
                {
                    setConstantFalseIfTarget(declRefExpr);
                }
            }

            return true;
        }
    };

    FunctionDecl *_currentFunc;

    void checkIfParmNotConstant(VarDecl* variable) {
        _checkIfNotConstant(variable, "parameter");
    }

    void checkIfVarNotConstant(VarDecl* variable) {
        _checkIfNotConstant(variable, "variable");
    }

    void _checkIfNotConstant(VarDecl* variable, string type)
    {
        ConstantAnalyzer Analyzer;
        
        if (Analyzer.isConstant(_currentFunc, variable))
        {
            string description = "The read-only " + type + " '" + variable->getNameAsString() + "' should be declared as const.";
            addViolation(variable, this, description);
        }
    }

public:
    virtual const string name() const override
    {
        return "declare const";
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
    virtual void tearDown() override {}

    bool VisitFunctionDecl(FunctionDecl *node)
    {
        _currentFunc = node;

        return true;
    }

    bool VisitParmVarDecl(ParmVarDecl *node)
    {
        string type = node->getType().getAsString();

        if (type.find('&') != string::npos && type.find("const ") == string::npos)
        {
            if (node->isUsed() && node->getNameAsString() != "")
            {
                checkIfParmNotConstant(node);
            }
        }

        return true;
    }

    bool VisitVarDecl(VarDecl *node)
    {
        string type = node->getType().getAsString();

        if (node->isLocalVarDecl() && node->isUsed() && type.find("const ") == string::npos) {
            checkIfVarNotConstant(node);
        }

        return true;
    }
};

static RuleSet rules(new DeclareConstRule());
