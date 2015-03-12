#ifndef _UIROMPICKER_H_
#define _UIROMPICKER_H_

#include <ChilliSource/ChilliSource.h>

#include <ChilliSource/Core/Delegate.h>

#include <functional>
#include <vector>

class UIRomPicker
{
public:

	//----------------------------------------------------------------
	/// Display the picker UI allowing the user to pick a chip 8
	/// rom.
	///
	/// @author S Downie
	///
	/// @param in_canvas - Canvas to show picker on
	/// @param in_selectDelegate - Function called when rom is picked.
	//----------------------------------------------------------------
	void Show(CSUI::Canvas* in_canvas, const std::function<void(const std::string&)>& in_selectDelegate);

private:

	std::vector<CSCore::EventConnectionUPtr> m_pickerItemConnections;
	CSUI::Canvas* m_canvas;
};

#endif