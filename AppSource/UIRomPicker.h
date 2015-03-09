#ifndef _UIROMPICKER_H_
#define _UIROMPICKER_H_

#include <functional>

class UIRomPicker
{
public:

	//----------------------------------------------------------------
	/// Display the picker UI allowing the user to pick a chip 8
	/// rom.
	///
	/// @author S Downie
	///
	/// in_selectDelegate - Function called when rom is picked.
	//----------------------------------------------------------------
	void Show(const std::function<void(const std::string&)>& in_selectDelegate);
};

#endif