#pragma once

#include "cocos2d.h"

namespace LocalPvPResolution
{
	// Selected window size (set before engine init)
	extern bool g_selectedSmallSize;

	// Normal mode
	float getNormalFrameWidth();
	float getNormalFrameHeight();
	float getNormalDesignWidth();
	float getNormalDesignHeight();

	// PvP mode (width doubles from normal, height same)
	float getPvPFrameWidth();
	float getPvPFrameHeight();
	float getPvPDesignWidth();
	float getPvPDesignHeight();
	float getPvPSingleViewWidth();
	float getPvPSingleViewHeight();

	void applyNormal();
	void applyLocalPvP();
}
