#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex>

using namespace std;
using namespace oclint;

class IndentationRule : public AbstractSourceCodeReaderRule
{
private:
    int depth = 0;
    int indentSize = 0; 

    int getIndent(string line)
    {
        int indent = 0;

        for (char c : line)
        {
            switch (c)
            {
            case ' ':
                indent++;
                break;
            case '\t':
                indent += 4;
                break;
            case '\r':
                return -1;
            default:
                return indent;
            }
        }

        return -1;
    }

    string sanitizeLine(string line)
    {
        regex isComment(R"(\/\/.*)");

        return regex_replace(line, isComment, "");
    }
public:
    virtual const string name() const override
    {
        return "indentation";
    }

    virtual int priority() const override
    {
        return 4;
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

    virtual void eachLine(int lineNumber, string line) override
    {
        line = sanitizeLine(line);
        int currentIndent = getIndent(line);

        if (currentIndent == -1) return;
        else if (indentSize == 0) indentSize = currentIndent;

        if (line.find('{') != string::npos && line.find('}') == string::npos)
        {
            depth++;
        }
        else if (line.find('}') != string::npos)
        {
            depth--;
        }
        else if (depth * indentSize != currentIndent && indentSize != 0) {
            string description = "";

            if (currentIndent % indentSize == 0)
            {
                description = "The indentation does not represent the coding's nesting structure.";
            }
            else
            {
                description = "The indentation is inconsistent, expected an indentation of a multiple of " + to_string(indentSize) + ".";
            }
        }
    }
};

static RuleSet rules(new IndentationRule());
