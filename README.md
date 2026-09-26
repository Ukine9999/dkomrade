# dkomrade

Dead on current anticheats, no writeup for researchers, needs your own kernel primitive for av/etc. Consider it a landmark, have fun.

Under the hood it's a Windows process hider that unlinks `_EPROCESS` from every kernel list an enumerator can walk — snapshot, unlink, checked restore.


## What it does

Given a PID, walks `PspCidTable` to resolve the `_EPROCESS`, snapshots the
`LIST_ENTRY` and CID slots it is about to touch, and unlinks the process from
every kernel-visible list an enumerator can walk:

- `_EPROCESS.ActiveProcessLinks`
- `_EPROCESS.SessionProcessLinks`
- `_EPROCESS.MmProcessLinks`
- `_EPROCESS.JobLinks`
- `_HANDLE_TABLE.HandleTableList` (via `_EPROCESS.ObjectTable`)
- The process's own entry in `PspCidTable`

Restore is transactional and *checked*: before writing the saved links back it
re-reads the current neighbours and refuses to restore if anything else has
touched the list since the hide, so it will not corrupt a list that another
component concurrently edited.

## How it works

`PspCidTable` is a `_HANDLE_TABLE` with a 1–3 level radix layout. The walker resolves the entry for a handle by masking off the low tag bits, splitting the index into 10-bit strides per level, and reading `TableCode & ~3` for the base and `TableCode & 3` for the level count. Each `_HANDLE_TABLE_ENTRY.Object` holds a compressed `_EPROCESS` pointer: `(low >> 16) & ~0xF`.

Hide = snapshot `{flink, blink}` for each list node, write `blink->flink = flink` and `flink->blink = blink`, then point the node at itself. CID clear = zero the two qwords of the entry. Both are undone from the same snapshot.

## Requirements

- C++20
- A kernel memory RW primitive — DMA, a signed-driver bridge, a hypervisor, whatever you have. Not shipped with this library.
- A symbol / RVA source (PDB parser, offline dump, hardcoded table). Not shipped either (yeah, im lazy).

Both sit behind interfaces (`IKernelMemory`, `ISymbolProvider`); the library never touches Windows APIs or your process directly.

## Usage

```cpp
#include "Dkomrade/Application/KernelOffsetsLoader.h"
#include "Dkomrade/Application/ProcessHider.h"
#include "Dkomrade/Infrastructure/Logging/CallbackLogger.h"

MyDmaMemory kernelMemory;
MyPdbSymbols symbolProvider;
auto logger = Dkomrade::Infrastructure::Logging::CallbackLogger([](std::string_view line)
  { 
    std::puts(std::string(line).c_str()); 
  });

auto loader = Dkomrade::Application::KernelOffsetsLoader(symbolProvider);
auto offsets = loader.Load(ntKernelBase);
if (!offsets)
    return;

auto hider = Dkomrade::Application::ProcessHider(kernelMemory, logger, *offsets);
auto hidden = hider.Hide(targetPid);
if (!hidden)
    return;

  // better connect any DI to this...

// ... later ...
hider.Unhide(*hidden);
```

## Layout

```
include/Dkomrade/
  Domain/          - value types (offsets, snapshots, HiddenProcess)
  Application/     - use cases (walker, editors, hider, loader)
    Ports/         - outbound interfaces (memory, symbols, logger)
  Infrastructure/  - stock adapters (null / callback logger)
```

## Caveats

DKOM breaks assumptions PatchGuard and HVCI make about these kernel regions. On builds with KDP or read-only kernel data pages, writes may be rejected before this library ever sees an error - that is on your memory primitive to handle. And the unlink set is not exhaustive: ETW providers, `Csrss` handle tables, and object-name directories can still see the process. This library covers the standard enumeration surface, not everything.
