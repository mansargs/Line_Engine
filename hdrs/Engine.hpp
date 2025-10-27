#pragma once

#include <memory>
#include "Desktop.hpp"
#include "Map.hpp"

namespace lge
{
	class Engine
	{
		private:
			std::unique_ptr<Desktop> desktop;
			std::unique_ptr<Map> mapData;
			Engine();

			Engine(const Engine &) = delete;
			Engine(Engine &&) = delete;
			Engine &operator=(const Engine &) = delete;
			Engine &operator=(Engine &&) = delete;
		public:
			static Engine& getInstance();
			Desktop& getDesktop() const;
			const Map& getMap() const;
	};
}
