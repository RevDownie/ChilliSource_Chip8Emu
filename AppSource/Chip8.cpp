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
#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/Entity.h>
#include <ChilliSource/Core/Scene.h>
#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/Rendering/Base.h>
#include <ChilliSource/Rendering/Camera.h>
#include <ChilliSource/Rendering/Material.h>

#include "OpCodeActions.h"

namespace
{    
    const u32 k_initMemOffset = 0x200;
    
    const CSCore::Colour k_pixelColours[2] =
    {
        CSCore::Colour(0.0f, 0.0f, 0.0f, 1.0f),
        CSCore::Colour(0.0f, 1.0f, 1.0f, 1.0f)
    };
    
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
}

Chip8::Chip8(CSCore::Scene* in_scene)
: m_scene(in_scene)
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
    m_state.m_shouldRedraw = true;
    //The program counter is offset as traditionally the interpreter would occupy the first 512 bytes.
    m_state.m_programCounter = k_initMemOffset;
    
    //Load the font set into memory
    std::copy(std::begin(k_fontSet), std::end(k_fontSet), m_state.m_memory);
    
    auto materialFactory = CSCore::Application::Get()->GetSystem<CSRendering::MaterialFactory>();
    auto renderFactory = CSCore::Application::Get()->GetSystem<CSRendering::RenderComponentFactory>();
    auto resourcePool = CSCore::Application::Get()->GetResourcePool();
    CSRendering::MaterialCSPtr material = materialFactory->CreateSprite("Sprite", resourcePool->LoadResource<CSRendering::Texture>(CSCore::StorageLocation::k_package, "Textures/White.png"));
    
    for(s32 i=0; i<64*32; ++i)
    {
        m_sprites[i] = renderFactory->CreateSpriteComponent(CSCore::Vector2::k_one, material, CSRendering::SpriteComponent::SizePolicy::k_none);
        m_sprites[i]->SetOriginAlignment(CSRendering::AlignmentAnchor::k_topLeft);
        m_entities[i] = CSCore::Entity::Create();
        m_entities[i]->AddComponent(m_sprites[i]);
        m_entities[i]->GetTransform().SetPosition((i % 64) - 31, 32 - (i / 64) - 17, 1.0f);
        m_scene->Add(m_entities[i]);
    }
    
    CSRendering::CameraComponentSPtr camera = renderFactory->CreateOrthographicCameraComponent(CSCore::Vector2(64, 32), 0.1f, 10.0f);
    CSCore::EntitySPtr cameraEnt = CSCore::Entity::Create();
    cameraEnt->AddComponent(camera);
    m_scene->Add(cameraEnt);
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
    auto action = OpCodeActions::Decode(nextOpCode);
    
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

void Chip8::Draw()
{
    if(m_state.m_shouldRedraw == true)
    {
        for(u32 i=0; i<64*32; ++i)
        {
            m_sprites[i]->SetColour(k_pixelColours[m_state.m_graphicsState[i]]);
        }
        
        m_state.m_shouldRedraw = false;
    }
}
    

