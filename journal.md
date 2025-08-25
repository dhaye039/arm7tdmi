# CpS-310: ArmSim Journal
### David John Hayes

### Project - Total Time Spent: 95 hours

---
### Roadmap

<ul></ul>
9.1.2023 - 9.2.2023: 
<ul>

<s>Phase 1: Loader</s>

</ul>
9.14.2023: 
<ul>

<s>Phase 2: Mockup</s>

</ul>
9.23.2023 - 9.25.2023: 
<ul>

<s>Phase 3a: Rough Design</s>

</ul>
10.2.2023 - 10.6.2023: 
<ul>

<s>Phase 3b: Refined Design</s>

</ul>
10.11.2023 - 11.1.2023: 
<ul>

<s>Phase 4: Prototype</s>

</ul>
11.13.2023 - 11.18.2023: 
<ul>

<s>Phase 5a: Simulator</s>

</ul>
12/04.2023 - 12.09.2023: (If needed)
<ul>

<s>Phase 5b: Simulator (Last Call)</s>

</ul>

<b>Easiest Parts:</b> Mockup & Simulator

<b>Hardest Part:</b> Prototype (Phase 4)

<b>Estimated Time:</b> 65-80 hours

---

### Loader Phase

<ul></ul> 
9.1.2023 
<ul> 

<b>Time Spent:</b> 2 hours

<b>Work Completed:</b> I read through the project’s different parts to get a feel for what I will be working on this semester. I also was looking through the Loader Phase assignment and trying to wrap my head around how to get started.

