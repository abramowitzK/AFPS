#pragma once
#include <scene.hpp>
#include <vector>
namespace Aurora {
	/*
	The game should encapsulate its own managers for game specific stuff. There might be managers that the engine itself
	has? It might be better to just have the engine provide some basic stuff to the game and have everything stored in game.
	*/
	struct Game {
		static void expose_to_script(ScriptManager* m);
		void load(const std::string& initialScene, EventManager* manager);
		Scene* current_scene;
		std::vector<Scene> scenes;
	};
	void update_game(Game* game, double dt);
	void render_game(Game* game, Renderer* renderer);
}
