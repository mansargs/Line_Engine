#include <memory>
#include "../hdrs/Engine.hpp"

lge::Engine::Engine() : desktop(std::make_unique<Desktop>()), mapData(std::make_unique<Map>()) {}

lge::Engine& lge::Engine::getInstance()
{
	static lge::Engine instance;
	return instance;
}

lge::Desktop& lge::Engine::getDesktop() const
{
	return *getInstance().desktop;
}

const lge::Map& lge::Engine::getMap() const
{
	return *getInstance().mapData;
}

