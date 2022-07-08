#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex>
#include <queue>

using namespace std;
using namespace oclint;

class EmptyLineRule : public AbstractSourceCodeReaderRule
{
private:
    enum StateType { INCLUDE, INSIDE, OUTSIDE, UNDEFINED };
    StateType currentState = StateType::UNDEFINED;
    int emptyLineCount = 0;
    int braceCount = 0;
    bool emptyLineExpected = false;

    bool checkIfInclude(string line)
    {
        return line.find("#include") != string::npos;
    }

    bool checkIfEmptyLine(string line)
    {
        for (char c : line)
        {
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;

            return false;
        }

        return true;
    }

    bool checkIfInside(string line)
    {
        bool prevIsSlash = false;

        for (char c : line)
        {
            switch (c)
            {
            case '/':
                if (prevIsSlash) return braceCount;
                prevIsSlash = true;
                continue;;
            case '{':
                braceCount++;
                break;
            case '}':
                braceCount--;
                break;
            default:
                break;
            }

            prevIsSlash = false;
        }

        return braceCount;
    }

    void updateState(string line)
    {
        switch (currentState)
        {
        case StateType::UNDEFINED:
            if (checkIfInclude(line)) currentState = StateType::INCLUDE;
            else if (checkIfInside(line)) currentState = StateType::INSIDE;
            break;
        case StateType::INCLUDE:
            if (!checkIfInclude(line)) currentState = StateType::OUTSIDE;
            break;
        case StateType::OUTSIDE:
            if (checkIfInside(line)) currentState = StateType::INSIDE;
            break;
        case StateType::INSIDE:
            if (!checkIfInside(line))
            {
                currentState = StateType::OUTSIDE;
                emptyLineExpected = true;
            } 
            break;
        }

        return;
    }

    int getEmptyLineThreshold()
    {
        return currentState;
    }

public:
    virtual const string name() const override
    {
        return "empty line";
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
        string description;

        if (emptyLineExpected && !checkIfEmptyLine(line))
        {
            description = "An empty line should be used to seperate this statement and the one above.";
        }
        
        emptyLineExpected = false;
        updateState(line);

        if (checkIfEmptyLine(line))
        {
            emptyLineCount++;
            if (emptyLineCount > getEmptyLineThreshold()) description = "This empty line is excessive.";
        }
        else emptyLineCount = 0;

        if (currentState == StateType::OUTSIDE && checkIfInclude(line))
        {
            description = "The include statements should be grouped together without any empty lines.";
        }

        if (!description.empty()) addViolation(lineNumber, 1, lineNumber, 1, this, description);
    }
};

static RuleSet rules(new EmptyLineRule());
