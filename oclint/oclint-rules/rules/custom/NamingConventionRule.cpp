#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class NamingConventionRule : public AbstractASTVisitorRule<NamingConventionRule>
{
public:
    virtual const string name() const override
    {
        return "naming convention";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return "naming";
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

    bool VisitCXXRecordDecl(CXXRecordDecl *node)
    {
        string className = node->getNameAsString();

        if (node->isClass() && islower(className.front())) {
            string description = "the class name '" + className + "' should start with an uppercase letter.";
            addViolation(node, this, description);
        }

        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *node)
    {
        string funcName = node->getNameAsString();
        if (!isa<CXXConstructorDecl>(node) && isupper(funcName[0])) {
            string description = "the function name '" + funcName + "' should start with a lowercase letter.";
            addViolation(node, this, description);
        }

        return true;
    }

    bool VisitVarDecl(VarDecl *node)
    {
        string varName = node->getNameAsString();

        if (node->hasConstantInitialization()) {
            for (const char &c : varName) {
                if (islower(c)) {
                    string description = "the const variable name '" + varName + "' should not contain lowercase letters.";
                    addViolation(node, this, description);

                    break;
                }
            }
        }
        else {
            if (isupper(varName.front())) {
                string description = "the variable name '" + varName + "' should start with a lowercase letter.";
                addViolation(node, this, description);
            }
        }

        return true;
    }
};

static RuleSet rules(new NamingConventionRule());
