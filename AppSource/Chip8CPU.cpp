//
//  Chip8CPU.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 07/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "Chip8CPU.h"

#include <ChilliSource/Core/Delegate.h>

namespace
{
    //----------------------------------------------------------------
    /// Fetch the opcode instruction from memory at the given
    /// program counter
    ///
    /// @author S Downie
    ///
    /// @param in_memory - Memory to read opcode from
    /// @param in_programCounter - Pointer to opcode in memory
    //----------------------------------------------------------------
    OpCode FetchNextOpcode(const u8* in_memory, u16 in_programCounter)
    {
        return in_memory[in_programCounter] << 8 | in_memory[in_programCounter + 1];
    }
    //----------------------------------------------------------------
    /// Tick the given timer and call the given function when the timer
    /// reaches zero.
    ///
    /// @author S Downie
    ///
    /// @param in_timer - Timer to decrement
    /// @param in_onTimerEnded - Function to call when timer reaches zero.
    //----------------------------------------------------------------
    u8 UpdateTimer(u8 in_timer, const std::function<void()>& in_onTimerEnded)
    {
        if (in_timer > 0)
        {
            --in_timer;
            
            if (in_timer == 0)
            {
                in_onTimerEnded();
            }
        }
        
        return in_timer;
    }
}

//----------------------------------------------------------------
//----------------------------------------------------------------
Chip8CPU::Chip8CPU()
{
    //Fill with NoOps which are called for missing or unknown instructions
    std::fill(std::begin(m_opcodeActions), std::end(m_opcodeActions), CSCore::MakeDelegate(&OpCodeActions::NoOp));
    
    m_opcodeActions[0x0000] = CSCore::MakeDelegate(this, &Chip8CPU::x0NNN);
    m_opcodeActions[0x00e0] = CSCore::MakeDelegate(&OpCodeActions::x00E0);
    m_opcodeActions[0x00ee] = CSCore::MakeDelegate(&OpCodeActions::x00EE);
    m_opcodeActions[0x1000] = CSCore::MakeDelegate(&OpCodeActions::x1NNN);
    m_opcodeActions[0x2000] = CSCore::MakeDelegate(&OpCodeActions::x2NNN);
    m_opcodeActions[0x3000] = CSCore::MakeDelegate(&OpCodeActions::x3XNN);
    m_opcodeActions[0x4000] = CSCore::MakeDelegate(&OpCodeActions::x4XNN);
    m_opcodeActions[0x5000] = CSCore::MakeDelegate(&OpCodeActions::x5XY0);
    m_opcodeActions[0x6000] = CSCore::MakeDelegate(&OpCodeActions::x6XNN);
    m_opcodeActions[0x7000] = CSCore::MakeDelegate(&OpCodeActions::x7XNN);
    m_opcodeActions[0x8000] = CSCore::MakeDelegate(this, &Chip8CPU::x8NNN);
    m_opcodeActions[0x8FF0] = CSCore::MakeDelegate(&OpCodeActions::x8XY0);
    m_opcodeActions[0x8FF1] = CSCore::MakeDelegate(&OpCodeActions::x8XY1);
    m_opcodeActions[0x8FF2] = CSCore::MakeDelegate(&OpCodeActions::x8XY2);
    m_opcodeActions[0x8FF3] = CSCore::MakeDelegate(&OpCodeActions::x8XY3);
    m_opcodeActions[0x8FF4] = CSCore::MakeDelegate(&OpCodeActions::x8XY4);
    m_opcodeActions[0x8FF5] = CSCore::MakeDelegate(&OpCodeActions::x8XY5);
    m_opcodeActions[0x8FF6] = CSCore::MakeDelegate(&OpCodeActions::x8XY6);
    m_opcodeActions[0x8FF7] = CSCore::MakeDelegate(&OpCodeActions::x8XY7);
    m_opcodeActions[0x8FFE] = CSCore::MakeDelegate(&OpCodeActions::x8XYE);
    m_opcodeActions[0x9000] = CSCore::MakeDelegate(&OpCodeActions::x9XY0);
    m_opcodeActions[0xA000] = CSCore::MakeDelegate(&OpCodeActions::xANNN);
    m_opcodeActions[0xB000] = CSCore::MakeDelegate(&OpCodeActions::xBNNN);
    m_opcodeActions[0xC000] = CSCore::MakeDelegate(&OpCodeActions::xCXNN);
    m_opcodeActions[0xD000] = CSCore::MakeDelegate(&OpCodeActions::xDXYN);
    m_opcodeActions[0xE000] = CSCore::MakeDelegate(this, &Chip8CPU::xENNN);
    m_opcodeActions[0xE00E] = CSCore::MakeDelegate(&OpCodeActions::xEX9E);
    m_opcodeActions[0xE001] = CSCore::MakeDelegate(&OpCodeActions::xEXA1);
    m_opcodeActions[0xF000] = CSCore::MakeDelegate(this, &Chip8CPU::xFNNN);
    m_opcodeActions[0xF007] = CSCore::MakeDelegate(&OpCodeActions::xFX07);
    m_opcodeActions[0xF00A] = CSCore::MakeDelegate(&OpCodeActions::xFX0A);
    m_opcodeActions[0xF015] = CSCore::MakeDelegate(&OpCodeActions::xFX15);
    m_opcodeActions[0xF018] = CSCore::MakeDelegate(&OpCodeActions::xFX18);
    m_opcodeActions[0xF01E] = CSCore::MakeDelegate(&OpCodeActions::xFX1E);
    m_opcodeActions[0xF029] = CSCore::MakeDelegate(&OpCodeActions::xFX29);
    m_opcodeActions[0xF033] = CSCore::MakeDelegate(&OpCodeActions::xFX33);
    m_opcodeActions[0xF055] = CSCore::MakeDelegate(&OpCodeActions::xFX55);
    m_opcodeActions[0xF065] = CSCore::MakeDelegate(&OpCodeActions::xFX65);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8CPU::FetchDecodeExecute(Chip8MutableState& inout_state)
{
    auto nextOpCode = FetchNextOpcode(inout_state.m_memory, inout_state.m_programCounter);
    auto action = Decode(nextOpCode);
    
    //Execute decoded action which will change the chip state.
    action(inout_state);
    
    inout_state.m_delayTimer = UpdateTimer(inout_state.m_delayTimer, [](){});
    
    inout_state.m_soundTimer = UpdateTimer(inout_state.m_soundTimer, []()
    {
        CS_LOG_VERBOSE("Beep!");
    });
}
//----------------------------------------------------------------
//----------------------------------------------------------------
std::function<void(Chip8MutableState& inout_state)> Chip8CPU::Decode(OpCode in_opCode)
{
    return std::bind(m_opcodeActions[in_opCode & 0xf000], in_opCode, std::placeholders::_1);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8CPU::x0NNN(OpCode in_opCode, Chip8MutableState& inout_state)
{
    m_opcodeActions[in_opCode & 0xf0ff](in_opCode, inout_state);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8CPU::x8NNN(OpCode in_opCode, Chip8MutableState& inout_state)
{
    m_opcodeActions[(in_opCode & 0xf00f) + 0xff0](in_opCode, inout_state);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8CPU::xENNN(OpCode in_opCode, Chip8MutableState& inout_state)
{
    m_opcodeActions[in_opCode & 0xf00f](in_opCode, inout_state);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8CPU::xFNNN(OpCode in_opCode, Chip8MutableState& inout_state)
{
    m_opcodeActions[in_opCode & 0xf0ff](in_opCode, inout_state);
}