#include "LocalPvPResolution.h"
#include "CCEGLView.h"

USING_NS_CC;

namespace LocalPvPResolution
{
	static void apply(float frameWidth,float frameHeight,float designWidth,float designHeight)
	{
		CCEGLView* view=CCEGLView::sharedOpenGLView();
		view->setFrameSize(frameWidth,frameHeight);
		view->setDesignResolutionSize(designWidth,designHeight,kResolutionFixedHeight);
	}

	void applyNormal()
	{
		apply(kNormalFrameWidth,kNormalFrameHeight,kNormalDesignWidth,kNormalDesignHeight);
	}

	void applyLocalPvP()
	{
		apply(kPvPFrameWidth,kPvPFrameHeight,kPvPDesignWidth,kPvPDesignHeight);
	}
}
