#pragma once
#ifndef	UIMANAGERCONFIG_H
#define UIMANAGERCONFIG_H

namespace BE
{
	/*------------- UI Element ---------------------*/
	using UIElementID = BEID;
	const UIElementID NULL_UIELEMENT = UIElementID(0);			//NULL UI Element
	constexpr int MAX_UI_COUNT = 32;

	/*-------------- UI Canvas --------------------*/
	using CanvasID = BEID;
	const CanvasID NULL_UICANVAS = CanvasID(0);				//NULL UI Canvas

}

#endif