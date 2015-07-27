#pragma once

#include <ctime>

#include "Observer.h"

class TileTimer : public Subject
{
public:
	TileTimer();

	void OnTimerStart();
	void OnTimerDone();
	void Tick();
	bool IsBusy() const;

private:
	struct TimerState
	{
		bool isBusy;
		time_t startTime;

		TimerState() : isBusy(false), startTime(std::clock()) { }
	};

	const double mTimeoutSec;
	TimerState mTimerState;

};