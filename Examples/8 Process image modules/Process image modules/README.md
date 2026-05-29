# Process Image Modules — Startup Synchronization Demo

## What This Example Shows

Each PLC instance in TwinCAT manages its own process image update independently. When multiple PLC instances share the same task with different sort orders, inputs received from a "later" PLC in the sort order will always be one cycle behind — because that PLC hasn't run yet when the earlier one reads its inputs.

This example makes that timing observable. Each worker PLC increments a counter (`ToPLC_X AT %Q*:UDINT`) every cycle. Once all counters are synchronized to the same starting value (via the Coordinator at startup), a scope trace clearly reveals which PLCs are reading the **current cycle** value vs the **previous cycle** value from each other — purely as a consequence of sort order and individual process image updates.

This example demonstrates the timing behavior and the startup sync mechanism using **TcCOM Data Area access**.

---

## The Timing Behavior

Three worker PLCs (PLC1, PLC2, PLC3) each run this every cycle:

```st
ToPLC_X := (ToPLC_X + 1) MOD 200;
```

Because each PLC does its own process image update and they execute in sort order (10 → 20 → 30), the value a PLC reads from another depends on whether that other PLC has already run this cycle:

| PLC | Reads from PLC1 | Reads from PLC2 | Reads from PLC3 |
|-----|-----------------|-----------------|-----------------|
| **PLC1** (sort 10) | — | previous cycle | previous cycle |
| **PLC2** (sort 20) | **current cycle** | — | previous cycle |
| **PLC3** (sort 30) | **current cycle** | **current cycle** | — |

PLC1 runs first — so when it reads PLC2 and PLC3's outputs, neither has updated yet. PLC2 runs second — it gets PLC1's fresh value but PLC3's old value. PLC3 runs last and sees the current-cycle value from both.

**Why the counters must be synced at startup:** If the counters start at different values (which they will without intervention, since PLCs don't all begin executing on the same cycle), the offset between them obscures the timing pattern. The Coordinator resets all counters to 0 so the 1-cycle lag is immediately visible in a scope trace.

---

## The Fix: Coordinator + TcCOM Data Area Access

A fourth PLC instance — the **Coordinator** (sort order 200, runs last in the task) — resets all worker counters to 0 at startup using `ITcADI` (TwinCAT Area Data Interface).

**How it works:**

1. Wait ~5 seconds for all PLCs to load
2. Discover all PLC instance ObjectIDs via `TComGetModulesByClass`
3. For each worker PLC, call `F_WriteDataArea` to write `0` directly into its output process image at the offset mapped to `ToPLC_X`
4. Enter idle state — expose a `ResetCounters` flag for manual re-sync

This uses **raw memory writes into the target PLC's process image** via its TcCOM ObjectID — no ADS round-trip, no variable links. The write happens within the same task context, making it synchronous and deterministic.

---

## Project Structure

| Instance | AMS Port | Sort Order | Role |
|----------|----------|------------|------|
| PLC1 Instance | 851 | 10 | Worker — increments counter |
| PLC2 Instance | 852 | 20 | Worker — increments counter |
| PLC3 Instance | 853 | 30 | Worker — increments counter |
| Cordinator Instance | 854 | 200 | Resets all counters via TcCOM |

**Cross-PLC variable mapping (tsproj):**
- Each PLC's `ToPLC_X` output is linked to the other PLCs' `FromPLC_X` inputs
- This handles data exchange — but the timing of when each PLC's image updates determines whether the received value is current-cycle or previous-cycle

---

## Key Files

| File | Purpose |
|------|---------|
| `Cordinator/POUs/MAIN.TcPOU` | Coordinator state machine — discovery, reset, idle |
| `Cordinator/POUs/F_WriteDataArea.TcPOU` | Writes to a target PLC's data area via ITcADI |
| `Cordinator/POUs/TComGetModulesByClass.TcPOU` | Enumerates PLC instances by TcCOM class ID |
| `PLC1/POUs/PRG_PLC1.TcPOU` | Worker logic — increments ToPLC_X each cycle |
| `PLC2/POUs/PRG_PLC2.TcPOU` | Worker logic — increments ToPLC_X each cycle |
| `PLC3/POUs/PRG_PLC3.TcPOU` | Worker logic — increments ToPLC_X each cycle |

---

## Key Takeaway

> Each PLC instance updates its own process image independently. PLCs that execute earlier in the sort order will read **previous-cycle** values from PLCs that haven't run yet — this is expected TwinCAT behavior, not a bug.

The counter sync at startup isn't just housekeeping — it's what makes the 1-cycle lag **visible**. Without it, the counters start at arbitrary offsets and the timing pattern is buried in noise. The Coordinator uses TcCOM `ITcADI` direct memory writes (rather than ADS or variable links) to perform this reset synchronously within the same task context.
