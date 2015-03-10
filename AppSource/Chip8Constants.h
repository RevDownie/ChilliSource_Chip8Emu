//
//  Chip8Constants.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 07/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _CHIP8CONSTANTS_H_
#define _CHIP8CONSTANTS_H_

#include <ChilliSource/Core/Base.h>

namespace Chip8Constants
{
    const u32 k_framesPerSecond = 60;
    const u32 k_opsPerSecond = 600;
    const u32 k_opsPerUpdate = k_opsPerSecond/k_framesPerSecond;
    
    const s32 k_screenWidth = 64;
    const s32 k_screenHeight = 32;
    const s32 k_screenResolution = k_screenWidth * k_screenHeight;
    const u32 k_memorySize = 4094;
    const u32 k_stackSize = 16;
    const u32 k_numVRegisters = 16;
    const u32 k_numKeys = 16;
    
    const u32 k_initMemOffset = 0x200;

	const CSCore::Colour k_backgroundColour(231.0f / 255.0f, 197.0f / 255.0f, 74.0f / 255.0f, 1.0f);
	const CSCore::Colour k_foregroundColour(231.0f / 255.0f, 74.0f / 255.0f, 108.0f / 255.0f, 1.0f);
    
    const u8 k_fontSet[80] =
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

#endif
