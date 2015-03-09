#ifndef _APP_STATES_CHIP8STATE_H_
#define _APP_STATES_CHIP8STATE_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/State/State.h>
#include <ChilliSource/Core/Delegate.h>

#include "Chip8MutableState.h"
#include "Chip8CPU.h"
#include "Chip8Renderer.h"
#include "Chip8Keyboard.h"

class Chip8State : public CSCore::State
{
public:

    //----------------------------------------------------------------
    /// Construct the state with the ROM at the given file stream
    ///
    /// @author S Downie
    ///
    /// @param in_romStream - Stream to load ROM from.
    //----------------------------------------------------------------
    Chip8State(CSCore::FileStreamUPtr in_romStream);
    //----------------------------------------------------------------
    /// Initialise the memory, registers and timers. Creates
    /// the renderer
    ///
    /// @author S Downie
    //----------------------------------------------------------------
	void OnInit() override;
    //----------------------------------------------------------------
    /// Performs a single CPU cycle and renders to screen. This needs
    /// to be done at 60Hz, hence fixed update.
    ///
    /// @author S Downie
    ///
    /// @param in_dt - Time since last update (s).
    //----------------------------------------------------------------
	void OnFixedUpdate(f32 in_dt) override;
    
private:
    
    //----------------------------------------------------------------
    /// Load the Chip8 Rom from the rom stream into VM memory
    ///
    /// @author S Downie
    //----------------------------------------------------------------
    void LoadROM();
    //----------------------------------------------------------------
    /// Reset the VM back to its initial state
    ///
    /// @author S Downie
    //----------------------------------------------------------------
    void Reset();
    //----------------------------------------------------------------
    /// Load in-game hud, including reset and pause buttons.
    ///
    /// @author S Downie
    //----------------------------------------------------------------
    void LoadInGameHud();

private:
    

    Chip8Renderer m_renderer;
    Chip8CPU m_cpu;
    Chip8Keyboard m_keyboard;
    Chip8MutableState m_state;
    
    CSCore::FileStreamUPtr m_romStream;
    CSCore::EventConnectionUPtr m_pauseButtonConnection;
    CSCore::EventConnectionUPtr m_resetButtonConnection;
    
    bool m_paused = false;
    bool m_reset = false;
};

#endif

