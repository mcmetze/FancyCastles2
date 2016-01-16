#include "Timer.h"

TimerObject::TimerObject(int timerID)
	: GameObject(timerID, -1), mTimerID(timerID), mTimerState(), mTimeoutSec(3.0)
{
}

GameObjectType
TimerObject::GetObjectType() const
{
	return GameObjectType::TIMER;
}

void
TimerObject::OnTimerStart(TimerResultPtr result)
{
	mTimerState.mIsBusy = true;
	mTimerState.mStartTime = std::clock();
	mTimerState.mResult = result;
}

void
TimerObject::OnTimerFinished()
{
	mTimerState.mIsBusy = false;
	Notify(mTimerState.mResult);
}

void
TimerObject::Tick()
{
	if (mTimerState.mIsBusy)
	{
		const auto duration = (std::clock() - mTimerState.mStartTime) / (double(CLOCKS_PER_SEC));
		if (duration - mTimeoutSec > DBL_EPSILON)
		{
			OnTimerFinished();
		}
	}
}

bool
TimerObject::IsBusy() const
{
	return mTimerState.mIsBusy; 
}

int
TimerObject::GetObjectID() const
{
	return mTimerID;
}

void
TimerObject::Cancel()
{
	mTimerState.mIsBusy = false;
}