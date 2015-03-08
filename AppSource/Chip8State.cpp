#include "Chip8State.h"

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/File.h>

//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::OnInit()
{
    LoadROM("Roms/BRIX");
    
    m_renderer.Build(GetScene());
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::LoadROM(const std::string& in_romPath)
{
    Reset();
    
    auto fileSystem = CSCore::Application::Get()->GetFileSystem();
    auto fileStream = fileSystem->CreateFileStream(CSCore::StorageLocation::k_package, in_romPath, CSCore::FileMode::k_readBinary);
    
    std::string rom;
    fileStream->GetAll(rom);
    std::copy(std::begin(rom), std::end(rom), &m_state.m_memory[Chip8Constants::k_initMemOffset]);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::Reset()
{
    std::fill(std::begin(m_state.m_memory), std::end(m_state.m_memory), 0);
    std::fill(std::begin(m_state.m_vRegs), std::end(m_state.m_vRegs), 0);
    std::fill(std::begin(m_state.m_stack), std::end(m_state.m_stack), 0);
    std::fill(std::begin(m_state.m_graphicsMemory), std::end(m_state.m_graphicsMemory), 0);
    std::fill(std::begin(m_state.m_keyState), std::end(m_state.m_keyState), 0);
    
    m_state.m_iReg = 0;
    m_state.m_stackPointer = 0;
    m_state.m_delayTimer = 0;
    m_state.m_soundTimer = 0;
    m_state.m_shouldRedraw = true;
    //The program counter is offset as traditionally the interpreter would occupy the first 512 bytes.
    m_state.m_programCounter = Chip8Constants::k_initMemOffset;
    
    //Load the font set into memory
    std::copy(std::begin(Chip8Constants::k_fontSet), std::end(Chip8Constants::k_fontSet), m_state.m_memory);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::OnFixedUpdate(f32 in_dt)
{
    m_cpu.FetchDecodeExecute(m_state);
    m_renderer.Draw(m_state);
}
