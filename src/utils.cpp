#include "utils.h"

wstring cleanText(wstring text)
{
    wstring::iterator it = text.begin();
    wstring::iterator end = text.end();
    wstring::iterator current = it;
    wstring::iterator commentStart = it;
    wstring::iterator commentEnd = it;

    while (current != end)
    {
        if (*current == L'(' && current + 1 != text.end() && *(current + 1) == L'*')
        {
            commentStart = current;
            if (current + 2 != text.end())
            {
                current = current + 2;
                while (current != text.end())
                {
                    if (*current == L'(' && current + 1 != text.end() && *(current + 1) == L'*')
                    {
                        current--;
                        break;
                    }
                    if (*current == L'*' && current + 1 != text.end() && *(current + 1) == L')')
                    {
                        commentEnd = current + 1;
                        current = text.erase(commentStart, commentEnd + 1);
                        break;
                    }
                    current++;
                }
                if (current == text.end())
                {
                    break;
                }
            }
        }
        current++;
    }
    return text;
}