<b>Resources:</b> [ARMSim](https://protect.bju.edu/cps/courses/armsim-project/overview.html)
</ul> 
9.2.2023 
<ul>

<b>Time Spent:</b> 10 hours

<b>Work Completed:</b> In this phase of the project, I had the task of loading an ELF file, parsing through it to get the instructions from it, and storing the designated contents in simulated RAM. This took me far longer than it should have as I struggled to understand exactly what I was doing. But once I was able to wrap my head around everything, it became much easier (and more fun).

</ul>
Summary: 
<ul>

This phase was the "warm-up" phase for what was to come. I had to load an ELF file and store the contents in simulated RAM. Then compare the hash of RAM with the "answer key" to determine if they were equal (indicating success/failure).

<b>Work Completed:</b> All requirements met.

<b>Total Time Spent (Loader Phase):</b> 12 hours

</ul>

---
### Mockup Phase

<ul></ul>
9.14.2023
<ul>

<b>Time Spent:</b> 5 hours

<b>Work Completed:</b> Figured out how to use CMake and create a library for other files (like `ash.py`).

</ul>
Summary: 
<ul>

This phase was the introductory phase for the simulator. I created functions that will help update the CPU state and learned how to use CMake to create a library that I tested with `ash.py`.

<b>Work Completed:</b> All requirements met.

<b>Total Time Spent (Mockup Phase):</b> 5 hours

</ul>

---
### Rough Design (Phase 3a)

</ul>
9.21.2023
<ul>

<b>Time Spent:</b> 2 hours

<b>Work Completed:</b> Created the `DESIGN.md` file to give me an idea of how to tackle this project.

</ul>
9.23.2023
<ul>

<b>Time Spent:</b> 7 hours

<b>Work Completed:</b> Implemented the decode and execute logic for my simulator.

<b>Resources:</b> 

- [bitset](https://www.geeksforgeeks.org/cpp-bitset-and-its-application/#)
- [stoul](https://www.geeksforgeeks.org/stdstoul-stdstoull-c/)
- [to_ulong](https://stackoverflow.com/questions/19583720/convert-bitset-to-int-in-c)

</ul>
9.24.2023
<ul>

<b>Time Spent:</b> 2 hours

<b>Work Completed:</b> Updated and formatted my `journal.md` file to be up to date and look tidy.

</ul>
Summary: 
<ul>

In this phase, I began figuring out how I would decode instructions and execute them. So far, it seems to be working fine. I'm not too sure if my decoding logic will hold up in the long run, but I could not figure out a better way to do it (yet).

<b>Work Completed:</b> All requirements met.

<b>Total Time Spent (Rough Design):</b> 11 hours

</ul>

---
### Refined Design (Phase 3b)

</ul>
10.2.2023
<ul>

<b>Time Spent:</b> 3 hours

<b>Work Completed:</b> Revised some parts of my simulator based on the comments Dr. Jueckstock gave me: 
- Got rid of strings.
- Implemented  better logic for determining an instruction.
- Added a way to initialize load/store instructions with more parts to my `instr_parts` struct.

</ul>
10.6.2023
<ul>

<b>Time Spent:</b> 8 hours

<b>Work Completed:</b> I updated my code to be able to use `mov` to support a <i>register-shift-by-immediate</i> (excluding `ROR` and `RRX`). I also attempted to implement the `ldr` instruction, to which I implemented all the logic, but I was unable to figure out how to access RAM at the calculated address (or really at all).

</ul>

<i> After Due Date: </i>

</ul>
10.9.2023
<ul>

<b>Time Spent:</b> 2 hours

<b>Work Completed:</b> I fixed my code so that my `asr` logic will correctly shift the most signifigant bit, keeping the number negative or positive.

</ul>
Summary: 
<ul>

In this phase, I started to expand the capabilities of my simulator by adding another instruction (`ldr`). I am pleased with how everything turned out design-wise and feel comfortable continuing down my chosen design path.

<b>Work Completed:</b> All requirements met (assuming we didn’t need to have logic to execute `ldr` instructions)

<b>Total Time Spent (Refined Design):</b> 13 hours

</ul>

---
### Prototype (Phase 4)

</ul>
10.11.2023
<ul>

<b>Time Spent:</b> 1.5 hours

<b>Work Completed:</b> Fleshed out/organized some of my code to make it more readable.

</ul>
10.16.2023
<ul>

<b>Time Spent:</b> 5.5 hours

<b>Work Completed:</b> Implemented all the data processing instructions, added the `ROR` shift, and added logic to perform register shifts for the data processing instructions.

</ul>
10.29.2023
<ul>

<b>Time Spent:</b> 2 hours

<b>Work Completed:</b> Tried to figure out how to use the test programs and fixed some compiling errors.

</ul>
10.30.2023
<ul>

<b>Time Spent:</b> 4 hours

<b>Work Completed:</b> Attempted to understand how the memory loading and storing worked. Also began implementing the functions used to run the CPU.

</ul>
10.31.2023
<ul>

<b>Time Spent:</b> 9 hours

<b>Work Completed:</b> Figured out how memory loading and storing worked. Also implemented the trace logs and was able to pass 4 of the 7 tests. Finished implementing LDR/LDRB and STR/STRB for offset addressing. I still need to implement post-indexing and pre-indexing, as well as checking to see if the addresses are word-aligned.

</ul>
11.1.2023
<ul>

<b>Time Spent:</b> 6 hours

<b>Work Completed:</b> I finished implementing the other uses of ldr/str, and I implemented the `FD` variant of ldm/stm. My code now passed all 7 tests, though I'm unsure if my kernel correctly updates the `loads` and `stores` variables. I'm 98% sure it does, but I have no way of knowing what the numbers are supposed to be, so I'll just assume its fine.

</ul>
Summary: 
<ul>

For this phase, I implemented a very bare-bones kernel with data processing instructions and loading and storing capabilities. I struggled when trying to figure out how to access memory. A lot. But I'm very proud of myself for having figured it out (mostly) on my own. I did get a few pointers from some friends. And I'm looking forward to this next phase where I will have to integrate exceptions and interrupts.

<b>Work Completed:</b> All requirements met. 

<b>Total Time Spent (Prototype):</b> 28 hours

</ul>

---
### Simulator (Phase 5)

</ul>
12.4.2023
<ul>

<b>Time Spent:</b> 1 hour

<b>Work Completed:</b> Fleshed out/organized some of my code and looked through the extras to try and get a grasp on what I want to do, and what I'll need to do for it.

</ul>
12.7.2023
<ul>

<b>Time Spent:</b> 6 hours

<b>Work Completed:</b> Attempted to add `cmp`, `b`, and `bl` to my code. Also attempted to implement the `cpsr` flags. Ran into problems with all attempts.

</ul>
12.8.2023
<ul>

<b>Time Spent:</b> 8 hours

<b>Work Completed:</b> Finalized the implementation of the `cpsr` flags, `cmp`, `b`, `bl`, and `bx`. Then I started trying to pass the official test cases. I passed the `cmp` and `branch` test cases, but was running into problems with `locals` and `pointers`,

</ul>
12.9.2023
<ul>
<b>Time Spent:</b> 11 hours

<b>Work Completed:</b> Figured out what was happening with the `locals` and `pointers` test cases and got my simulator to pass them. I had implemented my branching logic incorrectly. And in `pointers`, it required me to add the `stmai` instruction. I then spent a very long time trying to finalize my simulator and pass the baseline tests (as well as some extra ones). After getting to the baseline, I attempted to implement the advanced features, but I had no luck, unfortunately. I then worked on finalizing my journal.

</ul>
Summary: 
<ul>

For this phase, I finalized my simulator. Adding several more instructions and flushing all the bugs out. I struggled a lot with the `cpsr` flags and then had to cram everything in for the last hour because I didn't realize the simulator was due at 10:00 pm instead of midnight.
 
<b>Work Completed:</b> C+ requirements met. 

<b>Total Time Spent (Prototype):</b> 26 hours

</ul>