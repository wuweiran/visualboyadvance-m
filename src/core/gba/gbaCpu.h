#ifndef VBAM_CORE_GBA_GBACPU_H_
#define VBAM_CORE_GBA_GBACPU_H_

#include <cstdint>

#include "core/base/system.h"
#include "core/gba/gbaCheats.h"
#include "core/gba/gbaGlobals.h"

extern int armExecute();
extern int thumbExecute();

// regparm only has an effect on 32-bit x86; on x86_64 it is ignored by the
// compiler and emits a -Wattributes warning, so restrict it to __i386__.
#if defined(__i386__)
#define INSN_REGPARM __attribute__((regparm(1)))
#else
#define INSN_REGPARM /*nothing*/
#endif

#ifdef __GNUC__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define UPDATE_REG(address, value)                 \
    {                                              \
        WRITE16LE(((uint16_t*)&g_ioMem[address]), value); \
    }

#define ARM_PREFETCH                                        \
    {                                                       \
        cpuPrefetch[0] = CPUReadMemoryQuick(armNextPC);     \
        cpuPrefetch[1] = CPUReadMemoryQuick(armNextPC + 4); \
    }

#define THUMB_PREFETCH                                        \
    {                                                         \
        cpuPrefetch[0] = CPUReadHalfWordQuick(armNextPC);     \
        cpuPrefetch[1] = CPUReadHalfWordQuick(armNextPC + 2); \
    }

#define ARM_PREFETCH_NEXT cpuPrefetch[1] = CPUReadMemoryQuick(armNextPC + 4);

#define THUMB_PREFETCH_NEXT cpuPrefetch[1] = CPUReadHalfWordQuick(armNextPC + 2);

extern int SWITicks;
extern uint32_t mastercode;
extern bool busPrefetch;
extern bool busPrefetchEnable;
extern uint32_t busPrefetchCount;     // legacy bitmask (kept for save/restore)
extern int busPrefetchHalfwords;       // previous instruction's fractional prefetch progress
extern int busPrefetchAccum;           // cycles accumulated toward next halfword
extern int busPrefetchFrac;            // persistent prefetch-depth carry (mGBA model)
extern int cpuNextEvent;
extern bool holdState;
extern uint32_t cpuPrefetch[2];
extern int cpuTotalTicks;
extern uint8_t memoryWait[16];
extern uint8_t memoryWait32[16];
extern uint8_t memoryWaitSeq[16];
extern uint8_t memoryWaitSeq32[16];
extern uint8_t cpuBitsSet[256];
extern uint8_t cpuLowestBitSet[256];

// Per-cycle bus-state simulator helpers (scaffolding for a future
// halfword-FIFO prefetch model). See project_dma_cycle_rework.md for the
// roadmap and the hill-climbing iteration log demonstrating why the
// rewrite cannot complete in a single session.

extern void CPUSwitchMode(int mode, bool saveState, bool breakLoop);
extern void CPUSwitchMode(int mode, bool saveState);
extern void CPUUpdateCPSR();
extern void CPUUpdateFlags(bool breakLoop);
extern void CPUUpdateFlags();
extern void CPUUndefinedException();
extern void CPUSoftwareInterrupt();
extern void CPUSoftwareInterrupt(int comment);

extern bool g_dmaBrokeBurst;  // first cart access after a DMA is non-seq
extern bool g_dmaBreakSkipOne; // that access slipped in pre-DMA; skip it

// Waitstates when accessing data
inline int dataTicksAccess16([[maybe_unused]] uint32_t address) { return 0; }
inline int dataTicksAccess32([[maybe_unused]] uint32_t address) { return 0; }
inline int dataTicksAccessSeq16([[maybe_unused]] uint32_t address) { return 0; }
inline int dataTicksAccessSeq32([[maybe_unused]] uint32_t address) { return 0; }

inline int codeTicksAccess16([[maybe_unused]] uint32_t address) { return 0; }
inline int codeTicksAccess32([[maybe_unused]] uint32_t address) { return 0; }
inline int codeTicksAccessSeq16([[maybe_unused]] uint32_t address) { return 0; }
inline int codeTicksAccessSeq32([[maybe_unused]] uint32_t address) { return 0; }

inline int busPrefetchRomFloor([[maybe_unused]] int legacy, int pure,
    [[maybe_unused]] int halfwords, [[maybe_unused]] bool cartData)
{
    return pure;
}

inline int busPrefetchRomStall()
{
    return 0;
}

inline int busPrefetchAbortStall([[maybe_unused]] uint32_t dataAddr,
    [[maybe_unused]] int elapsed)
{
    return 0;
}

// Emulates the Cheat System (m) code
inline void cpuMasterCodeCheck()
{
    if ((mastercode) && (mastercode == armNextPC)) {
        uint32_t joy = 0;
        if (systemReadJoypads())
            joy = systemReadJoypad(-1);
        uint32_t ext = (joy >> 10);
        cheatsCheckKeys(P1 ^ 0x3FF, ext);
    }
}

#endif  // VBAM_CORE_GBA_GBACPU_H_
