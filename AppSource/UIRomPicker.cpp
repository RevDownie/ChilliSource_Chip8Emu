#include "UIRomPicker.h"

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/File.h>
#include <ChilliSource/UI/Base.h>

//----------------------------------------------------------------
//----------------------------------------------------------------
void UIRomPicker::Show(const std::function<void(const std::string&)>& in_selectDelegate)
{
	auto fileSystem = CSCore::Application::Get()->GetFileSystem();
	auto romPaths = fileSystem->GetFilePathsWithExtension(CSCore::StorageLocation::k_package, "Roms", false, "c8");

	//auto widgetFactory = CSCore::Application::Get()->GetWidgetFactory();
	//auto button = widgetFactory->CreateHighlightButton();
	//button->SetRelativeSize(CSCore::Vector2(0.1f, 0.1f));

	if (romPaths.size() > 0)
	{
		in_selectDelegate(romPaths[0]);
	}
	else
	{
		in_selectDelegate("");
	}
}