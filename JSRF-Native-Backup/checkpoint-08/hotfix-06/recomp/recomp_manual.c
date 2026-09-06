/**
 * JSRF manual overrides and indirect-call diagnostics.
 *
 * Lifted retail code remains authoritative except where runtime evidence proves
 * the lifter cannot preserve an original primitive. JSRF's CRT memmove uses an
 * unusual negative-index jump table in its reverse-copy path; replacing the
 * whole proven CRT primitive is exact and avoids treating a switch case as a
 * function entry.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#define RECOMP_TLS __declspec(thread)
#else
#define RECOMP_TLS _Thread_local
#endif

extern RECOMP_TLS uint32_t g_eax;
extern RECOMP_TLS uint32_t g_esp;
extern ptrdiff_t g_xbox_mem_offset;

extern volatile uint32_t g_icall_trace[16];
extern volatile uint32_t g_icall_trace_idx;
extern volatile uint64_t g_icall_count;

typedef void (*recomp_func_t)(void);

#define GUEST_U32(va) (*(uint32_t *)((uintptr_t)g_xbox_mem_offset + (uint32_t)(va)))
#define GUEST_PTR(va) ((void *)((uintptr_t)g_xbox_mem_offset + (uint32_t)(va)))

/*
 * Retail 0x0017CEC0: MSVC CRT memmove(dst, src, count), cdecl.
 *
 * The retail body has a reverse-copy path whose indexed jump at 0x0017D082
 * addresses an embedded table backwards; 0x0017D15C is one case label inside
 * this function, not a callable function. Host memmove is the same operation.
 * Generated callers retain ownership of the three cdecl arguments, so this
 * replacement pops only xboxrecomp's synthetic return address.
 */
void sub_0017CEC0(void)
{
    uint32_t dst = GUEST_U32(g_esp + 4);
    uint32_t src = GUEST_U32(g_esp + 8);
    uint32_t count = GUEST_U32(g_esp + 12);

    if (count != 0) {
        memmove(GUEST_PTR(dst), GUEST_PTR(src), (size_t)count);
    }
    g_eax = dst;
    g_esp += 4;
}

recomp_func_t recomp_lookup_manual(uint32_t xbox_va)
{
    if (xbox_va == 0x0017CEC0u) {
        return sub_0017CEC0;
    }
    return (recomp_func_t)0;
}

void recomp_icall_fail_log(uint32_t va)
{
    int i;
    fprintf(stderr, "[ICALL] unresolved target 0x%08X at call #%llu\n",
            va, (unsigned long long)g_icall_count);
    fprintf(stderr, "  recent targets:\n");
    for (i = 0; i < 16; ++i) {
        int index = (g_icall_trace_idx - 16 + i) & 15;
        if (g_icall_trace[index]) {
            fprintf(stderr, "    [%2d] 0x%08X\n", i, g_icall_trace[index]);
        }
    }
    fflush(stderr);
}

void recomp_icall_not_code_log(uint32_t va)
{
    enum { SLOT_COUNT = 16 };
    static uint32_t seen[SLOT_COUNT];
    static uint64_t hits[SLOT_COUNT];
    static int count;
    int i;

    for (i = 0; i < count; ++i) {
        if (seen[i] == va) {
            break;
        }
    }
    if (i == count) {
        if (count == SLOT_COUNT) {
            return;
        }
        seen[count] = va;
        hits[count] = 0;
        ++count;
    }

    ++hits[i];
    {
        uint64_t n = hits[i];
        while (n >= 10 && n % 10 == 0) {
            n /= 10;
        }
        if (n != 1) {
            return;
        }
    }

    fprintf(stderr,
            "[ICALL] target 0x%08X is not code; skipped %llu time(s) at call #%llu\n",
            va, (unsigned long long)hits[i], (unsigned long long)g_icall_count);
    fflush(stderr);
}
