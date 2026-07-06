#pragma once

#include "cocos2d.h"

namespace LocalPvPResolution
{
	static const float kNormalDesignWidth = 480.0f;
	static const float kNormalDesignHeight = 320.0f;
	static const float kNormalFrameWidth = 960.0f;
	static const float kNormalFrameHeight = 640.0f;

	static const float kPvPDesignWidth = 960.0f;
	static const float kPvPDesignHeight = 320.0f;
	static const float kPvPFrameWidth = 1920.0f;
	static const float kPvPFrameHeight = 640.0f;
	static const float kPvPSingleViewWidth = 480.0f;
	static const float kPvPSingleViewHeight = 320.0f;

	void applyNormal();
	void applyLocalPvP();
}
