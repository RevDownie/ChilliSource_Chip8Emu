#ifndef _APP_STATES_ROMPICKERSTATE_H_
#define _APP_STATES_ROMPICKERSTATE_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/State/State.h>

#include "UIRomPicker.h"

class RomPickerState : public CSCore::State
{
public:

    //----------------------------------------------------------------
    /// Shows the rom picker and loads the game.
    ///
    /// @author S Downie
    //----------------------------------------------------------------
	void OnInit() override;

private:
    
	UIRomPicker m_picker;
};

#endif

