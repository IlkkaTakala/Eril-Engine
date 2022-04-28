#include "AnimationController.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SkeletalMesh.h>

AnimationController::AnimationController()
{
	temp_anim = nullptr;
	animtime = 0.f;
}

void AnimationController::Tick(float delta)
{
}

void AnimationController::SetSkeleton(RenderMesh* s)
{
}

void AnimationController::UpdateBoneTransforms(float delta, RenderMesh* mesh)
{
	if (!temp_anim) return;
	auto m = dynamic_cast<RenderMeshSkeletalGL*>(mesh);
	if (!m) return;

	animtime += delta;
	auto& mats = m->GetBoneMatrices();

	for (int i = 1; i < mats.size(); i++) {
		Vector loc = temp_anim->GetLocation(i, animtime);
		Rotator rot = temp_anim->GetRotation(i, animtime);
		Vector sca = temp_anim->GetScale(i, animtime);

		mats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
			* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
			* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));
	}

	m->UpdateBoneMatrices();
}
