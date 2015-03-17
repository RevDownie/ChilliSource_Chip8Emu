//
//  OpCodeActions.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 28/02/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "OpCodeActions.h"

#include <ChilliSource/Core/Math.h>

#define C8_MASK_X(in_opCode) ((in_opCode & 0x0F00) >> 8)
#define C8_MASK_Y(in_opCode) ((in_opCode & 0x00F0) >> 4)
#define C8_MASK_NN(in_opCode) (in_opCode & 0x00FF)
#define C8_MASK_NNN(in_opCode) (in_opCode & 0x0FFF)

namespace OpCodeActions
{
    //Use in place of unknown OpCode
    void NoOp(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        CS_LOG_ERROR("No such op code: " + CSCore::ToString(in_opCode));
        inout_state.m_programCounter += 2;
    }
    
    //Clear the screen
    void x00E0(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        std::fill(std::begin(inout_state.m_graphicsMemory), std::end(inout_state.m_graphicsMemory), 0);
        inout_state.m_shouldRedraw = true;
        inout_state.m_programCounter += 2;
    }
    
    //Jump program to NNN
    void x1NNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter = C8_MASK_NNN(in_opCode);
    }
    
    //Jump to NNN and push the stack (call subroutine)
    void x2NNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_stack[inout_state.m_stackPointer] = inout_state.m_programCounter;
        ++inout_state.m_stackPointer;
        inout_state.m_programCounter = C8_MASK_NNN(in_opCode);
    }
    
    //Return from subroutine
    void x00EE(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        --inout_state.m_stackPointer;
        inout_state.m_programCounter = inout_state.m_stack[inout_state.m_stackPointer];
        inout_state.m_programCounter += 2;
    }
    
    //Skip instruction if VX == NN
    void x3XNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[C8_MASK_X(in_opCode)] == C8_MASK_NN(in_opCode) ? 4 : 2);
    }
    
    //Skip instruction if VX != NN
    void x4XNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
		inout_state.m_programCounter += (inout_state.m_vRegs[C8_MASK_X(in_opCode)] != C8_MASK_NN(in_opCode) ? 4 : 2);
    }
    
    //Skip instruction if VX == VY
    void x5XY0(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[C8_MASK_X(in_opCode)] == inout_state.m_vRegs[C8_MASK_Y(in_opCode)] ? 4 : 2);
    }
    
    //Set VX to NN
    void x6XNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = C8_MASK_NN(in_opCode);
        inout_state.m_programCounter += 2;
    }
    
    //Add NN to VX
    void x7XNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] += C8_MASK_NN(in_opCode);
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VY
    void x8XY0(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VX | VY
    void x8XY1(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = inout_state.m_vRegs[C8_MASK_X(in_opCode)] | inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VX & VY
    void x8XY2(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = inout_state.m_vRegs[C8_MASK_X(in_opCode)] & inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VX ^ VY
    void x8XY3(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = inout_state.m_vRegs[C8_MASK_X(in_opCode)] ^ inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        inout_state.m_programCounter += 2;
    }
    
    //Add VY to VX and store overflow in carry
    void x8XY4(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        if(inout_state.m_vRegs[C8_MASK_Y(in_opCode)] > (0xFF - inout_state.m_vRegs[C8_MASK_X(in_opCode)]))
        {
            inout_state.m_vRegs[0xF] = 1; //Carry
        }
        else
        {
            inout_state.m_vRegs[0xF] = 0;
        }
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] += inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        
        inout_state.m_programCounter += 2;
    }
    
    //Subtract VY from VX
    void x8XY5(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        if(inout_state.m_vRegs[C8_MASK_Y(in_opCode)] > (inout_state.m_vRegs[C8_MASK_X(in_opCode)]))
        {
            inout_state.m_vRegs[0xF] = 0; //Borrow
        }
        else
        {
            inout_state.m_vRegs[0xF] = 1;
        }
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] -= inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        
        inout_state.m_programCounter += 2;
    }
    
    //Shift VX right
    void x8XY6(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[0xF] = inout_state.m_vRegs[C8_MASK_X(in_opCode)] & 0x1;
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] >>= 1;
        inout_state.m_programCounter += 2;
    }
    
    //Set VX to VY - VX
    void x8XY7(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        if(inout_state.m_vRegs[C8_MASK_X(in_opCode)] > inout_state.m_vRegs[C8_MASK_Y(in_opCode)])	// VY-VX
        {
            inout_state.m_vRegs[0xF] = 0; //Borrow
        }
        else
        {
            inout_state.m_vRegs[0xF] = 1;
        }
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = inout_state.m_vRegs[C8_MASK_Y(in_opCode)] - inout_state.m_vRegs[C8_MASK_X(in_opCode)];
        
        inout_state.m_programCounter += 2;
    }
    
    //Shift VX left
    void x8XYE(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[0xF] = inout_state.m_vRegs[C8_MASK_X(in_opCode)] >> 7;
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] <<= 1;
        inout_state.m_programCounter += 2;
    }
    
    //Skip instruction if VX != VY
    void x9XY0(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_vRegs[C8_MASK_X(in_opCode)] != inout_state.m_vRegs[C8_MASK_Y(in_opCode)] ? 4 : 2);
    }
    
    //Set I to NNN
    void xANNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_iReg = C8_MASK_NNN(in_opCode);
        inout_state.m_programCounter += 2;
    }
    
    //Jump program to NNN + V0
    void xBNNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter = C8_MASK_NNN(in_opCode) + inout_state.m_vRegs[0];
    }
    
    //Set VX to random masked by NN
    void xCXNN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        //inout_state.m_vRegs[C8_MASK_X(in_opCode)] = CSCore::Random::Generate<u8>() & C8_MASK_NN(in_opCode);
		inout_state.m_vRegs[C8_MASK_X(in_opCode)] = rand() & C8_MASK_NN(in_opCode);
        inout_state.m_programCounter += 2;
    }
    
    //Toggle pixels at X, Y, and with width 8 and height N, on or off
    void xDXYN(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        auto x = inout_state.m_vRegs[C8_MASK_X(in_opCode)];
        auto y = inout_state.m_vRegs[C8_MASK_Y(in_opCode)];
        auto height = in_opCode & 0x000F;
            
        inout_state.m_vRegs[0xF] = 0;
        
        for (auto yline = 0; yline < height; ++yline)
        {
            if(y + yline >= Chip8Constants::k_screenHeight)
                break;
            
            auto pixel = inout_state.m_memory[inout_state.m_iReg + yline];
            for(auto xline = 0; xline < 8; ++xline)
            {
                if(x + xline >= Chip8Constants::k_screenWidth)
                    break;
                
                if((pixel & (0x80 >> xline)) != 0)
                {
                    auto index = (x + xline + ((y + yline) * 64));
                    if(inout_state.m_graphicsMemory[index] == 1)
                    {
                        //Set the collision detection flag.
                        inout_state.m_vRegs[0xF] = 1;
                    }
                    inout_state.m_graphicsMemory[index] ^= 1;
                }
            }
        }
        
        inout_state.m_shouldRedraw = true;
        inout_state.m_programCounter += 2;
    }
    
    //Skip instruction if key at VX is pressed
    void xEX9E(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_keyState[inout_state.m_vRegs[C8_MASK_X(in_opCode)]] != 0 ? 4 : 2);
    }
    
    //Skip instruction if key at VX is not pressed
    void xEXA1(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_programCounter += (inout_state.m_keyState[inout_state.m_vRegs[C8_MASK_X(in_opCode)]] == 0 ? 4 : 2);
    }
    
    //Set VX to delay timer
    void xFX07(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_vRegs[C8_MASK_X(in_opCode)] = inout_state.m_delayTimer;
        inout_state.m_programCounter += 2;
    }
    
    //Set delay timer to VX
    void xFX15(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_delayTimer = inout_state.m_vRegs[C8_MASK_X(in_opCode)];
        inout_state.m_programCounter += 2;
    }
    
    //Set sound timer to VX
    void xFX18(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_soundTimer = inout_state.m_vRegs[C8_MASK_X(in_opCode)];
        inout_state.m_programCounter += 2;
    }
    
    //Store binary coded VX to memory
    void xFX33(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_memory[inout_state.m_iReg]     = (inout_state.m_vRegs[C8_MASK_X(in_opCode)] / 100);
        inout_state.m_memory[inout_state.m_iReg + 1] = (inout_state.m_vRegs[C8_MASK_X(in_opCode)] / 10) % 10;
        inout_state.m_memory[inout_state.m_iReg + 2] = (inout_state.m_vRegs[C8_MASK_X(in_opCode)] % 100) % 10;
        
        inout_state.m_programCounter += 2;
    }
    
    //Add VX to I
    void xFX1E(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        if(inout_state.m_iReg + inout_state.m_vRegs[C8_MASK_X(in_opCode)] > 0xFFF)
        {
            inout_state.m_vRegs[0xF] = 1; //Carry
        }
        else
        {
            inout_state.m_vRegs[0xF] = 0;
        }
        inout_state.m_iReg += inout_state.m_vRegs[C8_MASK_X(in_opCode)];
        
        inout_state.m_programCounter += 2;
    }
    
    //Await key press
    void xFX0A(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        //If a key is pressed we jump to the next instruction otherwise we wait here
        for(auto i=0; i<16; ++i)
        {
            if(inout_state.m_keyState[i] != 0)
            {
                inout_state.m_vRegs[C8_MASK_X(in_opCode)] = i;
                inout_state.m_programCounter += 2;
            }
        }
    }
    
    //Set I to location of sprite at VX
    void xFX29(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        inout_state.m_iReg = inout_state.m_vRegs[C8_MASK_X(in_opCode)] * 0x5;
        
        inout_state.m_programCounter += 2;
    }
    
    //Store all V in memory
    void xFX55(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        for (auto i=0; i<=(C8_MASK_X(in_opCode)); ++i)
        {
            inout_state.m_memory[inout_state.m_iReg + i] = inout_state.m_vRegs[i];
        }
        
        inout_state.m_iReg += (C8_MASK_X(in_opCode)) + 1;
        inout_state.m_programCounter += 2;
    }
    
    //Fetch all V from memory
    void xFX65(OpCode in_opCode, Chip8MutableState& inout_state)
    {
        for (auto i=0; i<=(C8_MASK_X(in_opCode)); ++i)
        {
            inout_state.m_vRegs[i] = inout_state.m_memory[inout_state.m_iReg + i];
        }
        
        // On the original interpreter, when the operation is done, I = I + X + 1.
        inout_state.m_iReg += (C8_MASK_X(in_opCode)) + 1;
        inout_state.m_programCounter += 2;
    }
}
