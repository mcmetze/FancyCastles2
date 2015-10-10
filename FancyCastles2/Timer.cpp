#include "Timer.h"

TileTimer::TileTimer() : mTimerState(), mTimeoutSec(5.0) { }


void
TileTimer::OnTimerStart()
{
	mTimerState.isBusy = true;
	mTimerState.startTime = std::clock();
}

void
TileTimer::OnTimerDone()
{
	mTimerState.isBusy = false;
	Notify();
}

void
TileTimer::Tick()
{
	if (mTimerState.isBusy)
	{
		const auto duration = (std::clock() - mTimerState.startTime) / (double(CLOCKS_PER_SEC));
		if (duration - mTimeoutSec > DBL_EPSILON)
		{
			OnTimerDone();
		}
	}
}

bool
TileTimer::IsBusy() const
{
	return mTimerState.isBusy; 
}