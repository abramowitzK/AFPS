#include "game_object.hpp"
#include <rigid_body.hpp>
#include <script_manager.hpp>
namespace CBlocks {
	GameObject::GameObject() : parent(nullptr) {}

	GameObject::GameObject(GameObject * parent) : parent(parent){
	}

	void GameObject::expose_to_script() {
		auto m = ScriptManager::instance();
		auto l = m->get_lua_state();
		sol::usertype<GameObject> type{
			"add_component", &GameObject::add_component,
			"parent", sol::property(&GameObject::parent, &GameObject::parent),
			"transform", sol::property(&GameObject::transform, &GameObject::transform),
			"id", sol::property(&GameObject::mId, &GameObject::mId),
			"components", sol::property(&GameObject::mComponents, &GameObject::mComponents),
			"children", sol::property(&GameObject::mChildren, &GameObject::mChildren)
		};
		l->set_usertype("GameObject", type);
	}

	GameObject::~GameObject() {
	}
	void GameObject::update(double dt) {
		for (const auto comp : mComponents) {
			if (comp->mType == ComponentType::Script) {
				((Script*)comp)->update(dt);
			}
			if (comp->mType == ComponentType::Physics) {
				((RigidBody*)comp)->update(dt);
			}
		}
		for (const auto& go : mChildren) {
			go->update(dt);
		}
	}
	void GameObject::render(Renderer* renderer) {
		for (const auto comp : mComponents) {
			if (comp->mType == ComponentType::Mesh) {
				renderer->render_mesh((MeshRenderer*)comp);
			}
			if (comp->mType == ComponentType::Script) {
				static_cast<Script*>(comp)->render(renderer);
			}
		}
		for (const auto& go : mChildren) {
			go->render(renderer);
		}
	}
	void GameObject::add_component(Component* comp) {
		mComponents.push_back(comp);
	}
	void GameObject::init() {
		for (const auto comp : mComponents) {
			if (comp->mType == ComponentType::Script) {
				static_cast<Script*>(comp)->init();
			}
		}
		for (const auto& go : mChildren) {
			go->init();
		}
	}
}