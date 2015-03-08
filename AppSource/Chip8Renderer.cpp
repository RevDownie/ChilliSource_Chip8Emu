//
//  Chip8Renderer.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 07/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "Chip8Renderer.h"

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/Entity.h>
#include <ChilliSource/Core/Scene.h>
#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/Rendering/Base.h>
#include <ChilliSource/Rendering/Camera.h>
#include <ChilliSource/Rendering/Material.h>

namespace
{
    const CSCore::Colour k_pixelColours[2] =
    {
        CSCore::Colour(231.0f/255.0f, 197.0f/255.0f, 74.0f/255.0f, 1.0f),
        CSCore::Colour(231.0f/255.0f, 74.0f/255.0f, 108.0f/255.0f, 1.0f)
    };
}

//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8Renderer::Build(CSCore::Scene* in_scene)
{
    auto materialFactory = CSCore::Application::Get()->GetSystem<CSRendering::MaterialFactory>();
    auto renderFactory = CSCore::Application::Get()->GetSystem<CSRendering::RenderComponentFactory>();
    auto resourcePool = CSCore::Application::Get()->GetResourcePool();
    CSRendering::MaterialCSPtr material = materialFactory->CreateSprite("Sprite", resourcePool->LoadResource<CSRendering::Texture>(CSCore::StorageLocation::k_package, "Textures/White.png"));
    
    
    for(s32 i=0; i<m_sprites.size(); ++i)
    {
        CSRendering::SpriteComponentSPtr sprite = renderFactory->CreateSpriteComponent(CSCore::Vector2::k_one, material, CSRendering::SpriteComponent::SizePolicy::k_none);
        sprite->SetOriginAlignment(CSRendering::AlignmentAnchor::k_topLeft);
        CSCore::EntitySPtr spriteEnt = CSCore::Entity::Create();
        spriteEnt->AddComponent(sprite);
        spriteEnt->GetTransform().SetPosition((i % Chip8Constants::k_screenWidth) - (Chip8Constants::k_screenWidth/2 - 1),
                                              Chip8Constants::k_screenHeight - (i / Chip8Constants::k_screenWidth) - (Chip8Constants::k_screenHeight/2 + 1),
                                              1.0f);
        m_sprites[i] = sprite.get();
        in_scene->Add(spriteEnt);
    }
    
    CSRendering::CameraComponentSPtr camera = renderFactory->CreateOrthographicCameraComponent(CSCore::Vector2(Chip8Constants::k_screenWidth, Chip8Constants::k_screenHeight), 0.1f, 10.0f);
    CSCore::EntitySPtr cameraEnt = CSCore::Entity::Create();
    cameraEnt->AddComponent(camera);
    in_scene->Add(cameraEnt);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8Renderer::Draw(Chip8MutableState& inout_state)
{
    if(inout_state.m_shouldRedraw == true)
    {
        for(u32 i=0; i<m_sprites.size(); ++i)
        {
            m_sprites[i]->SetColour(k_pixelColours[inout_state.m_graphicsMemory[i]]);
        }
        
        inout_state.m_shouldRedraw = false;
    }
}