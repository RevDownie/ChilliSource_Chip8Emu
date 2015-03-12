#include "RomPickerState.h"

#include "Chip8State.h"

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/File.h>

//----------------------------------------------------------------
//----------------------------------------------------------------
void RomPickerState::OnInit()
{
	m_picker.Show(GetUICanvas(), [=](const std::string& in_romPath)
    {
        if (in_romPath.size() > 0)
        {
            auto fileSystem = CSCore::Application::Get()->GetFileSystem();
            auto fileStream = fileSystem->CreateFileStream(CSCore::StorageLocation::k_package, in_romPath, CSCore::FileMode::k_readBinary);
            CS_ASSERT(fileStream->IsOpen() == true && fileStream->IsBad() == false, "Cannot load ROM: " + in_romPath);
            
            CSCore::Application::Get()->GetStateManager()->Change(CSCore::StateSPtr(new Chip8State(std::move(fileStream))));
        }
    });
}


