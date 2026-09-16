*This project has been created as part of the 42 curriculum by aben-sab.*

# Codexion

**Master the race for resources before the deadline masters you.**

## Description
Each coder's `death_deadline` is set to `last_compile_start + time_to_burnout` at the beginning of the simulation and after each compile starts. A coder burns out if it does not start compiling before that deadline.

Codexion is a concurrency simulation written in C that models the classic Dining Philosophers problem applied to a modern software development scenario. Multiple coders sit in a circular co-working hub, each needing two neighboring USB dongles to compile their "quantum code." The simulation explores resource contention, deadlock prevention, starvation avoidance, and burnout detection under configurable scheduling policies.

Each coder cycles through four phases — **taking dongles**, **compiling** (requiring two adjacent dongles), **debugging**, and **refactoring** — repeatedly until either a coder burns out or all coders complete the required number of compile cycles. Two scheduling policies are supported: **FIFO** (First In, First Out) and **EDF** (Earliest Deadline First), allowing comparison of fairness vs. urgency-based resource allocation.

## Instructions

### Compilation

```bash
make
```

This compiles all source files with `-Wall -Wextra -Werror -pthread` and produces the `codexion` binary.

### Usage

```bash
./codexion <num_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <num_compiles_required> <dongle_cooldown> <scheduler>
```

| Argument               | Description                                                                 |
|------------------------|-----------------------------------------------------------------------------|
| `num_coders`           | Number of coders (and dongles) in the simulation                            |
| `time_to_burnout`      | Time in ms a coder can wait before starting to compile before burning out   |
| `time_to_compile`      | Duration in ms of the compile phase                                         |
| `time_to_debug`        | Duration in ms of the debug phase                                           |
| `time_to_refactor`     | Duration in ms of the refactor phase                                        |
| `num_compiles_required`| Number of compiles each coder must complete                              |
| `dongle_cooldown`      | Minimum time in ms before a released dongle can be re-acquired              |
| `scheduler`            | Scheduling policy: `fifo` or `edf`                                          |

### Example

```bash
./codexion 5 1000 200 200 200 3 50 edf
```

This runs a simulation with 5 coders, a 1000ms burnout window, 200ms for each phase, requiring 3 compiles per coder, a 50ms dongle cooldown, and Earliest Deadline First scheduling.

### Cleaning

```bash
make clean    # Remove object files
make fclean   # Remove object files and binary
make re       # Full rebuild
```

## Blocking Cases Handled

### Deadlock Prevention (Coffman's Conditions)

The project prevents deadlock by breaking two of Coffman's four necessary conditions:

- **Hold-and-wait**: Coders do not acquire dongles one at a time. Both the left and right dongles are acquired atomically in `acquire_dongles()` under the simulation mutex. A coder either gets both or neither.
- **Circular wait**: The scheduler (`has_highest_priority()`) ensures that when multiple coders compete for the same dongles, only the highest-priority coder proceeds. This breaks the circular dependency chain.

### Starvation Prevention

- **EDF scheduling**: The coder with the earliest `death_deadline` (closest to burnout) receives priority. This ensures that coders about to burn out get first access to resources.
- **FIFO scheduling**: The coder who requested dongles first (earliest `last_request_time`) receives priority, providing first-come-first-served fairness.
- When a lower-priority coder detects a higher-priority competitor, it yields and re-enters the wait state, allowing the urgent coder to proceed.

### Burnout Detection

- Each coder's `death_deadline` is set when it enters the scheduler queue, and applies while it waits to acquire its dongles.
- A dedicated **monitor thread** polls every 1ms and checks if any coder has exceeded their deadline.
- On burnout, the monitor sets `simulation_stop = 1`, prints the burnout log within the required 10ms window, and broadcasts to unblock all waiting threads.

### Cooldown Enforcement

- After a dongle is released, its `last_used_time` is recorded.
- `can_take_dongles()` verifies that `current_time - last_used_time >= dongle_cooldown` for both dongles before allowing acquisition, preventing immediate re-use.

### Priority Queue Scheduling

