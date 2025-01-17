#include <game_object.hpp>
#include <rigid_body.hpp>
#include <script_manager.hpp>
#include <transform.hpp>
#include <character_controller.hpp>
#include <camera_component.hpp>
namespace Aurora {
	GameObject::GameObject() : parent(nullptr) {}

	GameObject::GameObject(GameObject * parent) : parent(parent){
	}

	void GameObject::expose_to_script(ScriptManager* m) {
		auto l = m->get_lua_state();
		sol::usertype<GameObject> type{
			"add_component", &GameObject::add_component,
			"get_cc", &GameObject::get_component<CharacterController>,
			"parent", sol::property(&GameObject::parent, &GameObject::parent),
			"transform", sol::property(&GameObject::transform, &GameObject::transform),
			"id", sol::property(&GameObject::mId, &GameObject::mId),
			"components", sol::property(&GameObject::mComponents, &GameObject::mComponents),
			"children", sol::property(&GameObject::mChildren, &GameObject::mChildren)
		};
		l->set_usertype("GameObject", type);
	}

	GameObject::~GameObject() {
		for (const auto& child : mChildren) {
			delete child;
		}
	}
	void GameObject::update(double dt) {
		for (const auto comp : mComponents) {
			if (comp->mType == ComponentType::Script) {
				((Script*)comp)->update(dt);
			}
			if (comp->mType == ComponentType::Physics) {
				((RigidBody*)comp)->update(dt);
			}
			if (comp->mType == ComponentType::CharacterController) {
				((CharacterController*)comp)->update(dt);
			}
			if (comp->mType == ComponentType::Camera) {
				((CameraComponent*)comp)->update(dt);
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
			if (comp->mType == ComponentType::Camera) {
				renderer->render((CameraComponent*)comp);
			}

		}
		for (const auto& go : mChildren) {
			go->render(renderer);
		}
	}
	void GameObject::add_component(Component* comp) {
		comp->mOwner = this;
		if(!comp->mTransform)
			comp->mTransform = new Transform();
		comp->mTransform->mParent = &this->transform;
		mComponents.push_back(comp);
	}

	Component* GameObject::get_component_base(ComponentType type) {
	
		for (const auto comp : mComponents) {
			if (comp->mType == type) {
				return comp;
			}
		}
		return nullptr;
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