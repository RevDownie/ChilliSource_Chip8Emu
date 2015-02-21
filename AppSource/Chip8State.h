#ifndef _APP_STATES_CHIP8STATE_H_
#define _APP_STATES_CHIP8STATE_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/State/State.h>

#include <functional>
#include <vector>

class Chip8State : public CSCore::State
{
public:

	using OpCode = u16;

	void OnInit() override;
	void OnUpdate(f32 in_dt) override;

private:

	void ExecuteCycle();
	OpCode FetchNextOpcode();

private:

	u8 m_memory[4096];
	
	u8 m_vRegs[16];
	u16 m_iReg = 0;

	u16 m_programCounter = 0x200; //The program counter is offset as traditionally the interpreter would occupy the first 512 bytes.
	u16 m_stackCounter = 0;
	u16 m_stack[16];

	u8 m_graphicsState[64 * 32];
	u8 m_keyState[16];

	u32 m_delayTimer = 0;
	u32 m_soundTimer = 0;

	f32 m_executionTimer;
};

#endif

