#include "AnimationController.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SkeletalMesh.h>

AnimationController::AnimationController()
{
	temp_anim = nullptr;
	animtime = 0.f;
	combiner = nullptr;
	animoverride = false;
}

void AnimationController::Tick(float delta)
{
	animtime += delta;
}

void AnimationController::SetSkeleton(RenderMesh* s)
{
}

void AnimationController::UpdateBoneTransforms(float delta, RenderMesh* mesh)
{
	auto m = dynamic_cast<RenderMeshSkeletalGL*>(mesh);
	if (!m) return;

	auto& mats = m->GetBoneMatrices();

	if (animoverride && temp_anim) {
		for (int i = 1; i < mats.size(); i++) {
			Vector loc = temp_anim->GetLocation(i, animtime);
			Rotator rot = temp_anim->GetRotation(i, animtime);
			Vector sca = temp_anim->GetScale(i, animtime);

			mats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
				* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
				* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));
		}
	}
	else if (combiner) {
		
		for (int i = 1; i < mats.size(); i++) {
			Vector loc;
			Rotator rot;
			Vector sca;
			combiner->Evaluate(delta, i, loc, rot, sca);

			mats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
				* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
				* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));
		}
	}
	else {
		return;
		for (int i = 1; i < mats.size(); i++) {
			mats[i] = glm::mat4(1.f);
		}
	}

	m->UpdateBoneMatrices();
}

AnimationStateMachine::AnimationStateMachine(const std::unordered_map<String, AnimationCombiner*>& combiners, const std::multimap<String, std::pair<String, PathFunc>>& paths)
{

	std::unordered_map<String, int> ids;
	int begin = 0;
	for (auto& [name, com] : combiners) {
		Coms.push_back(com);
		ids.emplace(name, begin++);
	}
	for (auto& [name, pair] : paths) {
		const String& from = name;
		const String& to = pair.first;

		if (auto it = ids.find(from); it != ids.end()) {
			if (auto path = ids.find(to); path != ids.end()) {
				Paths.emplace(it->second, std::pair<int, PathFunc>{ path->second, pair.second });
			}
		}
	}

	currentState = 0;
}

AnimationCombiner* AnimationStateMachine::Evaluate()
{
	auto it = Paths.equal_range(currentState);
	while (it.first != it.second) {
		if (it.first->second.second()) {
			currentState = it.first->second.first;
			return Coms[currentState];
		}
		it.first++;
	}
	return Coms[currentState];
}
