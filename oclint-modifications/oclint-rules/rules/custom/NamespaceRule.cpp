#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex>

using namespace std;
using namespace oclint;

class NamespaceRule : public AbstractSourceCodeReaderRule
{
private:
    string sanitizeLine(string line)
    {
        regex isComment(R"(\/\/.*)");

        return regex_replace(line, isComment, "");
    }
public:
    virtual const string name() const override
    {
        return "namespace";
    }

    virtual int priority() const override
    {
        return 3;
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

    virtual void eachLine(int lineNumber, string line) override
    {
        smatch m;
        regex namespaceStd("\\s*using\\s+namespace\\s+std\\s*;\\s*");
        
        line = sanitizeLine(line);

        if (regex_match(line, m, namespaceStd)) {
            string description = "Do not use 'using namespace std;'.";
            addViolation(lineNumber, 1, lineNumber, 1, this, description);
        }
    }
};

static RuleSet rules(new NamespaceRule());
