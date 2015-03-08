#ifndef _APP_STATES_CHIP8STATE_H_
#define _APP_STATES_CHIP8STATE_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/State/State.h>

#include "Chip8MutableState.h"
#include "Chip8CPU.h"
#include "Chip8Renderer.h"

class Chip8State : public CSCore::State
{
public:

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
    /// Load the Chip8 Rom at the given path into VM memory
    ///
    /// @author S Downie
    ///
    /// @param in_romPath - Rom path in package.
    //----------------------------------------------------------------
    void LoadROM(const std::string& in_romPath);
    //----------------------------------------------------------------
    /// Reset the VM back to its initial state
    ///
    /// @author S Downie
    //----------------------------------------------------------------
    void Reset();

private:
    

    Chip8Renderer m_renderer;
    Chip8CPU m_cpu;
    Chip8MutableState m_state;
};

#endif

