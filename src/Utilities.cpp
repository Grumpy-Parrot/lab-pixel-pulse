#include "Utilities.h"

const char* PIXELPULSE_allocateId(size_t length)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    constexpr size_t charset_size = sizeof(charset) - 1;

    char* id = (char*)std::malloc(length + 1);
    if (!id) {
        return nullptr;
    }

    for (size_t i = 0; i < length; ++i)
    {
        id[i] = charset[rand() % charset_size];
    }
    id[length] = '\0';
    return id;
}

const char* PIXELPULSE_deriveId(const char* in, size_t length)
{
    if (!in) {
        return nullptr;
    }

    static const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    constexpr size_t charset_size = sizeof(charset) - 1;
    std::uint64_t hash = 5381; // djb2 hash initial value

    char* id = (char*)std::malloc(length + 1);
    if (!id) {
        return nullptr;
    }

    const size_t len = strlen(in);
    for (size_t i = 0; i < len; ++i)
    {
        hash = ((hash << 5) + hash) + (unsigned char)in[i]; // djb2 algorithm
    }

    for (size_t i = 0; i < length; ++i)
    {
        id[i] = charset[hash % charset_size];
        if constexpr (charset_size > 0) {
             hash /= charset_size;
        } else {
             hash = 0;
        }

    }
    id[length] = '\0';
    return id;
}

void PIXELPULSE_freeId(const char* id)
{
    if (id)
    {
        std::free((void*)id);
    }
}
