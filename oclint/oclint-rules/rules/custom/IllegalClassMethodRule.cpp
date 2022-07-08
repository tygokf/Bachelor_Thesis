#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleConfiguration.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class IllegalClassMethodRule : public AbstractASTVisitorRule<IllegalClassMethodRule>
{
private:
    string _ruleConfig;
    bool whiteListMode;
    bool hasCorrectConfig;
    map<string, vector<string>> listedMethodsPerClass;
    
    bool setMode()
    {
        char mode = _ruleConfig.front();

        switch (mode)
        {
        case 'w':
        case 'W':
            whiteListMode = true;
            break;
        case 'b':
        case 'B':
            whiteListMode = false;
            break;
        
        default:
            return false;
        }

        size_t delim_loc = _ruleConfig.find('-');
        _ruleConfig.erase(0, ++delim_loc);

        return true;
    }

    bool addMethodToMap(string methodName, string className)
    {
        if (className.empty())
        {
            return false;
        }

        if (listedMethodsPerClass.count(className))
        {
            listedMethodsPerClass[className].push_back(methodName);
        }
        else
        {
            listedMethodsPerClass[className] = { methodName };
        }

        return true;
    }

    bool loadRuleConfig()
    {
        string className;
        string buffer;

        for (char c : _ruleConfig)
        {
            switch (c)
            {
                case ':':
                    className = buffer;
                    buffer.clear();
                    break;
                case ',':
                    if(!addMethodToMap(buffer, className)) return false;
                    buffer.clear();
                    break;
                case ';':
                    if (className.empty()) 
                    {
                        className = buffer;
                        buffer.clear();
                    }
                    if(!addMethodToMap(buffer, className)) return false;
                    buffer.clear();
                    className.clear();
                    break;
                default:
                    buffer.push_back(c);
                    break;
            }
        }

        if (!buffer.empty())
        {
            if(!addMethodToMap(buffer, className)) return false;
        }

        return true;
    }

    bool isListedClass(string name)
    {
        return listedMethodsPerClass.count(name);
    }

    bool isListedClassMethod(string className, string methodName)
    {
        for (string listedMethod : listedMethodsPerClass[className])
        {
            if (listedMethod == methodName)
            {
                return true;
            }
        }

        return false;
    }

public:
    virtual const string name() const override
    {
        return "illegal class method";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual const string category() const override
    {
        return "convention";
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

    virtual void setUp() override {
        _ruleConfig = RuleConfiguration::stringForKey("ILLEGAL_CLASS_METHOD", "B");
        hasCorrectConfig = setMode() && loadRuleConfig();
    }
    virtual void tearDown() override {}

    bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr *node)
    {
        if (OverloadedOperatorKind::OO_Subscript == node->getOperator())
        {
            for (Expr *arg : node->arguments())
            {
                if (DeclRefExpr *declRefExpr = dyn_cast<DeclRefExpr>(arg))
                {
                    if (VarDecl *varDecl = dyn_cast<VarDecl>(declRefExpr->getFoundDecl()))
                    {
                        string type = varDecl->getType().getAsString();
                        if (type.find("vector<") != string::npos)
                        {
                            addViolation(node, this, "The [] operator is not allowed on vectors.");
                        }
                        
                    }
                }
            }
        }

        return true;
    }

    bool VisitCXXMemberCallExpr(CXXMemberCallExpr *node)
    {
        if (hasCorrectConfig)
        {
            string foundClass = node->getRecordDecl()->getNameAsString();
            string foundMethod = node->getMethodDecl()->getNameAsString();

            if (isListedClass(foundClass) && isListedClassMethod(foundClass, foundMethod) != whiteListMode)
            {
                addViolation(node, this, "The method '" + foundMethod + "' is not allowed on the class '" + foundClass + "'.");
            }
        }

        return true;
    }

};

static RuleSet rules(new IllegalClassMethodRule());
