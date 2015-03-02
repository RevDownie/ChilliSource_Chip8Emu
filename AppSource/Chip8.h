//
//  Chip8.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 22/02/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _CHIP_8_
#define _CHIP_8_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/Delegate.h>

#include <algorithm>
#include <functional>
#include <string>

class Chip8
{
public:
    
    Chip8(CSCore::Scene* in_scene);
    
    void Reset();
    void LoadROM(const std::string& in_romPath);
    void FetchDecodeExecute();
    void Draw();
    
    struct State
    {
        u8 m_memory[4096];
        
        u8 m_vRegs[16];
        u16 m_iReg;
        
        u16 m_programCounter;
        u16 m_stackPointer;
        u16 m_stack[16];
        
        u8 m_graphicsState[64 * 32];
        u8 m_keyState[16];
        
        u32 m_delayTimer;
        u32 m_soundTimer;
        
        bool m_shouldRedraw;
    };
    
private:
    
    State m_state;
    
    CSRendering::SpriteComponentSPtr m_sprites[64 * 32];
    CSCore::EntitySPtr m_entities[64 * 32];
    CSCore::Scene* m_scene;
};

#endif
