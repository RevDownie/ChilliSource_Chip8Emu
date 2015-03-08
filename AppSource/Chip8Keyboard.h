//
//  Chip8Keyboard.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 08/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _CHIP8KEYBOARD_H_
#define _CHIP8KEYBOARD_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Input/Keyboard.h>

#include "Chip8MutableState.h"

class Chip8Keyboard
{
public:
    //----------------------------------------------------------------
    /// Setup the keyboard listeners to update the Chip8 keyboard state
    ///
    /// @author S Downie
    //----------------------------------------------------------------
    Chip8Keyboard();
    //----------------------------------------------------------------
    /// Check if the Chip8 keys are down or up and update the given
    /// state. Note: there are 16 keys - 1234,qwer,asdf,zxcv
    ///
    /// @author S Downie
    ///
    /// @param inout_state - State to update
    //----------------------------------------------------------------
    void UpdateKeyStates(Chip8MutableState& inout_state);
    
private:
    
    CSInput::Keyboard* m_keyboard = nullptr;
};

#endif
