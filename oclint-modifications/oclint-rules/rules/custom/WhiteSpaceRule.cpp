#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex>
#include <vector>

using namespace std;
using namespace oclint;

class WhiteSpaceRule : public AbstractSourceCodeReaderRule
{
private:
    string sanitizeLine(string line)
    {
        regex isComment(R"(\/\/.*)");

        return regex_replace(line, isComment, "");
    }
    
    template <typename T>
    bool contains(vector<T> vec, T target)
    {
        for (T element : vec)
        {
            if (element == target)
            {
                return true;
            }
        }

        return false;
    }

    bool isWhitespace(char c)
    {
        return c == '\t' || c == ' ';
    }

    char getNextNonWhitespaceChar(string line)
    {
        char lastChar = line.find_last_not_of(' ');

        return lastChar == string::npos ? 0 : line.at(line.find_last_not_of(" \t\n\r\v\f"));
    }

    char getPrevNonWhitespaceChar(string line)
    {
        for (int i = line.size() - 1; i >= 0; i--)
        {
            if (!isWhitespace(line.at(i))) return line.at(i);
        }

        return 0;
    }

public:
    virtual const string name() const override
    {
        return "white space";
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
        // //TODO: add comma operator and remove commetns
        // vector<char> charOperators = { '=', '+', '-', '*', '/', '%', '<', '>',  '?', ':' };
        vector<char> charOperators = { '=', '+', '-', '/', '%', '?' };
        vector<string> stringOperators = { "+=", "-=", "/=", "*=", "%=", "==", "!=", "<=", ">=", "&&", "||", "<<", ">>" };
        vector<char> unaryOperators = { '+', '-' };

        line = sanitizeLine(line);

        for (int i = 0; i < line.size(); i++)
        {
            if (i < line.size() - 1 && contains(stringOperators, line.substr(i, 2)))
            {
                if (i > 0 && line.at(i - 1) != ' ' || i < line.size() - 2 && line.at(i + 2) != ' ')
                {
                    addViolation(lineNumber, 1, lineNumber, 1, this, "This line has incorrect spacing.");
                    return;
                }

                i++;
            }
            else if (contains(unaryOperators, line.at(i)))
            {
                char prevNonWhitespaceChar = getPrevNonWhitespaceChar(line.substr(0, i));
                char nextChar = line.at(i + 1);

                if (nextChar == '=')
                {
                    if (i > 0 && line.at(i - 1) != ' ' || i < line.size() - 2 && line.at(i + 2) != ' ')
                    {
                        addViolation(lineNumber, 1, lineNumber, 1, this, "This line has incorrect spacing.");
                        return;
                    }
                }
                else if (nextChar == line.at(i))
                {
                    if (i > 0 && line.at(i - 1) == ' ')
                    {
                        addViolation(lineNumber, 1, lineNumber, 1, this, "This line has incorrect spacing.");
                        return;
                    }
                }
                else if (prevNonWhitespaceChar == 0 || contains(charOperators, prevNonWhitespaceChar))
                {
                    if (i < line.size() - 1 && line.at(i + 1) == ' ')
                    {
                        addViolation(lineNumber, 1, lineNumber, 1, this, "This line has incorrect spacing.");
                        return;
                    }
                }
                else
                {
                    if (i > 0 && line.at(i - 1) != ' ' || i < line.size() - 1 && line.at(i + 1) != ' ')
                    {
                        addViolation(lineNumber, 1, lineNumber, 1, this, "This line has incorrect spacing.");
                        return;
                    }
                }
                
            }
            else if (contains(charOperators, line.at(i)))
            {
                if (i > 0 && line.at(i - 1) != ' ' || i < line.size() - 1 && line.at(i + 1) != ' ')
                {
                    addViolation(lineNumber, 1, lineNumber, 1, this, "This line has incorrect spacing.");
                    return;
                }
            }
        }
    }
};

static RuleSet rules(new WhiteSpaceRule());
