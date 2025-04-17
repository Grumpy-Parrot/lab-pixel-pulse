#pragma once

#ifndef PIXELPULSE_MEMORY_H
#define PIXELPULSE_MEMORY_H

#include "Platform/Std.h"

void PP_MemorySystemInitialize();     // Initialize the memory system
void PP_MemorySystemEnableTracking(); // Begin tracking memory allocations
void PP_MemorySystemShutdown();       // Shutdown the memory system and report leaks
void PP_MemorySystemDumpStats();      // Print current memory statistics

namespace PixelPulse::Platform::Memory
{
    struct MemoryStats
    {
        std::size_t totalAllocations;      // Total number of allocations
        std::size_t currentAllocations;    // Current number of active allocations
        std::size_t totalBytesAllocated;   // Total bytes allocated since start
        std::size_t currentBytesAllocated; // Current bytes in use
        std::size_t peakBytesAllocated;    // Peak memory usage
    };

    struct AllocationRecord
    {
        void *address;        // Memory address
        std::size_t size;     // Size of allocation in bytes
        const char *file;     // Source file
        int line;             // Line number
        const char *function; // Function name
    };

    class MemoryAllocator
    {
    public:
        static MemoryAllocator &getInstance();

        void *allocate(std::size_t size, const char *file = nullptr, int line = 0, const char *function = nullptr);
        void *reallocate(void *ptr, std::size_t newSize, const char *file = nullptr, int line = 0, const char *function = nullptr);
        void deallocate(void *ptr);

        const MemoryStats &getStats() const;
        void dumpLeaks() const;
        void resetStats();

    private:
        MemoryAllocator();
        ~MemoryAllocator();

        MemoryAllocator(const MemoryAllocator &) = delete;
        MemoryAllocator &operator=(const MemoryAllocator &) = delete;

        std::unordered_map<void *, AllocationRecord> m_allocations;
        MemoryStats m_stats;
        std::mutex m_mutex;
    };

    template <typename T, typename... Args>
    T *allocateObject(const char *file, int line, const char *function, Args &&...args)
    {
        void *memory = PixelPulse::Platform::Memory::MemoryAllocator::getInstance().allocate(
            sizeof(T), file, line, function);
        return new (memory) T(std::forward<Args>(args)...);
    }

    template <typename T>
    void freeObject(T *ptr)
    {
        if (ptr)
        {
            ptr->~T();
            PixelPulse::Platform::Memory::MemoryAllocator::getInstance().deallocate(ptr);
        }
    }

    template <typename T>
    T *allocateArray(size_t count, const char *file, int line, const char *function)
    {
        void *memory = PixelPulse::Platform::Memory::MemoryAllocator::getInstance().allocate(
            sizeof(T) * count, file, line, function);

        T *typedMemory = static_cast<T *>(memory);
        for (size_t i = 0; i < count; ++i)
        {
            new (&typedMemory[i]) T();
        }

        return typedMemory;
    }

    template <typename T>
    void freeArray(T *array, size_t count)
    {
        if (array)
        {
            for (size_t i = 0; i < count; ++i)
            {
                array[i].~T();
            }
            PixelPulse::Platform::Memory::MemoryAllocator::getInstance().deallocate(array);
        }
    }

    void *allocate(size_t size, const char *file, int line, const char *function);
    void *reallocate(void *ptr, size_t newSize, const char *file, int line, const char *function);
    void free(void *ptr);
}

#define PP_NEW(Type, ...) \
    ::PixelPulse::Platform::Memory::allocateObject<Type>(__FILE__, __LINE__, __FUNCTION__ __VA_OPT__(, ) __VA_ARGS__)

#define PP_DELETE(ptr) \
    ::PixelPulse::Platform::Memory::freeObject(ptr)

#define PP_NEW_ARRAY(Type, count) \
    ::PixelPulse::Platform::Memory::allocateArray<Type>(count, __FILE__, __LINE__, __FUNCTION__)

#define PP_DELETE_ARRAY(ptr, count) \
    ::PixelPulse::Platform::Memory::freeArray(ptr, count)

#define PP_MALLOC(size) \
    ::PixelPulse::Platform::Memory::allocate(size, __FILE__, __LINE__, __FUNCTION__)

#define PP_FREE(ptr) \
    ::PixelPulse::Platform::Memory::free(ptr)

#define PP_REALLOC(ptr, size) \
    ::PixelPulse::Platform::Memory::reallocate(ptr, size, __FILE__, __LINE__, __FUNCTION__)

#endif
