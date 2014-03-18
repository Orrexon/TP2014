#include <Thor/Shapes/ConcaveShape.hpp>
#include <Thor/Shapes/Shapes.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Thor/Math/Random.hpp"
#include "Thor/Input.hpp"
#include <string>
#include "Thor/Input/ActionMap.hpp"
#include <iostream>
#include "Thor/Math/Trigonometry.hpp"
#include "Box2D/Box2D.h"


b2Vec2 gameToPhysicsUnits(sf::Vector2f p_unit)
{
	//heje
	return b2Vec2(p_unit.x / 32.f, p_unit.y / 32.f);
}
b2Vec2 gameToPhysicsUnits(sf::Vector2i p_unit)
{
	return b2Vec2(static_cast<float>(p_unit.x) / 32.f, static_cast<float>(p_unit.y) / 32.f);
}

sf::Vector2f physicsToGameUnits(float p_x, float p_y)
{
	return sf::Vector2f(p_x * 32.f, p_y * 32.f);
}

void createBox(b2World* p_world, sf::Vector2f p_position, sf::Vector2f p_size)
{
	// The body
	b2BodyDef bodyDef;
	bodyDef.position = gameToPhysicsUnits(p_position);
	bodyDef.type = b2_dynamicBody;
	b2Body* body = p_world->CreateBody(&bodyDef);

	// The shape
	b2PolygonShape shape;
	b2Vec2 unit_size = gameToPhysicsUnits(sf::Vector2f(p_size.x / 2, p_size.y / 2));
	shape.SetAsBox(unit_size.x, unit_size.y);

	// The fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.f;
	fixtureDef.shape = &shape;
	fixtureDef.friction = 0.1f;
	//fixtureDef.restitution = 1;
	body->CreateFixture(&fixtureDef);
}

void createEdge(b2World* p_world, sf::Vector2f p_vec1, sf::Vector2f p_vec2)
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
	body->CreateFixture(&fixtureDef);
}

void createMixer(b2World* p_world, sf::Vector2f p_vec1, sf::Vector2f p_vec2)
{
	// The body
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(100, 100);
	bodyDef.type = b2_kinematicBody;
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
	body->CreateFixture(&fixtureDef);
	body->SetAngularVelocity(10);
}


int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << "Argument " << i << ": " << argv[i] << std::endl;
	}

	// Create render window
	sf::RenderWindow window(sf::VideoMode(800, 600), "Doodlemeat", sf::Style::Default);
	window.setVerticalSyncEnabled(true);

	// Physics world
	b2Vec2 gravity(0.0f, 9.8f);
	b2World* world = new b2World(gravity);
	world->SetAllowSleeping(true); // Allow Box2D to exclude resting bodies from simulation
	
	sf::Texture box;
	sf::Texture ground;
	box.loadFromFile("../assets/png/box_32.png");
	box.setRepeated(true);

	thor::ActionMap<std::string> actionMap;
	actionMap["walk_up"] = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	actionMap["walk_down"] = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	actionMap["walk_left"] = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	actionMap["walk_right"] = thor::Action(sf::Keyboard::D, thor::Action::Hold);
	actionMap["add_ball"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
	actionMap["add_mixer"] = thor::Action(sf::Mouse::Right, thor::Action::PressOnce);

	createEdge(world, sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)), sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)));
	createEdge(world, sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)), sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)));
	createEdge(world, sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)), sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)));
	createEdge(world, sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)), sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)));
	createEdge(world, sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)), sf::Vector2f(thor::random(0.f, 800.f), thor::random(0.f, 600.f)));

	// Main loop
	while (window.isOpen())
	{
		// Let the action map handle the events instead of the original SFML events loop
		actionMap.update(window);

		if (actionMap.isActive("add_ball"))
		{
			sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
			sf::Vector2f float_mouse_position;
			float_mouse_position.x = mouse_position.x;
			float_mouse_position.y = mouse_position.y;
			createBox(world, float_mouse_position, sf::Vector2f(32, 32));
		}

		if (actionMap.isActive("add_mixer"))
		{
			sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
			sf::Vector2f float_mouse_position;
			float_mouse_position.x = mouse_position.x;
			float_mouse_position.y = mouse_position.y;
			createMixer(world, float_mouse_position, sf::Vector2f(400, 300));
		}

		world->Step(1 / 60.f, 8, 3);

		window.clear(sf::Color::Black);
		for (b2Body* bodyIt = world->GetBodyList(); bodyIt != 0; bodyIt = bodyIt->GetNext())
		{
			if (bodyIt->GetType() == b2_dynamicBody)
			{
				sf::Sprite sprite;
				sprite.setTexture(box);
				sprite.setPosition(physicsToGameUnits(bodyIt->GetPosition().x, bodyIt->GetPosition().y));
				sprite.setOrigin(16.f, 16.f);
				sprite.setRotation(thor::toDegree(bodyIt->GetAngle()));
				window.draw(sprite);
			}
			else if (bodyIt->GetType() == b2_staticBody)
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
			else
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
					line[0].color = sf::Color::Yellow;
					line[1].color = sf::Color::Yellow;

					window.draw(line, 2, sf::Lines);
				}
			}
		}
		window.display();
	}
}