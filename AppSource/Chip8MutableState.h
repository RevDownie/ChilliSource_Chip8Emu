//
//  Chip8MutableState.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 07/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _CHIP8MUTABLESTATE_H_
#define _CHIP8MUTABLESTATE_H_

#include <ChilliSource/ChilliSource.h>

#include "Chip8Constants.h"

struct Chip8MutableState
{
    u8 m_memory[Chip8Constants::k_memorySize];
    
    u8 m_vRegs[Chip8Constants::k_numVRegisters];
    u16 m_iReg;
    
    u16 m_programCounter;
    u16 m_stackPointer;
    u16 m_stack[Chip8Constants::k_stackSize];
    
    u8 m_graphicsMemory[Chip8Constants::k_screenResolution];
    u8 m_keyState[Chip8Constants::k_numKeys];
    
    u32 m_delayTimer;
    u32 m_soundTimer;
    
    bool m_shouldRedraw;
};

#endif
