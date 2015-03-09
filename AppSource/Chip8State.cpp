#include "Chip8State.h"

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/File.h>
#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/UI/Base.h>
#include <ChilliSource/UI/Button.h>

#include "RomPickerState.h"

//----------------------------------------------------------------
//----------------------------------------------------------------
Chip8State::Chip8State(CSCore::FileStreamUPtr in_romStream)
: m_romStream(std::move(in_romStream))
{
    
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::OnInit()
{
    CSCore::Application::Get()->SetUpdateInterval(1.0f/(f32)Chip8Constants::k_framesPerSecond);
    
    LoadInGameHud();

    m_renderer.Build(GetScene());
    LoadROM();
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::LoadInGameHud()
{
    auto resourcePool = CSCore::Application::Get()->GetResourcePool();
    
    auto hudDesc = resourcePool->LoadResource<CSUI::WidgetTemplate>(CSCore::StorageLocation::k_package, "UI/InGame.csui");
    
    auto widgetFactory = CSCore::Application::Get()->GetWidgetFactory();
    CSUI::WidgetSPtr hud = widgetFactory->Create(hudDesc);
    
    m_pauseButtonConnection = hud->GetWidget("PauseButton")->GetReleasedInsideEvent().OpenConnection([=](CSUI::Widget* in_widget, const CSInput::Pointer& in_pointer, CSInput::Pointer::InputType in_type)
    {
        if(in_type == CSInput::Pointer::GetDefaultInputType())
        {
            m_paused = !m_paused;
        }
    });
    
    m_resetButtonConnection = hud->GetWidget("ResetButton")->GetReleasedInsideEvent().OpenConnection([=](CSUI::Widget* in_widget, const CSInput::Pointer& in_pointer, CSInput::Pointer::InputType in_type)
    {
       if(in_type == CSInput::Pointer::GetDefaultInputType())
       {
           m_reset = true;
       }
    });
    
    GetUICanvas()->AddWidget(hud);
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void Chip8State::LoadROM()
{
    Reset();

    std::string rom;
    m_romStream->GetAll(rom);
    m_romStream->Close();
    m_romStream.reset();
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
    if(m_paused == false)
    {
        m_keyboard.UpdateKeyStates(m_state);
        m_cpu.FetchDecodeExecute(m_state);
        m_renderer.Draw(m_state);
    }
    
    if(m_reset == true)
    {
        m_reset = false;
        m_paused = true;
        CSCore::Application::Get()->GetStateManager()->Change(CSCore::StateSPtr(new RomPickerState()));
    }
}
