#ifndef _APP_STATES_CHIP8STATE_H_
#define _APP_STATES_CHIP8STATE_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/State/State.h>

#include <vector>

#include "Chip8.h"

class Chip8State : public CSCore::State
{
public:

	void OnInit() override;
	void OnFixedUpdate(f32 in_dt) override;

private:
    
    Chip8* m_chip8;

	f32 m_executionTimer;
};

#endif

