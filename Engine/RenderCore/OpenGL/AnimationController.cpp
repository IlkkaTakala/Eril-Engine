#include "AnimationController.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SkeletalMesh.h>

AnimationController::AnimationController()
{
	temp_anim = nullptr;
	skeleton = nullptr;
}

void AnimationController::Tick(float delta)
{
}

void AnimationController::UpdateBoneTransforms(float delta, RenderMesh* mesh) const
{
	auto m = dynamic_cast<RenderMeshSkeletalGL*>(mesh);
	if (!m) return;

	auto mats = m->GetBoneMatrices();

	for (int i = 0; i < mats.size(); i++) {
		Vector loc = temp_anim->GetLocation(i, delta);
		Rotator rot = temp_anim->GetRotation(i, delta);
		Vector sca = temp_anim->GetScale(i, delta);

		mats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
			* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
			* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));
	}

	m->UpdateBoneMatrices();
}
