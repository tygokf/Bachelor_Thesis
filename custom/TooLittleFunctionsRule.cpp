#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleConfiguration.h"
#include "oclint/RuleSet.h"

#include <iostream>

using namespace std;
using namespace clang;
using namespace oclint;

class TooLittleFunctionsRule : public AbstractASTVisitorRule<TooLittleFunctionsRule>
{
private:
    int _threshold;
    int _funcCount;
    FunctionDecl *_mainDecl;

    string chooseSingularOrPlural(string singular, string plural, int value)
    {
        return value == 1 ? singular : plural;
    }

public:
    virtual const string name() const override
    {
        return "too little functions";
    }

    virtual int priority() const override
    {
        return 2;
    }

    virtual const string category() const override
    {
        return "size";
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
        _threshold = RuleConfiguration::intForKey("TOO_LITTLE_FUNCTIONS", 1);
        _funcCount = 0;
        _mainDecl = nullptr;
    }
    virtual void tearDown() override 
    {
        if (_funcCount < _threshold) 
        {
            string description = "This file contains " + to_string(_funcCount) + " user-defined " + 
                chooseSingularOrPlural("function", "functions", _funcCount) + " beyond main, while " + to_string(_threshold) +
                chooseSingularOrPlural(" function is", " functions are", _threshold) + " expected.";
            
            addViolation(_mainDecl, this, description);
        }
    }

    bool VisitFunctionDecl(FunctionDecl *node)
    {
        if (node->isMain())
        {
            _mainDecl = node;
        }
        else if (node->isThisDeclarationADefinition())
        {
            _funcCount++;
        }

        return true;
    }
};

static RuleSet rules(new TooLittleFunctionsRule());
