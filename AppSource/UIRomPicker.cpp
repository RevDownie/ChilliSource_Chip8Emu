#include "UIRomPicker.h"

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/File.h>
#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/Core/Math/Random.h>
#include <ChilliSource/UI/Base.h>

//----------------------------------------------------------------
//----------------------------------------------------------------
void UIRomPicker::Show(CSUI::Canvas* in_canvas, const std::function<void(const std::string&)>& in_selectDelegate)
{
	m_canvas = in_canvas;

	auto fileSystem = CSCore::Application::Get()->GetFileSystem();
	auto romPaths = fileSystem->GetFilePathsWithExtension(CSCore::StorageLocation::k_package, "Roms", false, "c8");

	auto resourcePool = CSCore::Application::Get()->GetResourcePool();
	auto widgetFactory = CSCore::Application::Get()->GetWidgetFactory();
	
	auto pickerDesc = resourcePool->LoadResource<CSUI::WidgetTemplate>(CSCore::StorageLocation::k_package, "UI/RomPicker.csui");
	auto pickerItemDesc = resourcePool->LoadResource<CSUI::WidgetTemplate>(CSCore::StorageLocation::k_package, "UI/RomPickerItem.csui");

	CSUI::WidgetSPtr picker = widgetFactory->Create(pickerDesc);

	for (const auto& path : romPaths)
	{
		CSUI::WidgetSPtr pickerItem = widgetFactory->Create(pickerItemDesc);
		picker->AddWidget(pickerItem);

		auto connection = pickerItem->GetWidget("Button")->GetReleasedInsideEvent().OpenConnection([=](CSUI::Widget* in_widget, const CSInput::Pointer& in_pointer, CSInput::Pointer::InputType in_type)
		{
			if (in_type == CSInput::Pointer::GetDefaultInputType())
			{
				picker->RemoveFromParent();
				in_selectDelegate("Roms/" + path);
			}
		});
		m_pickerItemConnections.push_back(std::move(connection));
	}

	m_canvas->AddWidget(picker);
}