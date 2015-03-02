//
//  OpCodeUnitTests.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 02/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "OpCodeUnitTests.h"

#include "OpCodeActions.h"
#include "Chip8.h"

namespace OpCodeUnitTests
{
    void Reset(Chip8::State& out_state)
    {
        std::fill(std::begin(out_state.m_memory), std::end(out_state.m_memory), 0);
        std::fill(std::begin(out_state.m_vRegs), std::end(out_state.m_vRegs), 0);
        std::fill(std::begin(out_state.m_stack), std::end(out_state.m_stack), 0);
        std::fill(std::begin(out_state.m_graphicsState), std::end(out_state.m_graphicsState), 0);
        std::fill(std::begin(out_state.m_keyState), std::end(out_state.m_keyState), 0);
        
        out_state.m_iReg = 0;
        out_state.m_stackPointer = 0;
        out_state.m_delayTimer = 0;
        out_state.m_soundTimer = 0;
        out_state.m_shouldRedraw = true;
        //The program counter is offset as traditionally the interpreter would occupy the first 512 bytes.
        out_state.m_programCounter = 512;
    }
    
    //Clear the screen
    void x00E0()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x00E0(0x0000, state);
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
        CS_ASSERT(state.m_shouldRedraw == true, "FAILED");
        for(auto px : state.m_graphicsState)
        {
            CS_ASSERT(px == 0, "FAILED");
        }
    }
    
    //Jump program to NNN
    void x1NNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x1NNN(0x1234, state);
        CS_ASSERT(state.m_programCounter == 0x0234, "FAILED");
    }
    
    //Jump to NNN and push the stack (call subroutine)
    void x2NNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x2NNN(0x1234, state);
        CS_ASSERT(state.m_stackPointer == 1, "FAILED");
        CS_ASSERT(state.m_stack[0] == 0x0200, "FAILED");
        CS_ASSERT(state.m_programCounter == 0x0234, "FAILED");
    }

    //Return from subroutine
    void x00EE()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x2NNN(0x1234, state);
        OpCodeActions::x00EE(0x0000, state);
        CS_ASSERT(state.m_stackPointer == 0, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }
    
    //Skip instruction if VX == NN
    void x3XNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x3XNN(0x3100, state);
        CS_ASSERT(state.m_programCounter == 516, "FAILED");
        
        Reset(state);
        OpCodeActions::x3XNN(0x3111, state);
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }
    
    //Skip instruction if VX != NN
    void x4XNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x4XNN(0x4100, state);
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
        
        Reset(state);
        OpCodeActions::x4XNN(0x4111, state);
        CS_ASSERT(state.m_programCounter == 516, "FAILED");
    }

    //Skip instruction if VX == VY
    void x5XY0()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[1] = 0;
        state.m_vRegs[2] = 1;
        OpCodeActions::x5XY0(0x5120, state);
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
        
        Reset(state);
        state.m_vRegs[1] = 0;
        state.m_vRegs[2] = 0;
        OpCodeActions::x5XY0(0x5120, state);
        CS_ASSERT(state.m_programCounter == 516, "FAILED");
    }

    //Set VX to NN
    void x6XNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::x6XNN(0x6420, state);
        CS_ASSERT(state.m_vRegs[4] == 0x0020, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }
    
    //Add NN to VX
    void x7XNN()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[5] = 0x0010;
        OpCodeActions::x7XNN(0x7520, state);
        CS_ASSERT(state.m_vRegs[5] == (0x0020 + 0x0010), "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }
    
    //Set VX to VY
    void x8XY0()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[1] = 0x0010;
        OpCodeActions::x8XY0(0x8010, state);
        CS_ASSERT(state.m_vRegs[0] ==  0x0010, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Set VX to VX | VY
    void x8XY1()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[0] = 0x0020;
        state.m_vRegs[1] = 0x0010;
        OpCodeActions::x8XY1(0x8011, state);
        CS_ASSERT(state.m_vRegs[0] == (0x0010 | 0x0020), "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Set VX to VX & VY
    void x8XY2()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[0] = 0x0020;
        state.m_vRegs[1] = 0x0010;
        OpCodeActions::x8XY2(0x8012, state);
        CS_ASSERT(state.m_vRegs[0] == (0x0010 & 0x0020), "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Set VX to VX ^ VY
    void x8XY3()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[0] = 0x0020;
        state.m_vRegs[1] = 0x0010;
        OpCodeActions::x8XY3(0x8013, state);
        CS_ASSERT(state.m_vRegs[0] == (0x0010 ^ 0x0020), "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Add VY to VX and store overflow in carry
    void x8XY4()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        state.m_vRegs[3] = 0x0010;
        OpCodeActions::x8XY4(0x8234, state);
        CS_ASSERT(state.m_vRegs[2] == (0x0010 + 0x0020), "FAILED");
        CS_ASSERT(state.m_vRegs[15] == 0, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
        
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        state.m_vRegs[3] = 0x00FF;
        OpCodeActions::x8XY4(0x8234, state);
        CS_ASSERT(state.m_vRegs[2] == (u8)(0x0020 + 0x00FF), "FAILED");
        CS_ASSERT(state.m_vRegs[15] == 1, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Subtract VY from VX
    void x8XY5()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        state.m_vRegs[3] = 0x0010;
        OpCodeActions::x8XY5(0x8235, state);
        CS_ASSERT(state.m_vRegs[2] == (0x0020 - 0x0010), "FAILED");
        CS_ASSERT(state.m_vRegs[15] == 1, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
        
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        state.m_vRegs[3] = 0x00FF;
        OpCodeActions::x8XY5(0x8235, state);
        CS_ASSERT(state.m_vRegs[2] == (u8)(0x0020 - 0x00FF), "FAILED");
        CS_ASSERT(state.m_vRegs[15] == 0, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Shift VX right
    void x8XY6()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        OpCodeActions::x8XY6(0x8236, state);
        CS_ASSERT(state.m_vRegs[2] == (0x0020 >> 1), "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Set VX to VY - VX
    void x8XY7()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        state.m_vRegs[3] = 0x0010;
        OpCodeActions::x8XY7(0x8237, state);
        CS_ASSERT(state.m_vRegs[2] == (u8)(0x0010 - 0x0020), "FAILED");
        CS_ASSERT(state.m_vRegs[15] == 0, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
        
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        state.m_vRegs[3] = 0x00FF;
        OpCodeActions::x8XY7(0x8237, state);
        CS_ASSERT(state.m_vRegs[2] == (u8)(0x00FF - 0x0020), "FAILED");
        CS_ASSERT(state.m_vRegs[15] == 1, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Shift VX left
    void x8XYE()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[2] = 0x0020;
        OpCodeActions::x8XYE(0x823E, state);
        CS_ASSERT(state.m_vRegs[2] == (0x0020 << 1), "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Skip instruction if VX != VY
    void x9XY0()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[1] = 0;
        state.m_vRegs[2] = 1;
        OpCodeActions::x9XY0(0x9120, state);
        CS_ASSERT(state.m_programCounter == 516, "FAILED");
        
        Reset(state);
        state.m_vRegs[1] = 0;
        state.m_vRegs[2] = 0;
        OpCodeActions::x9XY0(0x9120, state);
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Set I to NNN
    void xANNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::xANNN(0xA123, state);
        CS_ASSERT(state.m_iReg == 0x0123, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }

    //Jump program to NNN + V0
    void xBNNN()
    {
        Chip8::State state;
        Reset(state);
        state.m_vRegs[0] = 0x002;
        OpCodeActions::xBNNN(0xB123, state);
        CS_ASSERT(state.m_programCounter == 0x123 + 0x002, "FAILED");
    }

    //Set VX to random masked by NN
    void xCXNN()
    {
        Chip8::State state;
        Reset(state);
        OpCodeActions::xCXNN(0xC613, state);
        CS_ASSERT(state.m_vRegs[6] != 0, "FAILED");
        CS_ASSERT(state.m_programCounter == 514, "FAILED");
    }
//
//    //Toggle pixel at X, Y on or off
//    void xDXYN()
//    {
//        auto x = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
//        auto y = inout_state.m_vRegs[(in_opCode & 0x00F0) >> 4];
//        auto height = in_opCode & 0x000F;
//        u16 pixel = 0;
//        
//        inout_state.m_vRegs[0xF] = 0;
//        for (auto yline = 0; yline < height; ++yline)
//        {
//            pixel = inout_state.m_memory[inout_state.m_iReg + yline];
//            for(auto xline = 0; xline < 8; ++xline)
//            {
//                if((pixel & (0x80 >> xline)) != 0)
//                {
//                    auto index = (x + xline + ((y + yline) * 64));
//                    if(inout_state.m_graphicsState[index] == 1)
//                    {
//                        inout_state.m_vRegs[0xF] = 1;
//                    }
//                    inout_state.m_graphicsState[index] ^= 1;
//                }
//            }
//        }
//        
//        inout_state.m_shouldRedraw = true;
//        
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Skip instruction if key at VX is pressed
//    void xEX9E()
//    {
//        inout_state.m_programCounter += inout_state.m_keyState[inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]] != 0 ? 4 : 2;
//    }
//    
//    //Skip instruction if key at VX is not pressed
//    void xEXA1()
//    {
//        inout_state.m_programCounter += inout_state.m_keyState[inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8]] == 0 ? 4 : 2;
//    }
//    
//    //Set VX to delay timer
//    void xFX07()
//    {
//        inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = inout_state.m_delayTimer;
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Set delay timer to VX
//    void xFX15()
//    {
//        inout_state.m_delayTimer = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Set sound timer to VX
//    void xFX18()
//    {
//        inout_state.m_soundTimer = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Store binary coded VX to memory
//    void xFX33()
//    {
//        inout_state.m_memory[inout_state.m_iReg]     = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] / 100);
//        inout_state.m_memory[inout_state.m_iReg + 1] = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] / 10) % 10;
//        inout_state.m_memory[inout_state.m_iReg + 2] = (inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] % 100) % 10;
//        
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Add VX to I
//    void xFX1E()
//    {
//        if(inout_state.m_iReg + inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] > 0xFFF)
//        {
//            inout_state.m_vRegs[0xF] = 1; //Carry
//        }
//        else
//        {
//            inout_state.m_vRegs[0xF] = 0;
//        }
//        inout_state.m_iReg += inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8];
//        
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Await key press
//    void xFX0A()
//    {
//        //If a key is pressed we jump to the next instruction otherwise we wait here
//        for(auto i=0; i<16; ++i)
//        {
//            if(inout_state.m_keyState[i] != 0)
//            {
//                inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] = i;
//                inout_state.m_programCounter += 2;
//            }
//        }
//    }
//    
//    //Set I to location of sprite at VX
//    void xFX29()
//    {
//        inout_state.m_iReg = inout_state.m_vRegs[(in_opCode & 0x0F00) >> 8] * 0x5;
//        
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Store all V in memory
//    void xFX55()
//    {
//        for (auto i=0; i<=((in_opCode & 0x0F00) >> 8); ++i)
//        {
//            inout_state.m_memory[inout_state.m_iReg + i] = inout_state.m_vRegs[i];
//        }
//        
//        inout_state.m_iReg += ((in_opCode & 0x0F00) >> 8) + 1;
//        inout_state.m_programCounter += 2;
//    }
//    
//    //Fetch all V from memory
//    void xFX65()
//    {
//        for (auto i=0; i<=((in_opCode & 0x0F00) >> 8); ++i)
//        {
//            inout_state.m_vRegs[i] = inout_state.m_memory[inout_state.m_iReg + i];
//        }
//        
//        // On the original interpreter, when the operation is done, I = I + X + 1.
//        inout_state.m_iReg += ((in_opCode & 0x0F00) >> 8) + 1;
//        inout_state.m_programCounter += 2;
//    }
    
    void RunTests()
    {
        x00E0();
        x1NNN();
        x2NNN();
        x00EE();
        x3XNN();
        x4XNN();
        x5XY0();
        x6XNN();
        x7XNN();
        x8XY0();
        x8XY1();
        x8XY2();
        x8XY3();
        x8XY4();
        x8XY5();
        x8XY6();
        x8XY7();
        x8XYE();
        x9XY0();
        xANNN();
        xBNNN();
        xCXNN();
    }
}