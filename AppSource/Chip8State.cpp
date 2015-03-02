#include "Chip8State.h"

#include "OpCodeUnitTests.h"


void Chip8State::OnInit()
{
    OpCodeUnitTests::RunTests();
    
    m_chip8 = new Chip8(GetScene());
    m_chip8->LoadROM("Roms/PONG");
}

void Chip8State::OnFixedUpdate(f32 in_dt)
{
    m_chip8->FetchDecodeExecute();
    m_chip8->Draw();
}
