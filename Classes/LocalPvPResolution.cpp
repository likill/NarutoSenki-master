#include "LocalPvPResolution.h"
#include "CCEGLView.h"

USING_NS_CC;

namespace LocalPvPResolution
{
	bool g_selectedSmallSize = false;

	static void apply(float frameWidth,float frameHeight,float designWidth,float designHeight)
	{
		CCEGLView* view=CCEGLView::sharedOpenGLView();
		view->setFrameSize(frameWidth,frameHeight);
		// 480x320: use ShowAll so full design area is visible
		// 960x640: use FixedHeight (original behavior)
		ResolutionPolicy policy = g_selectedSmallSize ? kResolutionShowAll : kResolutionFixedHeight;
		view->setDesignResolutionSize(designWidth,designHeight,policy);
	}

	// Normal mode (design resolution same as original: 480x320)
	float getNormalFrameWidth()  { return g_selectedSmallSize ? 720.0f : 960.0f; }
	float getNormalFrameHeight() { return g_selectedSmallSize ? 480.0f : 640.0f; }
	float getNormalDesignWidth() { return 480.0f; }
	float getNormalDesignHeight(){ return 320.0f; }

	// PvP mode (width doubles, height same)
	float getPvPFrameWidth()     { return getNormalFrameWidth() * 2; }
	float getPvPFrameHeight()    { return getNormalFrameHeight(); }
	float getPvPDesignWidth()    { return 960.0f; }
	float getPvPDesignHeight()   { return 320.0f; }
	float getPvPSingleViewWidth(){ return 480.0f; }
	float getPvPSingleViewHeight(){ return 320.0f; }

	void applyNormal()
	{
		apply(getNormalFrameWidth(),getNormalFrameHeight(),getNormalDesignWidth(),getNormalDesignHeight());
	}

	void applyLocalPvP()
	{
		apply(getPvPFrameWidth(),getPvPFrameHeight(),getPvPDesignWidth(),getPvPDesignHeight());
	}
}
