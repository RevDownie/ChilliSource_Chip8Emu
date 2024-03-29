//
//  App.cpp
//  CSChip8Emulator
//  Created by Ian Copland on 15/10/2014.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include <App.h>

#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/UI/Base.h>

#include "RomPickerState.h"

CSCore::Application* CreateApplication()
{
    return new CSChip8Emulator::App();
}

namespace CSChip8Emulator
{
    void App::CreateSystems()
    {
        //Create systems here.
    }

    void App::OnInit()
    {
		auto resourcePool = CSCore::Application::Get()->GetResourcePool();
		auto widgetFactory = CSCore::Application::Get()->GetWidgetFactory();

		auto pickerDef = resourcePool->LoadResource<CSUI::WidgetDef>(CSCore::StorageLocation::k_package, "UI/Picker.csuidef");
		widgetFactory->RegisterDefinition(pickerDef);
    }

    void App::PushInitialState()
    {
        GetStateManager()->Push(CSCore::StateSPtr(new RomPickerState()));
    }

    void App::OnDestroy()
    {
        //destruction stuff here.
    }
}

