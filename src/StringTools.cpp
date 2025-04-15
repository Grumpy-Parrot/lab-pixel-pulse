#include "StringTools.h"

char *CString::join(const char *a, const char *b)
{
    if (!a)
    {
        a = "";
    }

    if (!b)
    {
        b = "";
    }

    size_t len_a = std::strlen(a);
    size_t len_b = std::strlen(b);
    size_t total_len = len_a + len_b + 1; // +1 for null terminator

    char *result = (char *)std::malloc(total_len);
    if (!result)
    {
        return nullptr;
    }

#ifdef PLATFORM_WINDOWS
    strcpy_s(result, total_len, a);
    strcat_s(result, total_len, b);
#else
    std::strcpy(result, a);
    std::strcat(result, b);
#endif

    return result;
}
