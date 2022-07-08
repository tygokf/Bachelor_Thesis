#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleConfiguration.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class RecursionRule : public AbstractASTVisitorRule<RecursionRule>
{
private:
    FunctionDecl *_currentFunc;
    string _ruleConfig;
    map<FunctionDecl*, vector<CallExpr*>> _recursiveCalls;

public:
    virtual const string name() const override
    {
        return "recursion";
    }

    virtual int priority() const override
    {
        return 2;
    }

    virtual const string category() const override
    {
        return "design";
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

    virtual void setUp() override
    {
        _ruleConfig = RuleConfiguration::stringForKey("RECURSION", "MANDATORY");
    }

    virtual void tearDown() override
    {
        if (_ruleConfig == "MANDATORY" && _recursiveCalls.empty())
        {
            string description = "A recursive function is mandatory.";
            addViolation(_currentFunc, this, description);
        }
        else if (_ruleConfig == "FORBIDDEN" && !_recursiveCalls.empty())
        {
            for (auto& kv : _recursiveCalls)
            {
                for (CallExpr *call : kv.second)
                {
                    string description = "This recursive function call to '" + kv.first->getNameAsString() + "' is not allowed.";
                    addViolation(call, this, description);
                }
            }
        }
    }
    
    bool VisitFunctionDecl(FunctionDecl *node)
    {
        _currentFunc = node;

        return true;
    }

    bool VisitCallExpr(CallExpr *node)
    {
        if (FunctionDecl *funcDecl = dyn_cast<FunctionDecl>(node->getCalleeDecl()))
        {
            if (funcDecl == _currentFunc)
            {
                _recursiveCalls[funcDecl].push_back(node);
            }
        }

        return true;
    }
};

static RuleSet rules(new RecursionRule());
