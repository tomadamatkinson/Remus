#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace remus
{
template <typename T>
class Sender;

template <typename T>
class Receiver;

template <typename T>
class Channel
{
  public:
	friend class Sender<T>;
	friend class Receiver<T>;

	Channel()  = default;
	~Channel() = default;

	Channel(const Channel &)            = delete;
	Channel(Channel &&)                 = delete;
	Channel &operator=(const Channel &) = delete;
	Channel &operator=(Channel &&)      = delete;

	Receiver<T> *receiver();
	Sender<T>   *sender();

  private:
	void send(const T &event) const;

	mutable std::shared_mutex                 mutex;
	std::vector<std::unique_ptr<Receiver<T>>> receivers;
	std::vector<std::unique_ptr<Sender<T>>>   senders;
};

template <typename T>
class Sender
{
  public:
	friend class Channel<T>;

	~Sender() = default;

	// send an event
	void send(const T &event) const
	{
		channel->send(event);
	}

  private:
	Sender(Channel<T> *channel) :
	    channel(channel)
	{
	}

	Channel<T> *channel;
};

template <typename T>
class Receiver
{
  public:
	friend class Channel<T>;

  public:
	~Receiver() = default;

	// receive the last event
	bool next(T *event)
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (events.empty())
		{
			return false;
		}
		if (event)
		{
			*event = events.front();
		}
		events.pop_front();
		return true;
	}

	// drain all events receive the last one
	bool drain(T *event)
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (events.empty())
		{
			return false;
		}
		if (event)
		{
			*event = events.back();
		}
		events.clear();
		return true;
	}

  private:
	Receiver() = default;

	void receive(T event)
	{
		std::lock_guard<std::mutex> lock(mutex);
		events.push_back(event);
	}

	mutable std::mutex mutex;
	std::deque<T>      events;
};

}        // namespace remus

namespace remus
{
template <typename T>
Receiver<T> *Channel<T>::receiver()
{
	std::unique_lock<std::shared_mutex> lock(mutex);

	auto receiver = std::unique_ptr<Receiver<T>>(new Receiver<T>());
	receivers.push_back(std::move(receiver));
	return receivers.back().get();
}

template <typename T>
Sender<T> *Channel<T>::sender()
{
	std::unique_lock<std::shared_mutex> lock(mutex);

	auto sender = std::unique_ptr<Sender<T>>(new Sender<T>(this));
	senders.push_back(std::move(sender));
	return senders.back().get();
}

template <typename T>
void Channel<T>::send(const T &event) const
{
	std::shared_lock<std::shared_mutex> lock(mutex);

	for (auto &receiver : receivers)
	{
		receiver->receive(event);
	}
}
}        // namespace remus
