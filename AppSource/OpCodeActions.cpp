//
//  OpCodeActions.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 28/02/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "OpCodeActions.h"

#include <ChilliSource/Core/Math.h>

namespace OpCodeActions
{
    extern const std::unordered_map<OpCode, std::function<void(OpCode, Chip8::State&)>> k_actions;
    
    //Use in place of unknown OpCode
    void NoOp(OpCode in_opCode, Chip8::State& inout_state)
    {
        CS_LOG_ERROR("No such op code: " + CSCore::ToString(in_opCode));
        inout_state.m_programCounter += 2;
    }
    
    //Forwarding method that decides what to do based on the second nibble
    void x0NNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        OpCode subCode = (in_opCode & 0xf0ff);
        
        auto actionIt = OpCodeActions::k_actions.find(subCode);
        if(actionIt != OpCodeActions::k_actions.end())
        {
            actionIt->second(subCode, inout_state);
        }
        else
        {
            OpCodeActions::NoOp(subCode, inout_state);
        }
    }
    
    //Clear the screen
    void x00E0(OpCode in_opCode, Chip8::State& inout_state)
    {
        std::fill(std::begin(inout_state.m_graphicsState), std::end(inout_state.m_graphicsState), 0);
        inout_state.m_shouldRedraw = true;
        inout_state.m_programCounter += 2;
    }
    
    //Jump program to NNN
    void x1NNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter = in_opCode & 0x0FFF;
    }
    
    //Jump to NNN and push the stack (call subroutine)
    void x2NNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_stack[inout_state.m_stackPointer] = inout_state.m_programCounter;
        ++inout_state.m_stackPointer;
        inout_state.m_programCounter = in_opCode & 0x0FFF;
    }
    
    //Return from subroutine
    void x00EE(OpCode in_opCode, Chip8::State& inout_state)
    {
        --inout_state.m_stackPointer;
        inout_state.m_programCounter = inout_state.m_stack[inout_state.m_stackPointer];
        inout_state.m_programCounter += 2;
    }
    
    //Skip instruction if VX == NN
    void x3XNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] == (in_opCode & 0x00FF) ? 4 : 2);
    }
    
    //Skip instruction if VX != NN
    void x4XNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] != (in_opCode & 0x00FF) ? 4 : 2);
    }
    
    //Skip instruction if VX == VY
    void x5XY0(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] == inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] ? 4 : 2);
    }
    
    //Set VX to NN
    void x6XNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = in_opCode & 0x00FF;
        inout_state.m_programCounter += 2;
    }
    
    //Add NN to VX
    void x7XNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] += in_opCode & 0x00FF;
        inout_state.m_programCounter += 2;
    }
    
    //Forwarding method that decides what to do based on the second nibble
    void x8NNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        OpCode subCode = (in_opCode & 0xf00f) + 0xff0;
        
        auto actionIt = OpCodeActions::k_actions.find(subCode);
        if(actionIt != OpCodeActions::k_actions.end())
        {
            actionIt->second(subCode, inout_state);
        }
        else
        {
            OpCodeActions::NoOp(subCode, inout_state);
        }
    }
    
    //Set VX to VY
    void x8XY0(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VX | VY
    void x8XY1(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] | inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VX & VY
    void x8XY2(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] & inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VX ^ VY
    void x8XY3(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] ^ inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        inout_state.m_programCounter += 2;
    }
    
    //Add VY to VX and store overflow in carry
    void x8XY4(OpCode in_opCode, Chip8::State& inout_state)
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
    
    //Subtract VY from VX
    void x8XY5(OpCode in_opCode, Chip8::State& inout_state)
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
    
    //Shift VX right
    void x8XY6(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[0xF] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] & 0x1;
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] >>= 1;
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VY - VX
    void x8XY7(OpCode in_opCode, Chip8::State& inout_state)
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
    
    //Shift VX left
    void x8XYE(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[0xF] = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] >> 7;
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] <<= 1;
        inout_state.m_programCounter += 2;
    }
    
    //Skip instruction if VX != VY
    void x9XY0(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] != inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4] ? 4 : 2);
    }
    
    //Set I to NNN
    void xANNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_iReg = in_opCode & 0x0FFF;
        inout_state.m_programCounter += 2;
    }
    
    //Jump program to NNN + V0
    void xBNNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter = (in_opCode & 0x0FFF) + inout_state.m_vRegs[0];
    }
    
    //Set VX to random masked by NN
    void xCXNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = CSCore::Random::Generate<u8>() & (in_opCode & 0x00FF);
        inout_state.m_programCounter += 2;
    }
    
    //Toggle pixel at X, Y on or off
    void xDXYN(OpCode in_opCode, Chip8::State& inout_state)
    {
        auto x = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        auto y = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
        auto height = in_opCode & 0x000F;
        u16 pixel = 0;
        
        inout_state.m_vRegs[0xF] = 0;
        for (auto yline = 0; yline < height; ++yline)
        {
            pixel = inout_state.m_memory[inout_state.m_iReg + yline];
            for(auto xline = 0; xline < 8; ++xline)
            {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    auto index = (x + xline + ((y + yline) * 64));
                    if(inout_state.m_graphicsState[index] == 1)
                    {
                        inout_state.m_vRegs[0xF] = 1;
                    }
                    inout_state.m_graphicsState[index] ^= 1;
                }
            }
        }
        
        inout_state.m_shouldRedraw = true;
        
        inout_state.m_programCounter += 2;
    }
    
    //Forwarding method that decides what to do based on the second nibble
    void xENNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        OpCode subCode = in_opCode & 0xf00f;
        
        auto actionIt = OpCodeActions::k_actions.find(subCode);
        if(actionIt != OpCodeActions::k_actions.end())
        {
            actionIt->second(subCode, inout_state);
        }
        else
        {
            OpCodeActions::NoOp(subCode, inout_state);
        }
    }
    
    //Skip instruction if key at VX is pressed
    void xEX9E(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_keyState[inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]] != 0 ? 4 : 2);
    }
    
    //Skip instruction if key at VX is not pressed
    void xEXA1(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_keyState[inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]] == 0 ? 4 : 2);
    }
    
    //Forwarding method that decides what to do based on the second nibble
    void xFNNN(OpCode in_opCode, Chip8::State& inout_state)
    {
        OpCode subCode = in_opCode & 0xf0ff;
        
        auto actionIt = OpCodeActions::k_actions.find(subCode);
        if(actionIt != OpCodeActions::k_actions.end())
        {
            actionIt->second(subCode, inout_state);
        }
        else
        {
            OpCodeActions::NoOp(subCode, inout_state);
        }
    }
    
    //Set VX to delay timer
    void xFX07(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_delayTimer;
        inout_state.m_programCounter += 2;
    }
    
    //Set delay timer to VX
    void xFX15(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_delayTimer = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        inout_state.m_programCounter += 2;
    }
    
    //Set sound timer to VX
    void xFX18(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_soundTimer = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
        inout_state.m_programCounter += 2;
    }
    
    //Store binary coded VX to memory
    void xFX33(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_memory[inout_state.m_iReg]     = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] / 100);
        inout_state.m_memory[inout_state.m_iReg + 1] = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] / 10) % 10;
        inout_state.m_memory[inout_state.m_iReg + 2] = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] % 100) % 10;
        
        inout_state.m_programCounter += 2;
    }
    
    //Add VX to I
    void xFX1E(OpCode in_opCode, Chip8::State& inout_state)
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
    
    //Await key press
    void xFX0A(OpCode in_opCode, Chip8::State& inout_state)
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
    
    //Set I to location of sprite at VX
    void xFX29(OpCode in_opCode, Chip8::State& inout_state)
    {
        inout_state.m_iReg = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] * 0x5;
        
        inout_state.m_programCounter += 2;
    }
    
    //Store all V in memory
    void xFX55(OpCode in_opCode, Chip8::State& inout_state)
    {
        for (auto i=0; i<=((in_opCode & 0x0F00) >> 8); ++i)
        {
            inout_state.m_memory[inout_state.m_iReg + i] = inout_state.m_vRegs[i];
        }
        
        inout_state.m_iReg += ((in_opCode & 0x0F00) >> 8) + 1;
        inout_state.m_programCounter += 2;
    }
    
    //Fetch all V from memory
    void xFX65(OpCode in_opCode, Chip8::State& inout_state)
    {
        for (auto i=0; i<=((in_opCode & 0x0F00) >> 8); ++i)
        {
            inout_state.m_vRegs[i] = inout_state.m_memory[inout_state.m_iReg + i];
        }
        
        // On the original interpreter, when the operation is done, I = I + X + 1.
        inout_state.m_iReg += ((in_opCode & 0x0F00) >> 8) + 1;
        inout_state.m_programCounter += 2;
    }
    
    const std::unordered_map<OpCode, std::function<void(OpCode, Chip8::State&)>> k_actions =
    {
        {0x0000, CSCore::MakeDelegate(&x0NNN)},
        {0x00e0, CSCore::MakeDelegate(&x00E0)},
        {0x00ee, CSCore::MakeDelegate(&x00EE)},
        {0x1000, CSCore::MakeDelegate(&x1NNN)},
        {0x2000, CSCore::MakeDelegate(&x2NNN)},
        {0x3000, CSCore::MakeDelegate(&x3XNN)},
        {0x4000, CSCore::MakeDelegate(&x4XNN)},
        {0x5000, CSCore::MakeDelegate(&x5XY0)},
        {0x6000, CSCore::MakeDelegate(&x6XNN)},
        {0x7000, CSCore::MakeDelegate(&x7XNN)},
        {0x8000, CSCore::MakeDelegate(&x8NNN)},
        {0x8FF0, CSCore::MakeDelegate(&x8XY0)},
        {0x8FF1, CSCore::MakeDelegate(&x8XY1)},
        {0x8FF2, CSCore::MakeDelegate(&x8XY2)},
        {0x8FF3, CSCore::MakeDelegate(&x8XY3)},
        {0x8FF4, CSCore::MakeDelegate(&x8XY4)},
        {0x8FF5, CSCore::MakeDelegate(&x8XY5)},
        {0x8FF6, CSCore::MakeDelegate(&x8XY6)},
        {0x8FF7, CSCore::MakeDelegate(&x8XY7)},
        {0x8FFE, CSCore::MakeDelegate(&x8XYE)},
        {0x9000, CSCore::MakeDelegate(&x9XY0)},
        {0xA000, CSCore::MakeDelegate(&xANNN)},
        {0xB000, CSCore::MakeDelegate(&xBNNN)},
        {0xC000, CSCore::MakeDelegate(&xCXNN)},
        {0xD000, CSCore::MakeDelegate(&xDXYN)},
        {0xE000, CSCore::MakeDelegate(&xENNN)},
        {0xE00E, CSCore::MakeDelegate(&xEX9E)},
        {0xE001, CSCore::MakeDelegate(&xEXA1)},
        {0xF000, CSCore::MakeDelegate(&xFNNN)},
        {0xF007, CSCore::MakeDelegate(&xFX07)},
        {0xF00A, CSCore::MakeDelegate(&xFX0A)},
        {0xF015, CSCore::MakeDelegate(&xFX15)},
        {0xF018, CSCore::MakeDelegate(&xFX18)},
        {0xF01E, CSCore::MakeDelegate(&xFX1E)},
        {0xF029, CSCore::MakeDelegate(&xFX29)},
        {0xF033, CSCore::MakeDelegate(&xFX33)},
        {0xF055, CSCore::MakeDelegate(&xFX55)},
        {0xF065, CSCore::MakeDelegate(&xFX65)}
    };
    
    std::function<void(Chip8::State& inout_state)> Decode(OpCode in_opCode)
    {
        auto key = in_opCode & 0xf000;
        auto actionIt = OpCodeActions::k_actions.find(key);
        if(actionIt != OpCodeActions::k_actions.end())
        {
            return std::bind(actionIt->second, in_opCode, std::placeholders::_1);
        }
        
        return std::bind(&OpCodeActions::NoOp, in_opCode, std::placeholders::_1);
    }
}
