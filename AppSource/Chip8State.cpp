#include "Chip8State.h"

#include <algorithm>

namespace
{
	const f32 k_executionHz = 60.0f;

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

void Chip8State::OnInit()
{
	m_executionTimer = k_executionHz;

	std::fill(std::begin(m_memory), std::end(m_memory), 0);
	std::fill(std::begin(m_vRegs), std::end(m_vRegs), 0);
	std::fill(std::begin(m_stack), std::end(m_stack), 0);
	std::fill(std::begin(m_graphicsState), std::end(m_graphicsState), 0);
	std::fill(std::begin(m_keyState), std::end(m_keyState), 0);
}

void Chip8State::OnUpdate(f32 in_dt)
{
	m_executionTimer -= in_dt;

	if (m_executionTimer <= 0.0f)
	{
		m_executionTimer = k_executionHz;
		ExecuteCycle();
	}
}

void Chip8State::ExecuteCycle()
{
	OpCode nextOpCode = FetchNextOpcode();

	m_delayTimer = UpdateTimer(m_delayTimer, []()
	{
	});

	m_soundTimer = UpdateTimer(m_soundTimer, []()
	{
		CS_LOG_VERBOSE("Beep!");
	});
}

Chip8State::OpCode Chip8State::FetchNextOpcode()
{
	return m_memory[m_programCounter] << 8 | m_memory[m_programCounter + 1];
}