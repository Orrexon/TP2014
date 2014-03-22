#include "SFML/Graphics.hpp"

#include "Thor/Shapes/ConcaveShape.hpp"
#include "Thor/Shapes/Shapes.hpp"
#include "Thor/Math/Random.hpp"
#include "Thor/Input.hpp"
#include "Thor/Input/ActionMap.hpp"
#include "Thor/Math/Trigonometry.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#include "Box2D/Box2D.h"
#include "manymouse.h"

b2Vec2 gameToPhysicsUnits(sf::Vector2f p_unit)
{
	return b2Vec2(p_unit.x / 32.f, p_unit.y / 32.f);
}
b2Vec2 gameToPhysicsUnits(sf::Vector2i p_unit)
{
	return b2Vec2(static_cast<float>(p_unit.x) / 32.f, static_cast<float>(p_unit.y) / 32.f);
}

float gameToPhysicsUnits(float p_unit)
{
	return p_unit / 32.f;
}

sf::Vector2f physicsToGameUnits(float p_x, float p_y)
{
	return sf::Vector2f(p_x * 32.f, p_y * 32.f);
}
sf::Vector2f physicsToGameUnits(b2Vec2 p_position)
{
	return sf::Vector2f(p_position.x * 32.f, p_position.y * 32.f);
}
void createWall(b2World* p_world, sf::Vector2f p_vec1, sf::Vector2f p_vec2)
{
	// The body
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(0, 0);
	bodyDef.type = b2_staticBody;
	bodyDef.angle = 0;
	b2Body* body = p_world->CreateBody(&bodyDef);

	// The shape
	b2EdgeShape shape;
	b2Vec2 v1 = gameToPhysicsUnits(p_vec1);
	b2Vec2 v2 = gameToPhysicsUnits(p_vec2);
	shape.Set(v1, v2);

	// The fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.shape = &shape;
	fixtureDef.friction = 1;
	fixtureDef.restitution = 1;
	body->CreateFixture(&fixtureDef);
}

struct Player
{
	Player()
	{
	}

	void update()
	{
		sf::Vector2f position = shape.getPosition();
		sf::FloatRect size = shape.getGlobalBounds();/*

		if (position.x < 0) shape.setPosition(0, position.y);
		if (position.x > 1920) shape.setPosition(1920, position.y);
		if (position.y < 0) shape.setPosition(position.x, 0);
		if (position.y > 1080) shape.setPosition(position.x, 1080 - size.height);*/
	}
	sf::CircleShape shape;
	b2Body* body;
};


int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << "Argument " << i << ": " << argv[i] << std::endl;
	}

	int numDevices = ManyMouse_Init();

	printf("Driver: %s\n", ManyMouse_DriverName());
	for (int i = 0; i < numDevices; i++)
	{
		std::cout << "Mouse " << i << ": " << ManyMouse_DeviceName(i) << std::endl;
	}

	// load textures
	sf::Texture cursor;
	cursor.loadFromFile("../cursor.png");


	// Physics world
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* world = new b2World(gravity);
	world->SetAllowSleeping(true); // Allow Box2D to exclude resting bodies from simulation

	createWall(world, sf::Vector2f(50, 50), sf::Vector2f(1870, 50));
	createWall(world, sf::Vector2f(1870, 50), sf::Vector2f(1870, 1030));
	createWall(world, sf::Vector2f(1870, 1030), sf::Vector2f(50, 1030));
	createWall(world, sf::Vector2f(50, 1030), sf::Vector2f(50, 50));

	// create player
	std::vector<Player*> players;
	for (int i = 0; i < numDevices; i++)
	{
		Player* player = new Player();
		player->shape.setFillColor(sf::Color::Green);
		player->shape.setRadius(30);
		player->shape.setOrigin(30, 30);
		player->shape.setPosition(sf::Vector2f(thor::random(300, 600), thor::random(300, 600)));

		// The body
		b2BodyDef bodyDef;
		bodyDef.position = gameToPhysicsUnits(player->shape.getPosition());
		bodyDef.type = b2_dynamicBody;
		bodyDef.linearDamping = 0.3;
		b2Body* body = world->CreateBody(&bodyDef);

		// The shape
		b2CircleShape shape;
		shape.m_radius = gameToPhysicsUnits(player->shape.getRadius());

		// The fixture
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1;
		fixtureDef.shape = &shape;
		fixtureDef.friction = 0.3;
		fixtureDef.restitution = 1;
		body->CreateFixture(&fixtureDef);
		player->body = body;
		players.push_back(player);
	}

	// Create render window
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Doodlemeat", sf::Style::None);
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);

	sf::Texture box;
	box.loadFromFile("../assets/png/box_32.png");
	box.setRepeated(true);

	thor::ActionMap<std::string> actionMap;

	while (window.isOpen())
	{
		actionMap.update(window);

		ManyMouseEvent event;
		while (ManyMouse_PollEvent(&event))
		{
			Player* player = players[event.device];

			if (event.type == MANYMOUSE_EVENT_RELMOTION)
			{
				sf::Vector2f playerPosition = player->shape.getPosition();
				if (event.item == 0)
				{
					player->body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(event.value), 0), player->body->GetWorldCenter(), true);
				}
				if (event.item == 1)
				{
					player->body->ApplyLinearImpulse(b2Vec2(0, gameToPhysicsUnits(event.value)), player->body->GetWorldCenter(), true);
				}
			}
		}
		world->Step(1 / 60.f, 8, 3);

		for (auto &player : players)
		{
			player->shape.setPosition(physicsToGameUnits(player->body->GetPosition()));
		}

		window.clear(sf::Color::Black);

		for (b2Body* bodyIt = world->GetBodyList(); bodyIt != 0; bodyIt = bodyIt->GetNext())
		{
			if (bodyIt->GetType() == b2_staticBody)
			{
				b2Shape* shape = bodyIt->GetFixtureList()[0].GetShape();
				if (shape->GetType() == b2Shape::e_edge)
				{
					b2EdgeShape* edge_shape = static_cast<b2EdgeShape*>(shape);
					sf::Vertex line[] =
					{
						sf::Vertex(physicsToGameUnits(edge_shape->m_vertex1.x, edge_shape->m_vertex1.y)),
						sf::Vertex(physicsToGameUnits(edge_shape->m_vertex2.x, edge_shape->m_vertex2.y))
					};
					window.draw(line, 2, sf::Lines);
				}
			}
		}

		for (auto &player : players)
		{
			window.draw(player->shape);
			for (int i = 0; i < player->shape.getPointCount(); i++)
			{
				sf::CircleShape circle;
				circle.setRadius(2);
				circle.setOrigin(2, 2);
				circle.setFillColor(sf::Color::Red);
				window.draw(circle);
			}
		}
		window.display();
	}
}