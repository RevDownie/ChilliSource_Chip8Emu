//
//  Chip8Keyboard.cpp
//  CSChip8Emulator
//
//  Created by Scott Downie on 08/03/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#include "Chip8Keyboard.h"

#include <ChilliSource/Core/Base.h>

//----------------------------------------------------------------
//----------------------------------------------------------------
Chip8Keyboard::Chip8Keyboard()
{
    m_keyboard = CSCore::Application::Get()->GetSystem<CSInput::Keyboard>();
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8Keyboard::UpdateKeyStates(Chip8MutableState& inout_state)
{
    if(m_keyboard != nullptr)
    {
        inout_state.m_keyState[0] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_num1);
        inout_state.m_keyState[1] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_num2);
        inout_state.m_keyState[2] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_num3);
        inout_state.m_keyState[3] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_num4);
        inout_state.m_keyState[4] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_q);
        inout_state.m_keyState[5] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_w);
        inout_state.m_keyState[6] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_e);
        inout_state.m_keyState[7] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_r);
        inout_state.m_keyState[8] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_a);
        inout_state.m_keyState[9] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_s);
        inout_state.m_keyState[10] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_d);
        inout_state.m_keyState[11] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_f);
        inout_state.m_keyState[12] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_z);
        inout_state.m_keyState[13] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_x);
        inout_state.m_keyState[14] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_c);
        inout_state.m_keyState[15] = (u8)m_keyboard->IsKeyDown(CSInput::KeyCode::k_v);
    }
}