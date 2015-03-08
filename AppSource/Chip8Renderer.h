//
//  Chip8Renderer.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 07/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _CHIP8RENDERER_H_
#define _CHIP8RENDERER_H_

#include <ChilliSource/ChilliSource.h>

#include "Chip8Constants.h"
#include "Chip8MutableState.h"

#include <array>

class Chip8Renderer
{
public:
    //----------------------------------------------------------------
    /// Construct the renderer to render to the given scene
    ///
    /// @author S Downie
    ///
    /// @param in_scene - Scene to render "pixels" to
    //----------------------------------------------------------------
    void Build(CSCore::Scene* in_scene);
    //----------------------------------------------------------------
    /// Updates the screen based on the current graphics memory state.
    ///
    /// @author S Downie
    ///
    /// @param inout_state - State used to refresh screen.
    //----------------------------------------------------------------
    void Draw(Chip8MutableState& inout_state);
    
private:
    
    std::array<CSRendering::SpriteComponent*, Chip8Constants::k_screenResolution> m_sprites;
};

#endif
