#include "utils.h"

/* wstring cleanText(wstring text)
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
} */

wstring cleanText(wstring text)
{
    size_t pos = 0;
    while ((pos = text.find(L"(*", pos)) != wstring::npos)
    {
        size_t endPos = text.find(L"*)", pos);
        if (endPos != wstring::npos)
        {
            text.erase(pos, endPos - pos + 2); // +2 to remove "*)"
        }
        else
        {
            break; // No closing "*)", so we break the loop
        }
    }
    return text;
}

wstring cleanYaPar(wstring text)
{
    size_t pos = 0;
    while ((pos = text.find(L"/*", pos)) != wstring::npos)
    {
        size_t endPos = text.find(L"*/", pos);
        if (endPos != wstring::npos)
        {
            text.erase(pos, endPos - pos + 2); // +2 to remove "*)"
        }
        else
        {
            break; // No closing "*)", so we break the loop
        }
    }
    return text;
}