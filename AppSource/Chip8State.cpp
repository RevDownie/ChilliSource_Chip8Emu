#include "Chip8State.h"

namespace
{
	const f32 k_executionHz = 1.0f/60.0f;
}

void Chip8State::OnInit()
{
	m_executionTimer = k_executionHz;
    m_chip8.LoadROM("Roms/BLINKY");
}

void Chip8State::OnUpdate(f32 in_dt)
{
	m_executionTimer -= in_dt;

	if (m_executionTimer <= 0.0f)
	{
		m_executionTimer = k_executionHz;
        m_chip8.FetchDecodeExecute();
	}
}
