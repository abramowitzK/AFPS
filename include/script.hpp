#pragma once
#include <sol.hpp>
#include <component.hpp>
#include <rendering.hpp>
namespace Aurora {
	struct Script : public Component {
		Script(const std::string& tableName, std::string script, ScriptManager* m);
		~Script() { printf("destructor called\n"); }
		void update(double dt);
		void render(Renderer* renderer) {
			int y = 0;
			//self.for_each([renderer, &y](sol::object key, sol::object value) { renderer->render_ttf(key.as<std::string>() + " " + std::to_string(value.as<float>()), 20, y += 30, 0.5, { 1.0,0.0,0.0,1.0 });});
		}
		sol::table self;
		sol::function mUpdate;
		std::string mScript;
		void set_value(float value) {
			self.set("value", value);
		}
		void init();
		bool valid = true;
		sol::state* global_state;
	};
}