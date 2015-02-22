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
#include <ChilliSource/Core/Math.h>

//    0NNN 	Calls RCA 1802 program at address NNN.
//TODO

namespace
{
    using OpCode = u16;
    
    u32 k_initMemOffset = 0x200;
    
    u8 k_fontSet[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
}

namespace OpCodeActions
{
    //0x00E0
    void ClearScreen(OpCode in_opCode, Chip8::State& inout_state)
    {
        std::fill(std::begin(inout_state.m_graphicsState), std::end(inout_state.m_graphicsState), 0);
        inout_state.m_programCounter += 2;
        
        //TODO: Set dirty flag
    }
    
    //0x1NNN
    void Jump(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter = in_opCode & 0x0FFF;
    }
    
    //0x2NNN
    void CallSubroutine(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_stack[inout_state.m_stackPointer] = inout_state.m_programCounter;
        ++inout_state.m_stackPointer;
        inout_state.m_programCounter = in_opCode & 0x0FFF;
    }
    
    //0x00EE
    void ReturnSubroutine(OpCode in_opCode, Chip8::State& inout_state)
    {
        --inout_state.m_stackPointer;
        inout_state.m_programCounter = inout_state.m_stack[inout_state.m_stackPointer];
        inout_state.m_programCounter += 2;
    }
    
    //0x3XNN
    void SkipInstructionIfVXeqNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] == inout_state.m_vRegs[in_opCode & 0x00FF] ? 4 : 2;
    }
    
    //0x4XNN
    void SkipInstructionIfVXnoteqNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] != inout_state.m_vRegs[in_opCode & 0x00FF] ? 4 : 2;
    }
    
    //0x5XY0
    void SkipInstructionIfVXeqVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] == inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] ? 4 : 2;
    }
    
    //0x6XNN
    void SetVXtoNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[in_opCode & 0x00FF];
        inout_state.m_programCounter += 2;
    }
    
    //0x7XNN
    void AddNNtoVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] += inout_state.m_vRegs[in_opCode & 0x00FF];
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY0
    void SetVXtoVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY1
    void SetVXtoVXorVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] | inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY2
    void SetVXtoVXandVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] & inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY3
    void SetVXtoVXxorVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] ^ inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY4
    void AddVYtoVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        if(inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] > (0xFF - inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]))
        {
            inout_state.m_vRegs[0xF] = 1; //Carry
        }
        else
        {
            inout_state.m_vRegs[0xF] = 0;
        }
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] += inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY5
    void SubtractVYfromVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        if(inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] > (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]))
        {
            inout_state.m_vRegs[0xF] = 0; //Borrow
        }
        else
        {
            inout_state.m_vRegs[0xF] = 1;
        }
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] -= inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY6
    void ShiftVXRight(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[0xF] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] & 0x1;
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] >>= 1;
        inout_state.m_programCounter += 2;
    }
    
    //0x8XY7
    void SetVXtoVYminusVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        if(inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] > inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4])	// VY-VX
        {
            inout_state.m_vRegs[0xF] = 0; //Borrow
        }
        else
        {
            inout_state.m_vRegs[0xF] = 1;
        }
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] - inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];

        inout_state.m_programCounter += 2;
    }
    
    //0x8XYE
    void ShiftVXLeft(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[0xF] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] >> 7;
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] <<= 1;
        inout_state.m_programCounter += 2;
    }
    
    //0x9XY0
    void SkipInstructionIfVXnoteqVY(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] != inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] ? 4 : 2;
    }
    
    //0xANNN
    void SetItoNNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_iReg = in_opCode & 0x0FFF;
        inout_state.m_programCounter += 2;
    }
    
    //0xBNNN
    void JumpPlus(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter = (in_opCode & 0x0FFF) + inout_state.m_vRegs[0];
    }
    
    //0xCXNN
    void SetVXtoRand(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = (CSCore::Random::Generate<s32>() % 0xFF) & (in_opCode & 0x00FF);
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
    
    //0xDXYN
    void TogglePixel(OpCode in_opCode, Chip8::State& inout_state)
    {
        auto x = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        auto y = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        auto height = in_opCode & 0x000F;
        u16 pixel = 0;
        
        inout_state.m_vRegs[0xF] = 0;
        for (auto yline = 0; yline < height; yline++)
        {
            pixel = inout_state.m_memory[inout_state.m_iReg + yline];
            for(auto xline = 0; xline < 8; xline++)
            {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    if(inout_state.m_graphicsState[(x + xline + ((y + yline) * 64))] == 1)
                    {
                        inout_state.m_vRegs[0xF] = 1;
                    }
                    inout_state.m_graphicsState[x + xline + ((y + yline) * 64)] ^= 1;
                }
            }
        }
        
        //TODO: Set dirty flag
        
        inout_state.m_programCounter += 2;
    }
    
    //0xEX9E
    void SkipInstructionIfVXPressed(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += inout_state.m_keyState[inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]] != 0 ? 4 : 2;
    }
    
    //0xEXA1
    void SkipInstructionIfVXNotPressed(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += inout_state.m_keyState[inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]] == 0 ? 4 : 2;
    }
    
    //0xFX07
    void SetVXToDelayTimer(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_delayTimer;
        inout_state.m_programCounter += 2;
    }
    
    //0xFX15
    void SetDelayTimerToVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_delayTimer = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        inout_state.m_programCounter += 2;
    }
    
    //0xFX18
    void SetSoundTimerToVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_soundTimer = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        inout_state.m_programCounter += 2;
    }
    
    //0xFX1E: Adds VX to I
    void AddVXtoI(OpCode in_opCode, Chip8::State& inout_state)
    {
        if(inout_state.m_iReg + inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] > 0xFFF)
        {
            inout_state.m_vRegs[0xF] = 1; //Carry
        }
        else
        {
            inout_state.m_vRegs[0xF] = 0;
        }
        inout_state.m_iReg += inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        
        inout_state.m_programCounter += 2;
    }
    
    //0xFX0A
    void AwaitKeyPress(OpCode in_opCode, Chip8::State& inout_state)
    {
        //If a key is pressed we jump to the next instruction otherwise we wait here
        for(auto i=0; i<16; ++i)
        {
            if(inout_state.m_keyState[i] != 0)
            {
                inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = i;
                inout_state.m_programCounter += 2;
            }
        }
    }
    
    //0xFX29
    void SetItoSpriteLocationForVX(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_iReg = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] * 0x5;
        
        inout_state.m_programCounter += 2;
    }
    
    //0xFX55
    void StoreVRegs(OpCode in_opCode, Chip8::State& inout_state)
    {
        for (auto i=0; i<=((in_opCode & 0x0F00) >> 8); ++i)
        {
            inout_state.m_memory[inout_state.m_iReg + i] = inout_state.m_vRegs[i];
        }
        
        inout_state.m_iReg += ((in_opCode & 0x0F00) >> 8) + 1;
        inout_state.m_programCounter += 2;
    }
    
    //0xFX65
    void FillVRegs(OpCode in_opCode, Chip8::State& inout_state)
    {
        for (auto i=0; i<=((in_opCode & 0x0F00) >> 8); ++i)
        {
            inout_state.m_vRegs[i] = inout_state.m_memory[inout_state.m_iReg + i];
        }
        
        // On the original interpreter, when the operation is done, I = I + X + 1.
        inout_state.m_iReg += ((in_opCode & 0x0F00) >> 8) + 1;
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
    
    //Load the font set into memory
    std::copy(std::begin(k_fontSet), std::end(k_fontSet), m_state.m_memory);
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
    

