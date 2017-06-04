#include "rigid_body.hpp"
#include <game_object.hpp>
namespace Aurora {
	RigidBody::RigidBody(btRigidBody* rigidBody) : mRigidBody(rigidBody) {
		mType = ComponentType::Physics;
	}

	void RigidBody::update(double dt) {
		btTransform tf;
		mRigidBody->getMotionState()->getWorldTransform(tf);
		auto& rot = tf.getRotation();
		Quaternion q(rot.x(), rot.y(), rot.z(), rot.w());
		auto vec = glm::eulerAngles(q);
		//mOwner->transform.set_rotation(q);
		auto& trans = tf.getOrigin();
		mOwner->transform.set_translation(trans.x(), trans.y(), trans.z());
	}
}