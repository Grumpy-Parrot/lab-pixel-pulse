#include "Memory.h"
#include "../Logger.h"
#include "Platform.h"

using namespace PixelPulse::Platform;

static bool g_memorySystemActive = false;
static bool g_memorySystemInitialized = false;

void PP_MemorySystemInitialize()
{
    if (!g_memorySystemInitialized)
    {
        PixelPulse::Platform::Memory::MemoryAllocator::getInstance();
        g_memorySystemInitialized = true;
        PixelPulse::Logger::info("Memory system initialized");
    }

#ifdef PIXELPULSE_TRACK_MEMORY
    PP_MemorySystemEnableTracking();
#endif
}

void PP_MemorySystemEnableTracking()
{
    if (g_memorySystemInitialized && !g_memorySystemActive)
    {
        g_memorySystemActive = true;
        PixelPulse::Logger::info("Memory tracking activated");
    }
}

void PP_MemorySystemShutdown()
{
    if (g_memorySystemActive)
    {
        PixelPulse::Platform::Memory::MemoryAllocator::getInstance().dumpLeaks();
        g_memorySystemActive = false;
        PixelPulse::Logger::info("Memory system deactivated");
    }
}

void PP_MemorySystemDumpStats()
{
    if (g_memorySystemActive)
    {
        const auto &stats = PixelPulse::Platform::Memory::MemoryAllocator::getInstance().getStats();
        PixelPulse::Logger::info("Memory stats: %zu active allocations, %zu bytes in use, %zu peak bytes",
                                 stats.currentAllocations, stats.currentBytesAllocated, stats.peakBytesAllocated);
    }
}

using namespace PixelPulse::Platform::Memory;

MemoryAllocator &MemoryAllocator::getInstance()
{
    static MemoryAllocator instance;
    return instance;
}

MemoryAllocator::MemoryAllocator()
{
    resetStats();
    PixelPulse::Logger::info("Memory allocator initialized");
}

MemoryAllocator::~MemoryAllocator()
{
    PixelPulse::Logger::info("Memory allocator shutdown. Total allocations: %zu, Peak memory usage: %zu bytes",
                             m_stats.totalAllocations, m_stats.peakBytesAllocated);
}

void *MemoryAllocator::allocate(std::size_t size, const char *file, int line, const char *function)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    void *ptr = ::malloc(size);

    if (ptr && g_memorySystemActive)
    {
        m_stats.totalAllocations++;
        m_stats.currentAllocations++;
        m_stats.totalBytesAllocated += size;
        m_stats.currentBytesAllocated += size;

        if (m_stats.currentBytesAllocated > m_stats.peakBytesAllocated)
        {
            m_stats.peakBytesAllocated = m_stats.currentBytesAllocated;
        }

        AllocationRecord record;
        record.address = ptr;
        record.size = size;
        record.file = file;
        record.line = line;
        record.function = function;

        m_allocations[ptr] = record;
    }
    else if (!ptr)
    {
        Logger::error("Memory allocation failed! Requested size: %zu bytes", size);
    }

    return ptr;
}

void *MemoryAllocator::reallocate(void *ptr, std::size_t newSize, const char *file, int line, const char *function)
{
    if (!g_memorySystemActive)
    {
        return ::realloc(ptr, newSize);
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if (!ptr)
    {
        return allocate(newSize, file, line, function);
    }

    auto it = m_allocations.find(ptr);
    if (it == m_allocations.end())
    {
        return ::realloc(ptr, newSize);
    }

    std::size_t oldSize = it->second.size;

    m_stats.currentBytesAllocated -= oldSize;

    void *newPtr = ::realloc(ptr, newSize);

    if (newPtr)
    {
        m_stats.totalAllocations++;
        m_stats.totalBytesAllocated += newSize;
        m_stats.currentBytesAllocated += newSize;

        if (m_stats.currentBytesAllocated > m_stats.peakBytesAllocated)
        {
            m_stats.peakBytesAllocated = m_stats.currentBytesAllocated;
        }

        m_allocations.erase(it);

        AllocationRecord record;
        record.address = newPtr;
        record.size = newSize;
        record.file = file;
        record.line = line;
        record.function = function;

        m_allocations[newPtr] = record;
    }
    else
    {
        m_stats.currentBytesAllocated += oldSize;
        Logger::error("Memory reallocation failed! Current size: %zu bytes, Requested size: %zu bytes", oldSize, newSize);
    }

    return newPtr;
}

void MemoryAllocator::deallocate(void *ptr)
{
    if (!ptr)
        return;

    if (!g_memorySystemActive)
    {
        ::free(ptr);
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_allocations.find(ptr);
    if (it != m_allocations.end())
    {
        m_stats.currentAllocations--;
        m_stats.currentBytesAllocated -= it->second.size;

        m_allocations.erase(it);

        ::free(ptr);
    }
    else
    {
        ::free(ptr);
    }
}

const MemoryStats &MemoryAllocator::getStats() const
{
    return m_stats;
}

void MemoryAllocator::resetStats()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_stats.totalAllocations = 0;
    m_stats.currentAllocations = 0;
    m_stats.totalBytesAllocated = 0;
    m_stats.currentBytesAllocated = 0;
    m_stats.peakBytesAllocated = 0;

    m_allocations.clear();
}

void MemoryAllocator::dumpLeaks() const
{
    if (!g_memorySystemActive)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(m_mutex));

    if (m_allocations.empty())
    {
        Logger::info("No memory leaks detected");
        return;
    }

    Logger::warning("Memory leaks detected: %zu allocations, %zu bytes",
                    m_allocations.size(), m_stats.currentBytesAllocated);

    for (const auto &pair : m_allocations)
    {
        const AllocationRecord &record = pair.second;
        if (record.file && record.function)
        {
            Logger::warning("Leak: %zu bytes at %p - %s:%d in %s",
                            record.size, record.address, record.file, record.line, record.function);
        }
        else
        {
            Logger::warning("Leak: %zu bytes at %p - unknown location",
                            record.size, record.address);
        }
    }
}

void *Memory::allocate(size_t size, const char *file, int line, const char *function)
{
    return MemoryAllocator::getInstance().allocate(size, file, line, function);
}

void *Memory::reallocate(void *ptr, size_t newSize, const char *file, int line, const char *function)
{
    return MemoryAllocator::getInstance().reallocate(ptr, newSize, file, line, function);
}

void Memory::free(void *ptr)
{
    MemoryAllocator::getInstance().deallocate(ptr);
}
