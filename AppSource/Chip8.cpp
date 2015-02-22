//
//  Chip8.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 22/02/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "Chip8.h"

#include <ChilliSource/Core/Base/Application.h>
#include <ChilliSource/Core/File.h>

//    0NNN 	Calls RCA 1802 program at address NNN.
//    00EE 	Returns from a subroutine.
//    1NNN 	Jumps to address NNN.
//    3XNN 	Skips the next instruction if VX equals NN.
//    4XNN 	Skips the next instruction if VX doesn't equal NN.
//    5XY0 	Skips the next instruction if VX equals VY.
//    6XNN 	Sets VX to NN.
//    7XNN 	Adds NN to VX.
//    8XY5 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
//    8XY6 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
//    8XY7 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
//    8XYE 	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
//    9XY0 	Skips the next instruction if VX doesn't equal VY.
//    ANNN 	Sets I to the address NNN.
//    BNNN 	Jumps to the address NNN plus V0.
//    CXNN 	Sets VX to a random number, masked by NN.
//    DXYN 	Sprites stored in memory at location in index register (I), maximum 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing (i.e. it toggles the screen pixels)
//    EX9E 	Skips the next instruction if the key stored in VX is pressed.
//    EXA1 	Skips the next instruction if the key stored in VX isn't pressed.
//    FX07 	Sets VX to the value of the delay timer.
//    FX0A 	A key press is awaited, and then stored in VX.
//    FX15 	Sets the delay timer to VX.
//    FX18 	Sets the sound timer to VX.
//    FX1E 	Adds VX to I.[3]
//    FX29 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
//    FX55 	Stores V0 to VX in memory starting at address I.[4]
//    FX65 	Fills V0 to VX with values from memory starting at address I.[4]

namespace
{
    using OpCode = u16;
    
    u32 k_initMemOffset = 0x200;
}

namespace OpCodeActions
{
    //0x00E0
    void ClearScreen(OpCode in_opCode, Chip8::State& inout_state)
    {
        CS_LOG_VERBOSE("Clear");
        std::fill(std::begin(inout_state.m_graphicsState), std::end(inout_state.m_graphicsState), 0);
    }
    
    //0x2NNN
    void CallSubroutine(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_stack[inout_state.m_stackPointer] = inout_state.m_programCounter;
        ++inout_state.m_stackPointer;
        inout_state.m_programCounter = in_opCode & 0x0FFF;
    }
    
    //0x8XY0
    void SetVXtoVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
    }
    
    //0x8XY1
    void SetVXtoVXorVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] | inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
    }
    
    //0x8XY2
    void SetVXtoVXandVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] & inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
    }
    
    //0x8XY3
    void SetVXtoVXxorVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] ^ inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
    }
    
    //0x8XY4
    void AddVYtoVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        if(inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] > (0xFF - inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]))
        {
            inout_state.m_vRegs[0xF] = 1; //carry
        }
        else
        {
            inout_state.m_vRegs[0xF] = 0;
        }
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] += inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        
        inout_state.m_programCounter += 2;
    }
    
    //0xFX33
    void StoreBinaryCodedVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_memory[inout_state.m_iReg]     = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] / 100);
        inout_state.m_memory[inout_state.m_iReg + 1] = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] / 10) % 10;
        inout_state.m_memory[inout_state.m_iReg + 2] = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] % 100) % 10;
        
        inout_state.m_programCounter += 2;
    }
}

namespace Utils
{
    OpCode FetchNextOpcode(const u8* in_memory, u16 in_programCounter)
    {
        return in_memory[in_programCounter] << 8 | in_memory[in_programCounter + 1];
    }
    
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
    
    std::function<void(Chip8::State& inout_state)> Decode(OpCode in_opCode)
    {
        //TODO: Map actions
        return std::bind(&OpCodeActions::ClearScreen, in_opCode, std::placeholders::_1);
    }
}

Chip8::Chip8()
{
    Reset();
}


void Chip8::Reset()
{
    std::fill(std::begin(m_state.m_memory), std::end(m_state.m_memory), 0);
    std::fill(std::begin(m_state.m_vRegs), std::end(m_state.m_vRegs), 0);
    std::fill(std::begin(m_state.m_stack), std::end(m_state.m_stack), 0);
    std::fill(std::begin(m_state.m_graphicsState), std::end(m_state.m_graphicsState), 0);
    std::fill(std::begin(m_state.m_keyState), std::end(m_state.m_keyState), 0);
    
    m_state.m_iReg = 0;
    m_state.m_stackPointer = 0;
    m_state.m_delayTimer = 0;
    m_state.m_soundTimer = 0;
    //The program counter is offset as traditionally the interpreter would occupy the first 512 bytes.
    m_state.m_programCounter = k_initMemOffset;
}

void Chip8::LoadROM(const std::string& in_romPath)
{
    auto fileSystem = CSCore::Application::Get()->GetFileSystem();
    auto fileStream = fileSystem->CreateFileStream(CSCore::StorageLocation::k_package, in_romPath, CSCore::FileMode::k_readBinary);
    
    std::string rom;
    fileStream->GetAll(rom);
    
    for(u32 i=0; i<rom.length(); ++i)
    {
        m_state.m_memory[i+k_initMemOffset] = rom[i];
    }
}

void Chip8::FetchDecodeExecute()
{
    auto nextOpCode = Utils::FetchNextOpcode(m_state.m_memory, m_state.m_programCounter);
    auto action = Utils::Decode(nextOpCode);
    
    //Execute decoded action which will change the chip state.
    action(m_state);
    
    m_state.m_delayTimer = Utils::UpdateTimer(m_state.m_delayTimer, []()
    {
    });
    
    m_state.m_soundTimer = Utils::UpdateTimer(m_state.m_soundTimer, []()
    {
        CS_LOG_VERBOSE("Beep!");
    });
}
    

