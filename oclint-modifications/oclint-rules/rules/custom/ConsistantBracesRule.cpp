#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex>
#include <stack>

using namespace std;
using namespace oclint;

class ConsistantBracesRule : public AbstractSourceCodeReaderRule
{
private:
    typedef struct brace_t {
        const int line;
        const int indent;
        brace_t(int line, int indent) : line(line), indent(indent) {}
    } brace_t;

    stack<brace_t> openingBraces;
    enum BraceType { NEW_LINE, SAME_LINE, UNDEFINED };
    BraceType fileBraceStyle = BraceType::UNDEFINED;

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
            default:
                return indent;
            }
        }

        return indent;
    }

    BraceType getBraceType(string line, int lineNumber)
    {
        smatch m;
        regex isSameLine(R"(.*\{\s*)");
        regex isNewLine(R"(\s*\{\s*)");

        if (regex_match(line, m, isNewLine)) return BraceType::NEW_LINE;
        if (regex_match(line, m, isSameLine)) return BraceType::SAME_LINE;

        return BraceType::UNDEFINED;
    }

    string getExpectedStyle()
    {
        string expected = ", expected '{'";
        
        switch (fileBraceStyle)
        {
        case BraceType::NEW_LINE:
            expected += " on their own line.";
            break;
        case BraceType::SAME_LINE:
            expected += " on the same line as the statement.";
            break;
        default:
            return ".";
        }

        return expected;
    }

    string sanitizeLine(string line)
    {
        regex isComment(R"(\/\/.*)");

        return regex_replace(line, isComment, "");
    }

public:
    virtual const string name() const override
    {
        return "consistant braces";
    }

    virtual int priority() const override
    {
        return 4;
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
        line = sanitizeLine(line);
        if (line.find("{") != string::npos)
        {
            BraceType current = getBraceType(line,lineNumber);

            if (fileBraceStyle == BraceType::UNDEFINED) fileBraceStyle = current;

            switch (current)
            {
            case BraceType::NEW_LINE:
            case BraceType::SAME_LINE:
                if (current != fileBraceStyle)
                {
                    string description = "The opening braces are not consistent across the file" + getExpectedStyle();
                    addViolation(lineNumber, 1, lineNumber, 1, this, description);
                }
                break;
            case BraceType::UNDEFINED:
                return;
            }

            openingBraces.push(brace_t(lineNumber, getIndent(line)));
        }
        if (line.find("}") != string::npos)
        {
            if (openingBraces.empty()) return;
            brace_t openingBrace = openingBraces.top();
            
            if (openingBrace.indent != getIndent(line))
            {
                string description = "The opening brace on line " + to_string(openingBrace.line) + " and closing brace on line " + to_string(lineNumber) + " do not have matching indentation.";
                addViolation(lineNumber, 1, lineNumber, 1, this, description);
            }

            openingBraces.pop();
        }
    }
};

static RuleSet rules(new ConsistantBracesRule());
