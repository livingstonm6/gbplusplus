#pragma once
#include "Registers.h"
#include "utility/Common.h"
#include "Bus.h"
#include "CPUDataFetcher.h"
#include "InstructionExecutor.h"
#include "CPUContext.h"
#include "Timer.h"
#include "PPU.h"
#include "instructions.h"
#include <iostream>

/**
* CPU
* 
* Emulates the Game Boy's CPU by executing instructions
* and handling interrupts. 
* 
* The Timer and PPU clocks are synced to the CPU, both are
* ticked four times per CPU cycle.
*/
class CPU
{

public:

   
    CPUContext ctx; // Contains CPU state (registers, current instruction, etc.)

    /**
    * Called by Motherboard to pass pointers to the other
    * components. Should be called before CPU::step.
    * 
    * CPU does not need to worry about managing memory,
    * Motherboard has ownership.
    */

    void connect(Bus* bus_param, Timer* timer_param, PPU* ppu_param);

    /**
    * Executes one CPU instruction, if not halted. Then, checks
    * for interrupts if interrupt master enable flag is set.
    */
    void step();

    CPU() {}
    
private:
	Bus* bus = nullptr;                 // Read/write data
    Timer* timer = nullptr;             // Ticked every 4 CPU cycles
    PPU* ppu = nullptr;                 // Also ticked every 4 CPU cycles
	
    CPUDataFetcher fetcher;             // Fetches instructions and data
    InstructionExecutor executor;       // Executes instructions and manages stack

    /**
    * Called when a CPU cycle passes (bus read/write, stack push/pop, etc).
    * Ticks the PPU and timer four times for each CPU cycle.
    *
    * @param num_cycles: the number of cpu cycles
    */
    void increment_cycles(int num_cycles);

    /**
    * Ticks the ppu once, then raises any interrupts that were triggered.
    */
    void tick_ppu();

    /**
    * Raise an interrupt to be handled.
    * 
    * @param interrupt: the type of interrupt to be raised
    */

    void request_interrupt(InterruptType interrupt);

    /**
    * Handle one interrupt, the first of VBlank, LCD Status,
    * Timer, serial, or Joypad (in that order) that has been raised.
    */
    void handle_interrupts();


    /**
    * Check if an interrupt has been raised, and if so, handle it.
    * 
    * @param address: the address of the interrupt handler
    * @param type: the type of interrupt to check for
    * 
    * @return: true if the given interrupt type was raised, false otherwise
    */
    bool check_interrupt(u16 address, InterruptType type);


    /**
    * Pushes the program counter to the stack, then sets the program
    * counter to the address of an interrupt handler.
    * 
    * @param address: the address of the interrupt handler
    */
    void jump_to_interrupt_handler(u16 address);

};

