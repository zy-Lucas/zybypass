#pragma once

namespace hotspot::memory
{
#define MEMORY_TYPES_DO(f)                                                                                             \
    /* Memory type by sub systems. It occupies lower byte. */                                                          \
    f(mtJavaHeap, "Java Heap")                             /* Java heap                                 */             \
        f(mtClass, "Class")                                /* Java classes                              */             \
        f(mtThread, "Thread")                              /* thread objects                            */             \
        f(mtThreadStack, "Thread Stack") f(mtCode, "Code") /* generated code                            */             \
        f(mtGC, "GC") f(mtCompiler, "Compiler") f(mtJVMCI, "JVMCI")                                                    \
            f(mtInternal, "Internal")                            /* memory used by VM, but does not belong to */       \
        /* any of above categories, and not used by */           /* NMT                                       */       \
        f(mtOther, "Other")                                      /* memory not used by VM                     */       \
        f(mtSymbol, "Symbol") f(mtNMT, "Native Memory Tracking") /* memory used by NMT            */                   \
        f(mtClassShared, "Shared class space")                   /* class data sharing            */                   \
        f(mtChunk, "Arena Chunk")                                /* chunk that holds content of arenas        */       \
        f(mtTest, "Test")                                        /* Test type for verifying NMT               */       \
        f(mtTracing, "Tracing") f(mtLogging, "Logging") f(mtStatistics, "Statistics") f(mtArguments, "Arguments")      \
            f(mtModule, "Module") f(mtSafepoint, "Safepoint") f(mtSynchronizer, "Synchronization")                     \
                f(mtServiceability, "Serviceability") f(mtMetaspace, "Metaspace")                                      \
                    f(mtStringDedup, "String Deduplication") f(mtObjectMonitor, "Object Monitors")                     \
                        f(mtNone, "Unknown") // end

#define MEMORY_TYPE_DECLARE_ENUM(type, human_readable) type,

/*
 * Memory types
 */
enum class MEMFLAGS
{
    MEMORY_TYPES_DO(MEMORY_TYPE_DECLARE_ENUM)
    mt_number_of_types // number of memory types (mtDontTrack
                       // is not included as validate type)
};

#define MEMORY_TYPE_SHORTNAME(type, human_readable) constexpr MEMFLAGS type = MEMFLAGS::type;

// Generate short aliases for the enum values. E.g. mtGC instead of MEMFLAGS::mtGC.
MEMORY_TYPES_DO(MEMORY_TYPE_SHORTNAME)

// Make an int version of the sentinel end value.
constexpr int mt_number_of_types = static_cast<int>(MEMFLAGS::mt_number_of_types);
} // namespace hotspot::memory