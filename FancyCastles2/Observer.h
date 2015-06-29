#ifndef METZE_OBSERVER_H
#define METZE_OBSERVER_H

#include <vector>

class Observer
{
public:
	virtual ~Observer() {}
	virtual void OnNotify() = 0;
};


class Subject
{
public:
	void AddObserver(Observer* obs)
	{ 
		mObservers.push_back(obs); 
	}

	void RemoveObserver(Observer* obs)
	{
		auto found = std::find(mObservers.begin(), mObservers.end(), obs);
		if (found != mObservers.end())
			mObservers.erase(found);
	}

private:
	std::vector<Observer*> mObservers;

protected:
	void Notify()
	{
		for (const auto& observer : mObservers)
		{
			observer->OnNotify();
		}
	}
};

#endif