#include "Observer.h"
#include "Timer.h"

void
Observable::AddObserver(ObserverPtr obs)
{
	mObservers.push_back(obs);
}

void 
Observable::Notify(TimerResultPtr result)
{
	for (auto observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->OnNotify(result);
	}

	RemoveDeadObservers();
}

void
Observable::Notify(Command* cmd)
{
	for (auto observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->OnNotify(cmd);
	}

	RemoveDeadObservers();
}

void
Observable::RemoveDeadObservers()
{
	std::vector<ObserverList::iterator> observersToRemove;
	for (auto obsIter = mObservers.begin(); obsIter != mObservers.end(); ++obsIter)
	{
		if (obsIter->expired())
			observersToRemove.push_back(obsIter);
	}

	for (auto obs : observersToRemove)
		mObservers.erase(obs);
}