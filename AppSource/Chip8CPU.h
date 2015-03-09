//
//  Chip8CPU.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 07/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _CHIP8CPU_H_
#define _CHIP8CPU_H_

#include "Chip8MutableState.h"
#include "OpCodeActions.h"

//----------------------------------------------------------------
/// Simulates the fetch, decode and execute cycle of the chip-8
/// cpu. This will alter memory, registers and timers.
///
/// Timers are updated at 60hz. CPU runs at 600 ops per second.
///
/// @author S Downie.
//----------------------------------------------------------------
class Chip8CPU
{
public:
    
    //----------------------------------------------------------------
    /// Constructor that sets up all the opcodes
    ///
    /// @author S Downie
    //----------------------------------------------------------------
    Chip8CPU();
    //----------------------------------------------------------------
    /// Performs a single CPU cycle, fetches and decodes the
    /// next instruction then executes it affecting the given state
    ///
    /// @author S Downie
    ///
    /// @param inout_state - The current state of the VM to modify
    //----------------------------------------------------------------
    void FetchDecodeExecute(Chip8MutableState& inout_state);
    
private:
    
    //----------------------------------------------------------------
    /// Convert the opcode to an executable function
    ///
    /// @author S Downie
    ///
    /// @param in_opCode - OpCode to convert
    ///
    /// @return Executable function for OpCode that modifies the state
    //----------------------------------------------------------------
    std::function<void(Chip8MutableState& inout_state)> Decode(OpCode in_opCode);
    //----------------------------------------------------------------
    /// Forwarding method that routes to the correct OpCode starting with 0
    ///
    /// @author S Downie
    ///
    /// @param in_opCode - OpCode to act on
    /// @param inout_state - State to mutate
    //----------------------------------------------------------------
    void x0NNN(OpCode in_opCode, Chip8MutableState& inout_state);
    //----------------------------------------------------------------
    /// Forwarding method that routes to the correct OpCode starting with 8
    ///
    /// @author S Downie
    ///
    /// @param in_opCode - OpCode to act on
    /// @param inout_state - State to mutate
    //----------------------------------------------------------------
    void x8NNN(OpCode in_opCode, Chip8MutableState& inout_state);
    //----------------------------------------------------------------
    /// Forwarding method that routes to the correct OpCode starting with E
    ///
    /// @author S Downie
    ///
    /// @param in_opCode - OpCode to act on
    /// @param inout_state - State to mutate
    //----------------------------------------------------------------
    void xENNN(OpCode in_opCode, Chip8MutableState& inout_state);
    //----------------------------------------------------------------
    /// Forwarding method that routes to the correct OpCode starting with F
    ///
    /// @author S Downie
    ///
    /// @param in_opCode - OpCode to act on
    /// @param inout_state - State to mutate
    //----------------------------------------------------------------
    void xFNNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
private:
    
    std::function<void(OpCode, Chip8MutableState&)> m_opcodeActions[0xF066];
};

#endif