- Waiting coders are stored in a binary heap protected by `sim_mutex`.
- The heap orders requests by arrival time for FIFO or by burnout deadline for EDF, with coder ID as a deterministic tie-breaker.

### Log Serialization

- All log output passes through `print_status()` which acquires a dedicated `log_mutex` before printing and releases it after.
- The burnout message in the monitor also acquires `log_mutex` directly.
- This prevents two log lines from interleaving on the same output line.

### Interruptible Sleep

- `smart_sleep()` does not use a single blocking `usleep()`. Instead, it loops in 500-microsecond increments, checking `simulation_stop` each iteration.
- This allows coders to exit immediately when the simulation ends, rather than sleeping for the full duration.

### Simulation Stop Propagation

- When the monitor sets `simulation_stop = 1`, it calls `pthread_cond_broadcast()` to wake all blocked coders.
- Each coder checks `simulation_stop` at the top of its main loop and after waking from `pthread_cond_wait`.
- `smart_sleep()` also checks `simulation_stop` to abort sleeping early.

## Thread Synchronization Mechanisms

### POSIX Threads (`pthread`)

- **`pthread_create`**: Spawns N coder threads (each running `coder_routine`) and 1 monitor thread (running `monitor_routine`).
- **`pthread_join`**: Waits for all threads to finish before cleanup.

### Mutexes (`pthread_mutex_t`)

| Mutex              | Scope       | Purpose                                                                              |
|--------------------|-------------|--------------------------------------------------------------------------------------|
| `data->sim_mutex`  | Global      | Protects shared simulation state, scheduler state, and the atomic acquisition/release decision. |
| `data->log_mutex`  | Global      | Serializes `printf` calls to prevent interleaved log output.                          |
| `dongles[i].mutex` | Per-dongle  | Protects each dongle's `is_taken` and `last_used_time` fields during access.           |

### Condition Variables (`pthread_cond_t`)

| Variable            | Purpose                                                                                     |
|---------------------|---------------------------------------------------------------------------------------------|
| `data->cond_sched`  | Scheduler condition variable. Coders block on this when they cannot take dongles. Woken by `pthread_cond_broadcast` when dongles are released or simulation ends. |

### How Race Conditions Are Prevented

1. **Dongle state protection**: Dongle state is accessed under both the simulation mutex and the dongle mutex, preventing two coders from simultaneously marking a dongle as taken.

2. **Atomic dual-acquisition**: `acquire_dongles()` checks `can_take_dongles()` and sets both `is_taken = 1` within a single `sim_mutex` critical section. No other thread can observe a partial acquisition.

3. **Priority arbitration**: After `pthread_cond_broadcast` wakes all waiting coders, each calls `has_highest_priority()` under `sim_mutex`. Only the highest-priority coder proceeds; others re-wait on the condition variable.

4. **Monitor-coder communication**: The monitor thread reads coder state (`compiles_count`, `death_deadline`) under `sim_mutex` and writes `simulation_stop` under the same lock. Coders check `simulation_stop` under `sim_mutex` before and after waiting, ensuring they observe the stop signal promptly.

5. **Safe sleep**: `smart_sleep()` acquires `sim_mutex` in each polling iteration to check `simulation_stop`, preventing the race condition where a coder sleeps through a simulation-stop signal.

## Resources

- [POSIX Threads Programming - LLNL](https://hpc-tutorials.llnl.gov/posix/)
- [The Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Coffman Conditions - Wikipedia](https://en.wikipedia.org/wiki/Coffman_conditions)
- [pthread man pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [Dijkstra's Dining Philosophers Solution](https://www.cs.utexas.edu/users/EWD/ewd08xx/PDF887.PDF)

### AI Usage

AI (ChatGPT) was used for the following tasks:

- **Code review and debugging**: Analyzing race conditions and identifying potential deadlocks in the synchronization logic.
- **Architecture discussions**: Reviewing the design choices for the circular topology, scheduler implementation, and monitor thread pattern.
- **Documentation**: Drafting and refining this README to ensure all required sections are covered comprehensively.

AI was **not** used to generate the source code directly. All implementation was written by me with AI serving as a reference and review tool.
# Codexion
