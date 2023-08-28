#pragma once

#include <functional>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace remus
{
class EventBus;

// the most basic event handler
class EventBusObserver
{
  public:
	virtual ~EventBusObserver()        = default;
	virtual void enable(EventBus &bus) = 0;
};

// a typed event handler
template <typename T>
class TypedEventHandler : public EventBusObserver
{
  public:
	friend class EventBus;

	virtual ~TypedEventHandler() = default;
	virtual void handle(T event) = 0;
};

// a base event handler
class EventHandlerBase : public EventBusObserver
{
  public:
	EventHandlerBase()  = default;
	~EventHandlerBase() = default;

	EventHandlerBase(const EventHandlerBase &)            = delete;
	EventHandlerBase(EventHandlerBase &&)                 = delete;
	EventHandlerBase &operator=(const EventHandlerBase &) = delete;
	EventHandlerBase &operator=(EventHandlerBase &&)      = delete;

	virtual void enable(EventBus &bus) override
	{}
};

// a typed event handler
template <typename... T>
class EventHandler : public EventHandlerBase, public TypedEventHandler<T>...
{
  public:
	friend class EventBus;

	EventHandler() = default;

	~EventHandler();

	void enable(EventBus &bus);

  private:
	EventBus *bus{nullptr};
};

// a simple event bus
class EventBus
{
  public:
	EventBus()  = default;
	~EventBus() = default;

	EventBus(const EventBus &)            = delete;
	EventBus(EventBus &&)                 = delete;
	EventBus &operator=(const EventBus &) = delete;
	EventBus &operator=(EventBus &&)      = delete;

  public:
	template <typename... T>
	void enable(EventHandler<T...> &handler)
	{
		handler.enable(*this);
	}

	template <typename... T>
	void enable(EventHandler<T...> *handler)
	{
		handler->enable(*this);
	}

	template <typename T>
	void bind(TypedEventHandler<T> *handler)
	{
		std::lock_guard<std::mutex> lock(mutex);

		handlers[std::type_index(typeid(T))][handler] = [handler](void *event) {
			handler->handle(*static_cast<T *>(event));
		};
	}

	template <typename T>
	void unbind(TypedEventHandler<T> *handler)
	{
		std::lock_guard<std::mutex> lock(mutex);

		handlers[std::type_index(typeid(T))].erase(handler);
	}

  private:
	std::mutex mutex;

	std::unordered_map<std::type_index, std::unordered_map<EventBusObserver *, std::function<void(void *)>>> handlers;
};
}        // namespace remus

namespace remus
{
template <typename... T>
EventHandler<T...>::~EventHandler()
{
	if (bus)
	{
		(bus->unbind<T>(this), ...);
	}
}

template <typename... T>
void EventHandler<T...>::enable(EventBus &bus)
{
	(bus.bind<T>(this), ...);
	this->bus = &bus;
}
}        // namespace remus
