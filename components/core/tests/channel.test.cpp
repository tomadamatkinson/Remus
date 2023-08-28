#include <events/channel.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Send an event", "[core]")
{
	remus::Channel<int> channel;

	auto receiver = channel.receiver();
	auto sender   = channel.sender();

	int i = 0;

	sender->send(i);

	int received;
	REQUIRE(receiver->next(&received));
	REQUIRE(received == i);
}

TEST_CASE("Send multiple events", "[core]")
{
	remus::Channel<int> channel;

	auto receiver = channel.receiver();
	auto sender   = channel.sender();

	sender->send(0);
	sender->send(1);
	sender->send(2);

	// received in order
	int received;
	REQUIRE(receiver->next(&received));
	REQUIRE(received == 0);
	REQUIRE(receiver->next(&received));
	REQUIRE(received == 1);
	REQUIRE(receiver->next(&received));
	REQUIRE(received == 2);
	REQUIRE(receiver->next(&received) == false);        // no more events
}

TEST_CASE("Drain multiple events", "[core]")
{
	remus::Channel<int> channel;

	auto receiver = channel.receiver();
	auto sender   = channel.sender();

	for (int i = 0; i < 100; ++i)
	{
		sender->send(i);
	}

	int received;
	REQUIRE(receiver->drain(&received));
	REQUIRE(received == 99);
	REQUIRE(receiver->drain(&received) == false);        // no more events
}

TEST_CASE("Multiple receivers", "[core]")
{
	remus::Channel<int> channel;

	auto receiver1 = channel.receiver();
	auto receiver2 = channel.receiver();
	auto sender    = channel.sender();

	sender->send(0);

	int received;
	REQUIRE(receiver1->next(&received));
	REQUIRE(received == 0);
	REQUIRE(receiver2->next(&received));
	REQUIRE(received == 0);
}

TEST_CASE("Multiple senders", "[core]")
{
	remus::Channel<int> channel;

	auto receiver = channel.receiver();
	auto sender1  = channel.sender();
	auto sender2  = channel.sender();

	sender1->send(0);
	sender2->send(1);

	int received;
	REQUIRE(receiver->next(&received));
	REQUIRE(received == 0);
	REQUIRE(receiver->next(&received));
	REQUIRE(received == 1);
}

TEST_CASE("Out of order receivers", "[core]")
{
	remus::Channel<int> channel;

	auto receiver1 = channel.receiver();
	auto sender    = channel.sender();

	sender->send(0);

	// receiver2 is created after the event is sent
	auto receiver2 = channel.receiver();

	int received;
	REQUIRE(receiver1->next(&received));
	REQUIRE(received == 0);
	REQUIRE(receiver2->next(&received) == false);        // event is not received
}
