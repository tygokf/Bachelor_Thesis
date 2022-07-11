#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleConfiguration.h"
#include "clang/Basic/SourceLocation.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class TooLittleClassesRule : public AbstractASTVisitorRule<TooLittleClassesRule>
{
private:
    int _threshold;
    int _classCount;
    RecordDecl *_classDecl;
    Stmt *_firstStmt;

    string chooseSingularOrPlural(string singular, string plural, int value)
    {
        return value == 1 ? singular : plural;
    }

public:
    virtual const string name() const override
    {
        return "too little classes";
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
        _threshold = RuleConfiguration::intForKey("TOO_LITTLE_CLASSES", 1);
        _classCount = 0;
        _classDecl = nullptr;
        _firstStmt = nullptr;
    }
    virtual void tearDown() override 
    {
        if (_classCount < _threshold) 
        {
            string description = "This file contains " + to_string(_classCount) + 
                chooseSingularOrPlural(" class", " classes", _classCount) + ", while " + to_string(_threshold) +
                chooseSingularOrPlural(" class is", " classes are", _threshold) + " expected.";
            
            _classDecl ? addViolation(_classDecl, this, description) : addViolation(_firstStmt, this, description);
        }
    }

    bool VisitCXXRecordDecl(CXXRecordDecl *node)
    {
        if (node->isClass())
        {
            if (_classDecl == nullptr)
            {
                _classDecl = node;
            }

            _classCount++;
            addViolation(node, this, node->getNameAsString());
        }

        return true;
    }

    bool VisitStmt(Stmt *node)
    {
        if (_firstStmt == nullptr)
        {
            _firstStmt = node;
        }

        return false;
    }
};

static RuleSet rules(new TooLittleClassesRule());
