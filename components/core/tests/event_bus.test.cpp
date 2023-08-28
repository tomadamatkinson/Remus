#include <events/event_bus.hpp>

#include <catch2/catch_test_macros.hpp>

struct Data
{
	int i = 0;
};

class Handler : public remus::EventHandler<Data>
{
  public:
	Handler()  = default;
	~Handler() = default;

	void handle(Data event) override
	{
		i = event.i;
	}

	int i = 0;
};

TEST_CASE("Bind an event handler", "[core]")
{
	remus::EventBus event_bus;

	Handler handler;

	event_bus.enable(handler);
}
