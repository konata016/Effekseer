﻿
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Instance.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.Model.h"
#include "Effekseer.Curve.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

template <typename T, typename U>
void Instance::ApplyEq(T& dstParam, Effect* e, InstanceGlobal* instg, IRandObject* rand, int dpInd, const U& originalParam)
{
	static_assert(sizeof(T) == sizeof(U), "size is not mismatched");
	const int count = sizeof(T) / 4;

	EFK_ASSERT(e != nullptr);
	EFK_ASSERT(0 <= dpInd && dpInd < static_cast<int>(instg->dynamicEqResults.size()));

	auto dst = reinterpret_cast<float*>(&(dstParam));
	auto src = reinterpret_cast<const float*>(&(originalParam));

	auto eqresult = instg->dynamicEqResults[dpInd];
	std::array<float, 1> globals;
	globals[0] = instg->GetUpdatedFrame() / 60.0f;

	std::array<float, 5> locals;

	for (int i = 0; i < count; i++)
	{
		locals[i] = src[i];
	}

	for (int i = count; i < 4; i++)
	{
		locals[i] = 0.0f;
	}

	locals[4] = m_pParent != nullptr ? m_pParent->m_LivingTime / 60.0f : 0.0f;

	auto e_ = static_cast<EffectImplemented*>(e);
	auto& dp = e_->dynamicEquation[dpInd];

	if (dp.GetRunningPhase() == InternalScript::RunningPhaseType::Local)
	{
		eqresult = dp.Execute(instg->dynamicInputParameters, globals, locals, RandCallback::Rand, RandCallback::RandSeed, rand);
	}

	for (int i = 0; i < count; i++)
	{
		dst[i] = eqresult[i];
	}
}

template <typename S>
Vec3f Instance::ApplyEq(
	Effect* e, InstanceGlobal* instg, IRandObject* rand, const int& dpInd, const Vec3f& originalParam, const S& scale, const S& scaleInv)
{
	Vec3f param = originalParam;
	if (dpInd >= 0)
	{
		param *= Vec3f(scaleInv[0], scaleInv[1], scaleInv[2]);

		ApplyEq(param, e, instg, rand, dpInd, param);

		param *= Vec3f(scale[0], scale[1], scale[2]);
	}
	return param;
}

random_float Instance::ApplyEq(Effect* e, InstanceGlobal* instg, IRandObject* rand, const RefMinMax& dpInd, random_float originalParam)
{
	if (dpInd.Max >= 0)
	{
		ApplyEq(originalParam.max, e, instg, rand, dpInd.Max, originalParam.max);
	}

	if (dpInd.Min >= 0)
	{
		ApplyEq(originalParam.min, e, instg, rand, dpInd.Min, originalParam.min);
	}

	return originalParam;
}

template <typename S>
random_vector3d Instance::ApplyEq(Effect* e,
								  InstanceGlobal* instg,
								  IRandObject* rand,
								  const RefMinMax& dpInd,
								  random_vector3d originalParam,
								  const S& scale,
								  const S& scaleInv)
{
	if (dpInd.Max >= 0)
	{
		originalParam.max.x *= scaleInv[0];
		originalParam.max.y *= scaleInv[1];
		originalParam.max.z *= scaleInv[2];

		ApplyEq(originalParam.max, e, instg, rand, dpInd.Max, originalParam.max);

		originalParam.max.x *= scale[0];
		originalParam.max.y *= scale[1];
		originalParam.max.z *= scale[2];
	}

	if (dpInd.Min >= 0)
	{
		originalParam.min.x *= scaleInv[0];
		originalParam.min.y *= scaleInv[1];
		originalParam.min.z *= scaleInv[2];

		ApplyEq(originalParam.min, e, instg, rand, dpInd.Min, originalParam.min);

		originalParam.min.x *= scale[0];
		originalParam.min.y *= scale[1];
		originalParam.min.z *= scale[2];
	}

	return originalParam;
}

random_int Instance::ApplyEq(Effect* e, InstanceGlobal* instg, IRandObject* rand, const RefMinMax& dpInd, random_int originalParam)
{
	if (dpInd.Max >= 0)
	{
		float value = static_cast<float>(originalParam.max);
		ApplyEq(value, e, instg, rand, dpInd.Max, value);
		originalParam.max = static_cast<int32_t>(value);
	}

	if (dpInd.Min >= 0)
	{
		float value = static_cast<float>(originalParam.min);
		ApplyEq(value, e, instg, rand, dpInd.Min, value);
		originalParam.min = static_cast<int32_t>(value);
	}

	return originalParam;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::Instance(Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer, InstanceGroup* pGroup)
	: m_pManager(pManager)
	, m_pEffectNode((EffectNodeImplemented*)pEffectNode)
	, m_pContainer(pContainer)
	, ownGroup_(pGroup)
	, childrenGroups_(nullptr)
	, m_pParent(nullptr)
	, m_State(INSTANCE_STATE_ACTIVE)
	, m_LivedTime(0)
	, m_LivingTime(0)
	, m_flexibleGeneratedChildrenCount(nullptr)
	, m_flexibleNextGenerationTime(nullptr)
	, m_GlobalMatrix43Calculated(false)
	, m_ParentMatrix43Calculated(false)
	, is_time_step_allowed(false)
	, m_sequenceNumber(0)
	, m_flipbookIndexAndNextRate(0)
	, m_AlphaThreshold(0.0f)
{
	m_generatedChildrenCount = m_fixedGeneratedChildrenCount;
	maxGenerationChildrenCount = fixedMaxGenerationChildrenCount_;
	m_nextGenerationTime = m_fixedNextGenerationTime;

	ColorInheritance = Color(255, 255, 255, 255);
	ColorParent = Color(255, 255, 255, 255);

	InstanceGroup* group = NULL;

	for (int i = 0; i < m_pEffectNode->GetChildrenCount(); i++)
	{
		InstanceContainer* childContainer = m_pContainer->GetChild(i);

		auto allocated = childContainer->CreateInstanceGroup();

		// Lack of memory
		if (allocated == nullptr)
		{
			break;
		}
	
		if (group != NULL)
		{
			group->NextUsedByInstance = allocated;
			group = allocated;
		}
		else
		{
			group = allocated;
			childrenGroups_ = group;
		}
	}

	for (auto& it : uvTimeOffsets)
	{
		it = 0;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::~Instance()
{
	assert(m_State != INSTANCE_STATE_ACTIVE);

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	if (m_flexibleGeneratedChildrenCount != nullptr)
	{
		m_pManager->GetFreeFunc()(m_flexibleGeneratedChildrenCount, sizeof(int32_t) * parameter->GetChildrenCount());
	}

	if (flexibleMaxGenerationChildrenCount_ != nullptr)
	{
		m_pManager->GetFreeFunc()(flexibleMaxGenerationChildrenCount_, sizeof(int32_t) * parameter->GetChildrenCount());
	}

	if (m_flexibleNextGenerationTime != nullptr)
	{
		m_pManager->GetFreeFunc()(m_flexibleNextGenerationTime, sizeof(float) * parameter->GetChildrenCount());
	}
}

void Instance::GenerateChildrenInRequired()
{
	if (m_State == INSTANCE_STATE_REMOVED)
	{
		return;
	}

	const float currentTime = m_LivingTime;

	auto effect = this->m_pEffectNode->m_effect;
	auto instanceGlobal = this->m_pContainer->GetRootInstance();
	auto& rand = m_randObject;

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	InstanceGroup* group = childrenGroups_;

	for (int32_t i = 0; i < parameter->GetChildrenCount(); i++, group = group->NextUsedByInstance)
	{
		auto node = (EffectNodeImplemented*)parameter->GetChild(i);

		// Lack of memory
		if (group == nullptr)
		{
			return;
		}

		while (true)
		{
			// GenerationTimeOffset can be minus value.
			// Minus frame particles is generated simultaniously at frame 0.
			if (maxGenerationChildrenCount[i] > m_generatedChildrenCount[i] && m_nextGenerationTime[i] <= currentTime)
			{
				// Create a particle
				auto newInstance = group->CreateInstance();
				if (newInstance != nullptr)
				{
					Mat43f rootMatrix = Mat43f::Identity;

					newInstance->Initialize(this, m_generatedChildrenCount[i], rootMatrix);
				}

				m_generatedChildrenCount[i]++;

				auto gt = ApplyEq(effect, instanceGlobal, &rand, node->CommonValues.RefEqGenerationTime, node->CommonValues.GenerationTime);
				m_nextGenerationTime[i] += Max(0.0f, gt.getValue(rand));
			}
			else
			{
				break;
			}
		}

		/*int32_t instanceNumberOffset = m_generatedChildrenCount[i];

		// GenerationTimeOffset can be minus value.
		// Minus frame particles is generated simultaniously at frame 0.
		while (maxGenerationChildrenCount[i] > m_generatedChildrenCount[i] &&
			m_nextGenerationTime[i] <= currentTime)
		{
			m_generatedChildrenCount[i]++;

			auto gt = ApplyEq(node->CommonValues.RefEqGenerationTime, node->CommonValues.GenerationTime);
			m_nextGenerationTime[i] += Max(0.0f, gt.getValue(rand));
		}

		int32_t generatingCount = m_generatedChildrenCount[i] - instanceNumberOffset;

		// Create instances
		group->CreateInstances(instanceNumberOffset, generatingCount, m_LivingTime, m_GlobalMatrix43);*/
	}
}

void Instance::UpdateChildrenGroupMatrix()
{
	for (InstanceGroup* group = childrenGroups_; group != nullptr; group = group->NextUsedByInstance)
	{
		group->SetParentMatrix(m_GlobalMatrix43);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGlobal* Instance::GetInstanceGlobal()
{
	return m_pContainer->GetRootInstance();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eInstanceState Instance::GetState() const
{
	return m_State;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const Mat43f& Instance::GetGlobalMatrix43() const
{
	return m_GlobalMatrix43;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Initialize(Instance* parent, int32_t instanceNumber, const Mat43f& globalMatrix)
{
	assert(this->m_pContainer != nullptr);

	// 状態の初期化
	m_State = INSTANCE_STATE_ACTIVE;

	// 親の設定
	m_pParent = parent;

	m_GlobalMatrix43 = globalMatrix;
	assert(m_GlobalMatrix43.IsValid());

	// 時間周りの初期化
	m_LivingTime = 0.0f;
	m_LivedTime = FLT_MAX;

	m_InstanceNumber = instanceNumber;

	m_IsFirstTime = true;

	auto instanceGlobal = this->m_pContainer->GetRootInstance();

	// Set random seed from InstanceGlobal's randomizer
	m_randObject.SetSeed(instanceGlobal->GetRandObject().GetRandInt());

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	// Extend array
	if (parameter->GetChildrenCount() >= ChildrenMax)
	{
		m_flexibleGeneratedChildrenCount = (int32_t*)(m_pManager->GetMallocFunc()(sizeof(int32_t) * parameter->GetChildrenCount()));
		flexibleMaxGenerationChildrenCount_ = (int32_t*)(m_pManager->GetMallocFunc()(sizeof(int32_t) * parameter->GetChildrenCount()));
		m_flexibleNextGenerationTime = (float*)(m_pManager->GetMallocFunc()(sizeof(float) * parameter->GetChildrenCount()));

		m_generatedChildrenCount = m_flexibleGeneratedChildrenCount;
		maxGenerationChildrenCount = flexibleMaxGenerationChildrenCount_;
		m_nextGenerationTime = m_flexibleNextGenerationTime;
	}

	prevPosition_ = Vec3f(0, 0, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::FirstUpdate()
{
	m_IsFirstTime = false;
	assert(this->m_pContainer != nullptr);

	auto effect = this->m_pEffectNode->m_effect;
	auto instanceGlobal = this->m_pContainer->GetRootInstance();
	auto& rand = m_randObject;

	auto parameter = (EffectNodeImplemented*)m_pEffectNode;

	// initialize children
	for (int32_t i = 0; i < parameter->GetChildrenCount(); i++)
	{
		auto pNode = (EffectNodeImplemented*)parameter->GetChild(i);

		m_generatedChildrenCount[i] = 0;

		auto gt =
			ApplyEq(effect, instanceGlobal, &rand, pNode->CommonValues.RefEqGenerationTimeOffset, pNode->CommonValues.GenerationTimeOffset);

		m_nextGenerationTime[i] = gt.getValue(rand);

		if (pNode->CommonValues.RefEqMaxGeneration >= 0)
		{
			auto maxGene = static_cast<float>(pNode->CommonValues.MaxGeneration);
			ApplyEq(maxGene, effect, instanceGlobal, &rand, pNode->CommonValues.RefEqMaxGeneration, maxGene);
			maxGenerationChildrenCount[i] = static_cast<int32_t>(maxGene);
		}
		else
		{
			maxGenerationChildrenCount[i] = pNode->CommonValues.MaxGeneration;
		}
	}

	if (m_pParent == nullptr)
	{
		// initialize SRT
		m_GenerationLocation = Mat43f::Identity;

		// initialize Parent
		m_ParentMatrix = Mat43f::Identity;

		// Generate zero frame effect

		// for new children
		// UpdateChildrenGroupMatrix();
		//
		// GenerateChildrenInRequired(0.0f);

		return;
	}

	const int32_t parentTime = (int32_t)std::max(0.0f, this->m_pParent->m_LivingTime);

	{
		auto ri = ApplyEq(effect, instanceGlobal, &rand, parameter->CommonValues.RefEqLife, parameter->CommonValues.life);
		m_LivedTime = (float)ri.getValue(rand);
	}

	// initialize SRT

	// calculate parent matrixt to get matrix
	m_pParent->CalculateMatrix(0);

	const Mat43f& parentMatrix = m_pParent->GetGlobalMatrix43();
	m_GlobalPosition = parentMatrix.GetTranslation();
	m_GlobalRevisionLocation = Vec3f(0.0f, 0.0f, 0.0f);
	m_GlobalRevisionVelocity = Vec3f(0.0f, 0.0f, 0.0f);
#ifdef OLD_LF
	modifyWithNoise_ = Vec3f(0.0f, 0.0f, 0.0f);
#else
	localForceField_.Reset();
#endif
	m_GenerationLocation = Mat43f::Identity;

	// 親の初期化
	if (parameter->CommonValues.TranslationBindType == BindType::WhenCreating ||
		parameter->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent ||
		parameter->CommonValues.RotationBindType == BindType::WhenCreating ||
		parameter->CommonValues.ScalingBindType == BindType::WhenCreating)
	{
		m_ParentMatrix = parentMatrix;
		assert(m_ParentMatrix.IsValid());
	}

	// Initialize parent color
	if (parameter->RendererCommon.ColorBindType == BindType::Always)
	{
		ColorParent = m_pParent->ColorInheritance;
	}
	else if (parameter->RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		ColorParent = m_pParent->ColorInheritance;
	}

	steeringVec_ = Vec3f(0, 0, 0);

	if (m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::NotBind_FollowParent ||
		m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent)
	{
		followParentParam.maxFollowSpeed = m_pEffectNode->SteeringBehaviorParam.MaxFollowSpeed.getValue(rand);
		followParentParam.steeringSpeed = m_pEffectNode->SteeringBehaviorParam.SteeringSpeed.getValue(rand) / 100.0f;
	}

	// Translation
	if (m_pEffectNode->TranslationType == ParameterTranslationType_Fixed)
	{
		translation_values.fixed.location = m_pEffectNode->TranslationFixed.Position;
		ApplyDynamicParameterToFixedLocation();

		prevPosition_ = translation_values.fixed.location;
	}
	else if (m_pEffectNode->TranslationType == ParameterTranslationType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->TranslationPVA.RefEqP,
						   m_pEffectNode->TranslationPVA.location,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		translation_values.random.location = rvl.getValue(rand);

		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->TranslationPVA.RefEqV,
						   m_pEffectNode->TranslationPVA.velocity,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		translation_values.random.velocity = rvv.getValue(rand);

		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->TranslationPVA.RefEqA,
						   m_pEffectNode->TranslationPVA.acceleration,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		translation_values.random.acceleration = rva.getValue(rand);

		prevPosition_ = translation_values.random.location;

		steeringVec_ = translation_values.random.velocity;
	}
	else if (m_pEffectNode->TranslationType == ParameterTranslationType_Easing)
	{
		auto rvs = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->TranslationEasing.RefEqS,
						   m_pEffectNode->TranslationEasing.location.start,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);
		auto rve = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->TranslationEasing.RefEqE,
						   m_pEffectNode->TranslationEasing.location.end,
						   m_pEffectNode->DynamicFactor.Tra,
						   m_pEffectNode->DynamicFactor.TraInv);

		translation_values.easing.start = rvs.getValue(rand);
		translation_values.easing.end = rve.getValue(rand);

		prevPosition_ = translation_values.easing.start;
	}
	else if (m_pEffectNode->TranslationType == ParameterTranslationType_FCurve)
	{
		assert(m_pEffectNode->TranslationFCurve != NULL);

		translation_values.fcruve.offset = m_pEffectNode->TranslationFCurve->GetOffsets(rand);

		prevPosition_ = translation_values.fcruve.offset + m_pEffectNode->TranslationFCurve->GetValues(m_LivingTime, m_LivedTime);
	}
	else if (m_pEffectNode->TranslationType == ParameterTranslationType_NurbsCurve)
	{
		// TODO refactoring
		auto& NurbsCurveParam = m_pEffectNode->TranslationNurbsCurve;

		if (NurbsCurveParam.Index != -1)
		{
			Curve* curve = static_cast<Curve*>(m_pEffectNode->m_effect->GetCurve(NurbsCurveParam.Index));
			float moveSpeed = NurbsCurveParam.MoveSpeed;
			int32_t loopType = NurbsCurveParam.LoopType;

			float speed = 1.0f / (curve->GetLength() * NurbsCurveParam.Scale);

			float t = speed * m_LivingTime * moveSpeed;

			switch (loopType)
			{
			default:
			case 0:
				t = fmod(t, 1.0f);
				break;

			case 1:
				if (t > 1.0f)
				{
					t = 1.0f;
				}
				break;
			}

			prevPosition_ = curve->CalcuratePoint(t, NurbsCurveParam.Scale * m_pEffectNode->m_effect->GetMaginification());
		}
		else
		{
			prevPosition_ = {0, 0, 0};
		}
	}
	else if (m_pEffectNode->TranslationType == ParameterTranslationType_ViewOffset)
	{
		translation_values.view_offset.distance = m_pEffectNode->TranslationViewOffset.distance.getValue(rand);
		prevPosition_ = {0, 0, 0};
	}

	// Rotation
	if (m_pEffectNode->RotationType == ParameterRotationType_Fixed)
	{
		rotation_values.fixed.rotation = m_pEffectNode->RotationFixed.Position;
		ApplyDynamicParameterToFixedRotation();
	}
	else if (m_pEffectNode->RotationType == ParameterRotationType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->RotationPVA.RefEqP,
						   m_pEffectNode->RotationPVA.rotation,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->RotationPVA.RefEqV,
						   m_pEffectNode->RotationPVA.velocity,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->RotationPVA.RefEqA,
						   m_pEffectNode->RotationPVA.acceleration,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);

		rotation_values.random.rotation = rvl.getValue(rand);
		rotation_values.random.velocity = rvv.getValue(rand);
		rotation_values.random.acceleration = rva.getValue(rand);
	}
	else if (m_pEffectNode->RotationType == ParameterRotationType_Easing)
	{
		auto rvs = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->RotationEasing.RefEqS,
						   m_pEffectNode->RotationEasing.rotation.start,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);
		auto rve = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->RotationEasing.RefEqE,
						   m_pEffectNode->RotationEasing.rotation.end,
						   m_pEffectNode->DynamicFactor.Rot,
						   m_pEffectNode->DynamicFactor.RotInv);

		rotation_values.easing.start = rvs.getValue(rand);
		rotation_values.easing.end = rve.getValue(rand);
	}
	else if (m_pEffectNode->RotationType == ParameterRotationType_AxisPVA)
	{
		rotation_values.axis.random.rotation = m_pEffectNode->RotationAxisPVA.rotation.getValue(rand);
		rotation_values.axis.random.velocity = m_pEffectNode->RotationAxisPVA.velocity.getValue(rand);
		rotation_values.axis.random.acceleration = m_pEffectNode->RotationAxisPVA.acceleration.getValue(rand);
		rotation_values.axis.rotation = rotation_values.axis.random.rotation;
		rotation_values.axis.axis = m_pEffectNode->RotationAxisPVA.axis.getValue(rand);
		if (rotation_values.axis.axis.GetLength() < 0.001f)
		{
			rotation_values.axis.axis = Vec3f(0, 1, 0);
		}
		rotation_values.axis.axis.Normalize();
	}
	else if (m_pEffectNode->RotationType == ParameterRotationType_AxisEasing)
	{
		rotation_values.axis.easing.start = m_pEffectNode->RotationAxisEasing.easing.start.getValue(rand);
		rotation_values.axis.easing.end = m_pEffectNode->RotationAxisEasing.easing.end.getValue(rand);
		rotation_values.axis.rotation = rotation_values.axis.easing.start;
		rotation_values.axis.axis = m_pEffectNode->RotationAxisEasing.axis.getValue(rand);
		if (rotation_values.axis.axis.GetLength() < 0.001f)
		{
			rotation_values.axis.axis = Vec3f(0, 1, 0);
		}
		rotation_values.axis.axis.Normalize();
	}
	else if (m_pEffectNode->RotationType == ParameterRotationType_FCurve)
	{
		assert(m_pEffectNode->RotationFCurve != NULL);

		rotation_values.fcruve.offset = m_pEffectNode->RotationFCurve->GetOffsets(rand);
	}

	// Scaling
	if (m_pEffectNode->ScalingType == ParameterScalingType_Fixed)
	{
		scaling_values.fixed.scale = m_pEffectNode->ScalingFixed.Position;
		ApplyDynamicParameterToFixedScaling();
	}
	else if (m_pEffectNode->ScalingType == ParameterScalingType_PVA)
	{
		auto rvl = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->ScalingPVA.RefEqP,
						   m_pEffectNode->ScalingPVA.Position,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rvv = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->ScalingPVA.RefEqV,
						   m_pEffectNode->ScalingPVA.Velocity,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rva = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->ScalingPVA.RefEqA,
						   m_pEffectNode->ScalingPVA.Acceleration,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);

		scaling_values.random.scale = rvl.getValue(rand);
		scaling_values.random.velocity = rvv.getValue(rand);
		scaling_values.random.acceleration = rva.getValue(rand);
	}
	else if (m_pEffectNode->ScalingType == ParameterScalingType_Easing)
	{
		auto rvs = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->ScalingEasing.RefEqS,
						   m_pEffectNode->ScalingEasing.Position.start,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);
		auto rve = ApplyEq(effect,
						   instanceGlobal,
						   &rand,
						   m_pEffectNode->ScalingEasing.RefEqE,
						   m_pEffectNode->ScalingEasing.Position.end,
						   m_pEffectNode->DynamicFactor.Scale,
						   m_pEffectNode->DynamicFactor.ScaleInv);

		scaling_values.easing.start = rvs.getValue(rand);
		scaling_values.easing.end = rve.getValue(rand);
	}
	else if (m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA)
	{
		scaling_values.single_random.scale = m_pEffectNode->ScalingSinglePVA.Position.getValue(rand);
		scaling_values.single_random.velocity = m_pEffectNode->ScalingSinglePVA.Velocity.getValue(rand);
		scaling_values.single_random.acceleration = m_pEffectNode->ScalingSinglePVA.Acceleration.getValue(rand);
	}
	else if (m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing)
	{
		scaling_values.single_easing.start = m_pEffectNode->ScalingSingleEasing.start.getValue(rand);
		scaling_values.single_easing.end = m_pEffectNode->ScalingSingleEasing.end.getValue(rand);
	}
	else if (m_pEffectNode->ScalingType == ParameterScalingType_FCurve)
	{
		assert(m_pEffectNode->ScalingFCurve != NULL);

		scaling_values.fcruve.offset = m_pEffectNode->ScalingFCurve->GetOffsets(rand);
	}

	// Spawning Method
	if (m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT)
	{
		Vec3f p = m_pEffectNode->GenerationLocation.point.location.getValue(rand);
		m_GenerationLocation = Mat43f::Translation(p.GetX(), p.GetY(), p.GetZ());
	}
	else if (m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_LINE)
	{
		Vec3f s = m_pEffectNode->GenerationLocation.line.position_start.getValue(rand);
		Vec3f e = m_pEffectNode->GenerationLocation.line.position_end.getValue(rand);
		auto noize = m_pEffectNode->GenerationLocation.line.position_noize.getValue(rand);
		auto division = Max(1, m_pEffectNode->GenerationLocation.line.division);

		Vec3f dir = e - s;

		if (dir.IsZero())
		{
			m_GenerationLocation = Mat43f::Translation(0, 0, 0);
		}
		else
		{
			auto len = dir.GetLength();
			dir /= len;

			int32_t target = 0;
			if (m_pEffectNode->GenerationLocation.line.type == ParameterGenerationLocation::LineType::Order)
			{
				target = m_InstanceNumber % division;
			}
			else if (m_pEffectNode->GenerationLocation.line.type == ParameterGenerationLocation::LineType::Random)
			{
				target = (int32_t)((division)*rand.GetRand());
				if (target == division)
					target -= 1;
			}

			auto d = 0.0f;
			if (division > 1)
			{
				d = (len / (float)(division - 1)) * target;
			}

			d += noize;

			s += dir * d;

			Vec3f xdir;
			Vec3f ydir;
			Vec3f zdir;

			if (fabs(dir.GetY()) > 0.999f)
			{
				xdir = dir;
				zdir = Vec3f::Cross(xdir, Vec3f(-1, 0, 0)).Normalize();
				ydir = Vec3f::Cross(zdir, xdir).Normalize();
			}
			else
			{
				xdir = dir;
				ydir = Vec3f::Cross(Vec3f(0, 0, 1), xdir).Normalize();
				zdir = Vec3f::Cross(xdir, ydir).Normalize();
			}

			if (m_pEffectNode->GenerationLocation.EffectsRotation)
			{
				m_GenerationLocation.X.SetX(xdir.GetX());
				m_GenerationLocation.Y.SetX(xdir.GetY());
				m_GenerationLocation.Z.SetX(xdir.GetZ());

				m_GenerationLocation.X.SetY(ydir.GetX());
				m_GenerationLocation.Y.SetY(ydir.GetY());
				m_GenerationLocation.Z.SetY(ydir.GetZ());

				m_GenerationLocation.X.SetZ(zdir.GetX());
				m_GenerationLocation.Y.SetZ(zdir.GetY());
				m_GenerationLocation.Z.SetZ(zdir.GetZ());
			}
			else
			{
				m_GenerationLocation = Mat43f::Identity;
			}

			m_GenerationLocation.X.SetW(s.GetX());
			m_GenerationLocation.Y.SetW(s.GetY());
			m_GenerationLocation.Z.SetW(s.GetZ());
		}
	}
	else if (m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE)
	{
		Mat43f mat_x = Mat43f::RotationX(m_pEffectNode->GenerationLocation.sphere.rotation_x.getValue(rand));
		Mat43f mat_y = Mat43f::RotationY(m_pEffectNode->GenerationLocation.sphere.rotation_y.getValue(rand));
		float r = m_pEffectNode->GenerationLocation.sphere.radius.getValue(rand);
		m_GenerationLocation = Mat43f::Translation(0, r, 0) * mat_x * mat_y;
	}
	else if (m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_MODEL)
	{
		m_GenerationLocation = Mat43f::Identity;

		int32_t modelIndex = m_pEffectNode->GenerationLocation.model.index;
		if (modelIndex >= 0)
		{
			Model* model = (Model*)m_pEffectNode->GetEffect()->GetModel(modelIndex);
			if (model != NULL)
			{
				Model::Emitter emitter;

				if (m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_RANDOM)
				{
					emitter = model->GetEmitter(&rand,
												parentTime,
												m_pManager->GetCoordinateSystem(),
												((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification());
				}
				else if (m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_VERTEX)
				{
					emitter = model->GetEmitterFromVertex(m_InstanceNumber,
														  parentTime,
														  m_pManager->GetCoordinateSystem(),
														  ((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification());
				}
				else if (m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_VERTEX_RANDOM)
				{
					emitter = model->GetEmitterFromVertex(&rand,
														  parentTime,
														  m_pManager->GetCoordinateSystem(),
														  ((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification());
				}
				else if (m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_FACE)
				{
					emitter = model->GetEmitterFromFace(m_InstanceNumber,
														parentTime,
														m_pManager->GetCoordinateSystem(),
														((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification());
				}
				else if (m_pEffectNode->GenerationLocation.model.type == ParameterGenerationLocation::MODELTYPE_FACE_RANDOM)
				{
					emitter = model->GetEmitterFromFace(&rand,
														parentTime,
														m_pManager->GetCoordinateSystem(),
														((EffectImplemented*)m_pEffectNode->GetEffect())->GetMaginification());
				}

				m_GenerationLocation = Mat43f::Translation(emitter.Position);

				if (m_pEffectNode->GenerationLocation.EffectsRotation)
				{
					m_GenerationLocation.X.SetX(emitter.Binormal.X);
					m_GenerationLocation.Y.SetX(emitter.Binormal.Y);
					m_GenerationLocation.Z.SetX(emitter.Binormal.Z);

					m_GenerationLocation.X.SetY(emitter.Tangent.X);
					m_GenerationLocation.Y.SetY(emitter.Tangent.Y);
					m_GenerationLocation.Z.SetY(emitter.Tangent.Z);

					m_GenerationLocation.X.SetZ(emitter.Normal.X);
					m_GenerationLocation.Y.SetZ(emitter.Normal.Y);
					m_GenerationLocation.Z.SetZ(emitter.Normal.Z);
				}
			}
		}
	}
	else if (m_pEffectNode->GenerationLocation.type == ParameterGenerationLocation::TYPE_CIRCLE)
	{
		m_GenerationLocation = Mat43f::Identity;
		float radius = m_pEffectNode->GenerationLocation.circle.radius.getValue(rand);
		float start = m_pEffectNode->GenerationLocation.circle.angle_start.getValue(rand);
		float end = m_pEffectNode->GenerationLocation.circle.angle_end.getValue(rand);
		int32_t div = Max(m_pEffectNode->GenerationLocation.circle.division, 1);

		int32_t target = 0;
		if (m_pEffectNode->GenerationLocation.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_ORDER)
		{
			target = m_InstanceNumber % div;
		}
		else if (m_pEffectNode->GenerationLocation.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_REVERSE_ORDER)
		{
			target = div - 1 - (m_InstanceNumber % div);
		}
		else if (m_pEffectNode->GenerationLocation.circle.type == ParameterGenerationLocation::CIRCLE_TYPE_RANDOM)
		{
			target = (int32_t)((div)*rand.GetRand());
			if (target == div)
				target -= 1;
		}

		float angle = (end - start) * ((float)target / (float)div) + start;

		angle += m_pEffectNode->GenerationLocation.circle.angle_noize.getValue(rand);

		switch (m_pEffectNode->GenerationLocation.circle.axisDirection)
		{
		case ParameterGenerationLocation::AxisType::X:
			m_GenerationLocation = Mat43f::Translation(0, 0, radius) * Mat43f::RotationX(angle);
			break;
		case ParameterGenerationLocation::AxisType::Y:
			m_GenerationLocation = Mat43f::Translation(radius, 0, 0) * Mat43f::RotationY(angle);
			break;
		case ParameterGenerationLocation::AxisType::Z:
			m_GenerationLocation = Mat43f::Translation(0, radius, 0) * Mat43f::RotationZ(angle);
			break;
		}
	}

	if (m_pEffectNode->SoundType == ParameterSoundType_Use)
	{
		soundValues.delay = (int32_t)m_pEffectNode->Sound.Delay.getValue(rand);
	}

	// UV
	for (int32_t i = 0; i < ParameterRendererCommon::UVParameterNum; i++)
	{
		const auto& UVType = m_pEffectNode->RendererCommon.UVTypes[i];
		const auto& UV = m_pEffectNode->RendererCommon.UVs[i];

		if (UVType == ParameterRendererCommon::UV_ANIMATION)
		{
			auto& uvTimeOffset = uvTimeOffsets[i];
			uvTimeOffset = (int32_t)UV.Animation.StartFrame.getValue(rand);
			uvTimeOffset *= UV.Animation.FrameLength;
		}
		else if (UVType == ParameterRendererCommon::UV_SCROLL)
		{
			auto& uvAreaOffset = uvAreaOffsets[i];
			auto& uvScrollSpeed = uvScrollSpeeds[i];

			auto xy = UV.Scroll.Position.getValue(rand);
			auto zw = UV.Scroll.Size.getValue(rand);

			uvAreaOffset.X = xy.GetX();
			uvAreaOffset.Y = xy.GetY();
			uvAreaOffset.Width = zw.GetX();
			uvAreaOffset.Height = zw.GetY();

			uvScrollSpeed = UV.Scroll.Speed.getValue(rand);
		}
		else if (UVType == ParameterRendererCommon::UV_FCURVE)
		{
			auto& uvAreaOffset = uvAreaOffsets[i];

			uvAreaOffset.X = UV.FCurve.Position->X.GetOffset(rand);
			uvAreaOffset.Y = UV.FCurve.Position->Y.GetOffset(rand);
			uvAreaOffset.Width = UV.FCurve.Size->X.GetOffset(rand);
			uvAreaOffset.Height = UV.FCurve.Size->Y.GetOffset(rand);
		}
	}

	// Alpha Cutoff
	if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::FIXED)
	{
		if (m_pEffectNode->AlphaCutoff.Fixed.RefEq < 0)
		{
			m_AlphaThreshold = m_pEffectNode->AlphaCutoff.Fixed.Threshold;
		}
	}
	else if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::FPI)
	{
		auto& fpiValue = alpha_cutoff_values.four_point_interpolation;
		auto& nodeAlphaCutoffValue = m_pEffectNode->AlphaCutoff.FourPointInterpolation;

		fpiValue.begin_threshold = nodeAlphaCutoffValue.BeginThreshold.getValue(rand);
		fpiValue.transition_frame = static_cast<int32_t>(nodeAlphaCutoffValue.TransitionFrameNum.getValue(rand));
		fpiValue.no2_threshold = nodeAlphaCutoffValue.No2Threshold.getValue(rand);
		fpiValue.no3_threshold = nodeAlphaCutoffValue.No3Threshold.getValue(rand);
		fpiValue.transition_frame2 = static_cast<int32_t>(nodeAlphaCutoffValue.TransitionFrameNum2.getValue(rand));
		fpiValue.end_threshold = nodeAlphaCutoffValue.EndThreshold.getValue(rand);
	}
	else if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::EASING)
	{
		auto& easingValue = alpha_cutoff_values.easing;
		auto& nodeAlphaCutoffValue = m_pEffectNode->AlphaCutoff.Easing;

		easingValue.start = nodeAlphaCutoffValue.Threshold.start.getValue(rand);
		easingValue.end = nodeAlphaCutoffValue.Threshold.end.getValue(rand);
	}
	else if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::F_CURVE)
	{
		auto& fcurveValue = alpha_cutoff_values.fcurve;
		auto& nodeAlphaCutoffValue = m_pEffectNode->AlphaCutoff.FCurve;

		fcurveValue.offset = nodeAlphaCutoffValue.Threshold->GetOffsets(rand);
	}

	// CustomData
	for (int32_t index = 0; index < 2; index++)
	{
		ParameterCustomData* parameterCustomData = nullptr;
		InstanceCustomData* instanceCustomData = nullptr;

		if (index == 0)
		{
			parameterCustomData = &m_pEffectNode->RendererCommon.CustomData1;
			instanceCustomData = &customDataValues1;
		}
		else if (index == 1)
		{
			parameterCustomData = &m_pEffectNode->RendererCommon.CustomData2;
			instanceCustomData = &customDataValues2;
		}

		if (parameterCustomData->Type == ParameterCustomDataType::Fixed2D)
		{
			// none
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Easing2D)
		{
			instanceCustomData->easing.start = parameterCustomData->Easing.Values.start.getValue(rand);
			instanceCustomData->easing.end = parameterCustomData->Easing.Values.end.getValue(rand);
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::Random2D)
		{
			instanceCustomData->random.value = parameterCustomData->Random.Values.getValue(rand);
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::FCurve2D)
		{
			instanceCustomData->fcruve.offset = parameterCustomData->FCurve.Values->GetOffsets(rand);
		}
		else if (parameterCustomData->Type == ParameterCustomDataType::FCurveColor)
		{
			instanceCustomData->fcurveColor.offset = parameterCustomData->FCurveColor.Values->GetOffsets(rand);
		}
	}

	m_pEffectNode->InitializeRenderedInstance(*this, m_pManager);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Update(float deltaFrame, bool shown)
{
	assert(this->m_pContainer != nullptr);

	if (IsFirstTime())
	{
		FirstUpdate();
	}

	// Invalidate matrix
	m_GlobalMatrix43Calculated = false;
	m_ParentMatrix43Calculated = false;

	if (is_time_step_allowed && m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		/* 音の更新(現状放置) */
		if (m_pEffectNode->SoundType == ParameterSoundType_Use)
		{
			float living_time = m_LivingTime;
			float living_time_p = living_time + deltaFrame;

			if (living_time <= (float)soundValues.delay && (float)soundValues.delay < living_time_p)
			{
				m_pEffectNode->PlaySound_(*this, m_pContainer->GetRootInstance(), m_pManager);
			}
		}
	}

	// step time
	// frame 0 - generated time
	// frame 1- now
	if (is_time_step_allowed)
	{
		m_LivingTime += deltaFrame;
	}

	if (shown)
	{
		CalculateMatrix(deltaFrame);
	}
	else if (m_pEffectNode->LocationAbs.type != LocationAbsType::None
#ifdef OLD_LF
			 || m_pEffectNode->LocalForceFieldsOld[0].Turbulence != nullptr || m_pEffectNode->LocalForceFieldsOld[1].Turbulence != nullptr || m_pEffectNode->LocalForceFieldsOld[2].Turbulence != nullptr
#else
			 || m_pEffectNode->LocalForceField.HasValue
#endif
	)
	{
		// If attraction forces are not default, updating is needed in each frame.
		CalculateMatrix(deltaFrame);
	}

	// Get parent color.
	if (m_pParent != NULL)
	{
		if (m_pEffectNode->RendererCommon.ColorBindType == BindType::Always)
		{
			ColorParent = m_pParent->ColorInheritance;
		}
	}

	/* 親の削除処理 */
	if (m_pParent != NULL && m_pParent->GetState() != INSTANCE_STATE_ACTIVE)
	{
		CalculateParentMatrix(deltaFrame);
		m_pParent = nullptr;
	}

	// Create child particles
	// if( !m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct )
	//{
	//	GenerateChildrenInRequired(m_LivingTime);
	//}

	UpdateChildrenGroupMatrix();

	// check whether killed?
	bool killed = false;
	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		// if pass time
		if (m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct)
		{
			if (m_LivingTime > m_LivedTime)
			{
				killed = true;
			}
		}

		// if remove parent
		if (m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved)
		{
			if (m_pParent == nullptr || m_pParent->GetState() != INSTANCE_STATE_ACTIVE)
			{
				m_pParent = nullptr;
				killed = true;
			}
		}

		// if children are removed and going not to generate a child
		if (!killed && m_pEffectNode->CommonValues.RemoveWhenChildrenIsExtinct)
		{
			int maxcreate_count = 0;
			InstanceGroup* group = childrenGroups_;

			for (int i = 0; i < m_pEffectNode->GetChildrenCount(); i++, group = group->NextUsedByInstance)
			{
				auto child = (EffectNodeImplemented*)m_pEffectNode->GetChild(i);

				if (maxGenerationChildrenCount[i] <= m_generatedChildrenCount[i] && group->GetInstanceCount() == 0)
				{
					maxcreate_count++;
				}
				else
				{
					break;
				}
			}

			if (maxcreate_count == m_pEffectNode->GetChildrenCount())
			{
				killed = true;
			}
		}
	}

	{
		auto& CommonValue = m_pEffectNode->RendererCommon;
		auto& UV = CommonValue.UVs[0];
		int UVType = CommonValue.UVTypes[0];

		if (UVType == ParameterRendererCommon::UV_ANIMATION)
		{
			auto time = m_LivingTime + uvTimeOffsets[0];

			// 経過時間を取得
			if (m_pEffectNode->GetType() == eEffectNodeType::EFFECT_NODE_TYPE_RIBBON ||
				m_pEffectNode->GetType() == eEffectNodeType::EFFECT_NODE_TYPE_TRACK)
			{
				auto baseInstance = this->m_pContainer->GetFirstGroup()->GetFirst();
				if (baseInstance != nullptr)
				{
					time = baseInstance->m_LivingTime + baseInstance->uvTimeOffsets[0];
				}
			}

			float fFrameNum = time / (float)UV.Animation.FrameLength;
			int32_t frameNum = (int32_t)fFrameNum;
			int32_t frameCount = UV.Animation.FrameCountX * UV.Animation.FrameCountY;

			if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_ONCE)
			{
				if (frameNum >= frameCount)
				{
					frameNum = frameCount - 1;
				}
			}
			else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_LOOP)
			{
				frameNum %= frameCount;
			}
			else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_REVERSELOOP)
			{
				bool rev = (frameNum / frameCount) % 2 == 1;
				frameNum %= frameCount;
				if (rev)
				{
					frameNum = frameCount - 1 - frameNum;
				}
			}

			m_flipbookIndexAndNextRate = static_cast<float>(frameNum);
			m_flipbookIndexAndNextRate += fFrameNum - static_cast<float>(frameNum);
		}
	}

	if (m_pEffectNode->m_effect->GetVersion() >= 1600)
	{
		auto effect = this->m_pEffectNode->m_effect;
		auto instanceGlobal = this->m_pContainer->GetRootInstance();
		auto& rand = m_randObject;

		if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::FIXED)
		{
			if (m_pEffectNode->AlphaCutoff.Fixed.RefEq >= 0)
			{
				auto alphaThreshold = static_cast<float>(m_pEffectNode->AlphaCutoff.Fixed.Threshold);
				ApplyEq(alphaThreshold,
						effect,
						instanceGlobal,
						&rand,
						m_pEffectNode->AlphaCutoff.Fixed.RefEq,
						alphaThreshold);

				m_AlphaThreshold = alphaThreshold;
			}
		}
		else if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::FPI)
		{
			float t = m_LivingTime / m_LivedTime;
			auto val = alpha_cutoff_values.four_point_interpolation;

			float p[4][2] = {0.0f,
							 val.begin_threshold,
							 float(val.transition_frame) / m_LivedTime,
							 val.no2_threshold,
							 (m_LivedTime - float(val.transition_frame2)) / m_LivedTime,
							 val.no3_threshold,
							 1.0f,
							 val.end_threshold};

			for (int32_t i = 1; i < 4; i++)
			{
				if (0 < p[i][0] && p[i - 1][0] <= t && t <= p[i][0])
				{
					float r = (t - p[i - 1][0]) / (p[i][0] - p[i - 1][0]);
					m_AlphaThreshold = p[i - 1][1] + (p[i][1] - p[i - 1][1]) * r;
					break;
				}
			}
		}
		else if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::EASING)
		{
			m_AlphaThreshold = m_pEffectNode->AlphaCutoff.Easing.Threshold.getValue(
				alpha_cutoff_values.easing.start, alpha_cutoff_values.easing.end, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->AlphaCutoff.Type == ParameterAlphaCutoff::EType::F_CURVE)
		{
			auto fcurve = m_pEffectNode->AlphaCutoff.FCurve.Threshold->GetValues(m_LivingTime, m_LivedTime);
			m_AlphaThreshold = fcurve + alpha_cutoff_values.fcurve.offset;
			m_AlphaThreshold /= 100.0f;
		}
	}

	if (killed)
	{
		// if it need to calculate a matrix
		if (m_pEffectNode->GetChildrenCount() > 0)
		{
			// Get parent color.
			if (m_pParent != nullptr)
			{
				if (m_pEffectNode->RendererCommon.ColorBindType == BindType::Always)
				{
					ColorParent = m_pParent->ColorInheritance;
				}
			}
		}

		// Delete this particle with myself.
		Kill();
		return;
	}

	// allow to pass time
	is_time_step_allowed = true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::CalculateMatrix(float deltaFrame)
{
	// 計算済なら終了
	if (m_GlobalMatrix43Calculated)
		return;

	// if( m_sequenceNumber == ((ManagerImplemented*)m_pManager)->GetSequenceNumber() ) return;
	m_sequenceNumber = ((ManagerImplemented*)m_pManager)->GetSequenceNumber();

	assert(m_pEffectNode != NULL);
	assert(m_pContainer != NULL);

	// 親の処理
	if (m_pParent != NULL)
	{
		CalculateParentMatrix(deltaFrame);
	}

	/* 更新処理 */
	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		Vec3f localPosition;
		Vec3f localAngle;
		Vec3f localScaling;

		/* 位置の更新(時間から直接求めれるよう対応済み) */
		if (m_pEffectNode->TranslationType == ParameterTranslationType_None)
		{
			localPosition = {0, 0, 0};
		}
		else if (m_pEffectNode->TranslationType == ParameterTranslationType_Fixed)
		{
			ApplyDynamicParameterToFixedLocation();

			localPosition = translation_values.fixed.location;
		}
		else if (m_pEffectNode->TranslationType == ParameterTranslationType_PVA)
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localPosition = translation_values.random.location + (translation_values.random.velocity * m_LivingTime) +
							(translation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (m_pEffectNode->TranslationType == ParameterTranslationType_Easing)
		{
			localPosition = m_pEffectNode->TranslationEasing.location.getValue(
				translation_values.easing.start, translation_values.easing.end, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->TranslationType == ParameterTranslationType_FCurve)
		{
			assert(m_pEffectNode->TranslationFCurve != NULL);
			auto fcurve = m_pEffectNode->TranslationFCurve->GetValues(m_LivingTime, m_LivedTime);
			localPosition = fcurve + translation_values.fcruve.offset;
		}
		else if (m_pEffectNode->TranslationType == ParameterTranslationType_NurbsCurve)
		{
			auto& NurbsCurveParam = m_pEffectNode->TranslationNurbsCurve;

			if (NurbsCurveParam.Index != -1)
			{
				Curve* curve = static_cast<Curve*>(m_pEffectNode->m_effect->GetCurve(NurbsCurveParam.Index));
				float moveSpeed = NurbsCurveParam.MoveSpeed;
				int32_t loopType = NurbsCurveParam.LoopType;

				float speed = 1.0f / (curve->GetLength() * NurbsCurveParam.Scale);

				float t = speed * m_LivingTime * moveSpeed;

				switch (loopType)
				{
				default:
				case 0:
					t = fmod(t, 1.0f);
					break;

				case 1:
					if (t > 1.0f)
					{
						t = 1.0f;
					}
					break;
				}

				localPosition = curve->CalcuratePoint(t, NurbsCurveParam.Scale * m_pEffectNode->m_effect->GetMaginification());
			}
			else
			{
				localPosition = {0, 0, 0};
			}
		}
		else if (m_pEffectNode->TranslationType == ParameterTranslationType_ViewOffset)
		{
			localPosition = {0, 0, 0};
		}

		// Velocitty
		Vec3f localVelocity = Vec3f(0, 0, 0);
		if (m_pEffectNode->LocalForceField.HasValue)
		{
			localVelocity = localPosition - prevPosition_;
		}

		if (m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::NotBind_FollowParent ||
			m_pEffectNode->CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent)
		{
			localPosition = prevPosition_;

			Vec3f worldPos = Vec3f::Transform(localPosition, m_ParentMatrix);
			Vec3f toTarget = parentPosition_ - worldPos;

			if (toTarget.GetLength() > followParentParam.maxFollowSpeed)
			{
				toTarget = toTarget.Normalize();
				toTarget *= followParentParam.maxFollowSpeed;
			}

			Vec3f vSteering = toTarget - steeringVec_;
			vSteering *= followParentParam.steeringSpeed;

			steeringVec_ += vSteering * deltaFrame;

			if (steeringVec_.GetLength() > followParentParam.maxFollowSpeed)
			{
				steeringVec_ = steeringVec_.Normalize();
				steeringVec_ *= followParentParam.maxFollowSpeed;
			}

			Vec3f followVelocity = steeringVec_ * deltaFrame * m_pEffectNode->m_effect->GetMaginification();
			localPosition += followVelocity;
		}

		prevPosition_ = localPosition;

		if (!m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			localPosition += m_GenerationLocation.GetTranslation();
		}

		/* 回転の更新(時間から直接求めれるよう対応済み) */
		if (m_pEffectNode->RotationType == ParameterRotationType_None)
		{
			localAngle = {0, 0, 0};
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_Fixed)
		{
			ApplyDynamicParameterToFixedRotation();

			localAngle = rotation_values.fixed.rotation;
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_PVA)
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localAngle = rotation_values.random.rotation + (rotation_values.random.velocity * m_LivingTime) +
						 (rotation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_Easing)
		{
			localAngle = m_pEffectNode->RotationEasing.rotation.getValue(
				rotation_values.easing.start, rotation_values.easing.end, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_AxisPVA)
		{
			rotation_values.axis.rotation = rotation_values.axis.random.rotation + rotation_values.axis.random.velocity * m_LivingTime +
											rotation_values.axis.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f);
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_AxisEasing)
		{
			rotation_values.axis.rotation = m_pEffectNode->RotationAxisEasing.easing.getValue(
				rotation_values.axis.easing.start, rotation_values.axis.easing.end, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_FCurve)
		{
			assert(m_pEffectNode->RotationFCurve != NULL);
			auto fcurve = m_pEffectNode->RotationFCurve->GetValues(m_LivingTime, m_LivedTime);
			localAngle = fcurve + rotation_values.fcruve.offset;
		}

		/* 拡大の更新(時間から直接求めれるよう対応済み) */
		if (m_pEffectNode->ScalingType == ParameterScalingType_None)
		{
			localScaling = {1.0f, 1.0f, 1.0f};
		}
		else if (m_pEffectNode->ScalingType == ParameterScalingType_Fixed)
		{
			ApplyDynamicParameterToFixedScaling();

			localScaling = scaling_values.fixed.scale;
		}
		else if (m_pEffectNode->ScalingType == ParameterScalingType_PVA)
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localScaling = scaling_values.random.scale + (scaling_values.random.velocity * m_LivingTime) +
						   (scaling_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (m_pEffectNode->ScalingType == ParameterScalingType_Easing)
		{
			localScaling = m_pEffectNode->ScalingEasing.Position.getValue(
				scaling_values.easing.start, scaling_values.easing.end, m_LivingTime / m_LivedTime);
		}
		else if (m_pEffectNode->ScalingType == ParameterScalingType_SinglePVA)
		{
			float s = scaling_values.single_random.scale + scaling_values.single_random.velocity * m_LivingTime +
					  scaling_values.single_random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
			localScaling = {s, s, s};
		}
		else if (m_pEffectNode->ScalingType == ParameterScalingType_SingleEasing)
		{
			float s = m_pEffectNode->ScalingSingleEasing.getValue(
				scaling_values.single_easing.start, scaling_values.single_easing.end, m_LivingTime / m_LivedTime);
			localScaling = {s, s, s};
		}
		else if (m_pEffectNode->ScalingType == ParameterScalingType_FCurve)
		{
			assert(m_pEffectNode->ScalingFCurve != NULL);
			auto fcurve = m_pEffectNode->ScalingFCurve->GetValues(m_LivingTime, m_LivedTime);
			localScaling = fcurve + scaling_values.fcruve.offset;
		}

		// update local fields
		Vec3f currentLocalPosition;

		if (m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			// the center of force field depends Spawn method
			// It should be used a result of past frame
			auto location = Mat43f::Translation(localPosition);
			location *= m_GenerationLocation;
			currentLocalPosition = location.GetTranslation();
		}
		else
		{
			currentLocalPosition = localPosition;
		}

#ifdef OLD_LF
		currentLocalPosition += modifyWithNoise_;

		for (const auto& field : m_pEffectNode->LocalForceFieldsOld)
		{
			if (field.Turbulence != nullptr)
			{
				auto mag = static_cast<EffectImplemented*>(m_pEffectNode->GetEffect())->GetMaginification();
				modifyWithNoise_ += field.Turbulence->Noise.Get(currentLocalPosition / mag) * field.Turbulence->Strength * mag;
			}
		}
#else
		currentLocalPosition += localForceField_.ModifyLocation;
		localForceField_.ExternalVelocity = localVelocity;
		localForceField_.Update(m_pEffectNode->LocalForceField, currentLocalPosition, m_pEffectNode->GetEffect()->GetMaginification());
#endif

		/* 描画部分の更新 */
		m_pEffectNode->UpdateRenderedInstance(*this, m_pManager);

		// 回転行列の作成
		Mat43f MatRot;
		if (m_pEffectNode->RotationType == ParameterRotationType_Fixed || m_pEffectNode->RotationType == ParameterRotationType_PVA ||
			m_pEffectNode->RotationType == ParameterRotationType_Easing || m_pEffectNode->RotationType == ParameterRotationType_FCurve)
		{
			MatRot = Mat43f::RotationZXY(localAngle.GetZ(), localAngle.GetX(), localAngle.GetY());
		}
		else if (m_pEffectNode->RotationType == ParameterRotationType_AxisPVA ||
				 m_pEffectNode->RotationType == ParameterRotationType_AxisEasing)
		{
			Vec3f axis = rotation_values.axis.axis;

			MatRot = Mat43f::RotationAxis(axis, rotation_values.axis.rotation);
		}
		else
		{
			MatRot = Mat43f::Identity;
		}

		// Update matrix
		if (m_pEffectNode->GenerationLocation.EffectsRotation)
		{
			m_GlobalMatrix43 = Mat43f::SRT(localScaling, MatRot, localPosition);
			assert(m_GlobalMatrix43.IsValid());

			m_GlobalMatrix43 *= m_GenerationLocation;
			assert(m_GlobalMatrix43.IsValid());
#ifdef OLD_LF
			m_GlobalMatrix43 *= Mat43f::Translation(modifyWithNoise_);
#else
			m_GlobalMatrix43 *= Mat43f::Translation(localForceField_.ModifyLocation);
#endif
		}
		else
		{
#ifdef OLD_LF
			localPosition += modifyWithNoise_;
#else
			localPosition += localForceField_.ModifyLocation;
#endif

			m_GlobalMatrix43 = Mat43f::SRT(localScaling, MatRot, localPosition);
			assert(m_GlobalMatrix43.IsValid());
		}

		if (m_pEffectNode->TranslationType != ParameterTranslationType_ViewOffset)
		{
			m_GlobalMatrix43 *= m_ParentMatrix;
			assert(m_GlobalMatrix43.IsValid());
		}

		if (m_pEffectNode->LocationAbs.type != LocationAbsType::None)
		{
			Vec3f currentTranslation = m_GlobalMatrix43.GetTranslation();
			assert(m_GlobalMatrix43.IsValid());

			m_GlobalVelocity = currentTranslation - m_GlobalPosition;
			m_GlobalPosition = currentTranslation;

			ModifyMatrixFromLocationAbs(deltaFrame);
		}
	}

	m_GlobalMatrix43Calculated = true;
}

void Instance::CalculateParentMatrix(float deltaFrame)
{
	// 計算済なら終了
	if (m_ParentMatrix43Calculated)
		return;

	// 親の行列を計算
	m_pParent->CalculateMatrix(deltaFrame);

	parentPosition_ = m_pParent->GetGlobalMatrix43().GetTranslation();

	if (m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT)
	{
		TranslationParentBindType tType = m_pEffectNode->CommonValues.TranslationBindType;
		BindType rType = m_pEffectNode->CommonValues.RotationBindType;
		BindType sType = m_pEffectNode->CommonValues.ScalingBindType;

		if (tType == BindType::WhenCreating 
			|| tType == TranslationParentBindType::WhenCreating_FollowParent
			&& rType == BindType::WhenCreating && sType == BindType::WhenCreating)
		{
			// do not do anything
		}
		else if (tType == BindType::Always && rType == BindType::Always && sType == BindType::Always)
		{
			m_ParentMatrix = ownGroup_->GetParentMatrix();
			assert(m_ParentMatrix.IsValid());
		}
		else
		{
			Vec3f s, t;
			Mat43f r;

			if (tType == BindType::WhenCreating
				|| tType == TranslationParentBindType::WhenCreating_FollowParent
				)
				t = m_ParentMatrix.GetTranslation();
			else
				t = ownGroup_->GetParentTranslation();

			if (rType == BindType::WhenCreating)
				r = m_ParentMatrix.GetRotation();
			else
				r = ownGroup_->GetParentRotation();

			if (sType == BindType::WhenCreating)
				s = m_ParentMatrix.GetScale();
			else
				s = ownGroup_->GetParentScale();

			m_ParentMatrix = Mat43f::SRT(s, r, t);
			assert(m_ParentMatrix.IsValid());
		}
	}

	m_ParentMatrix43Calculated = true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::ModifyMatrixFromLocationAbs(float deltaFrame)
{
	InstanceGlobal* instanceGlobal = m_pContainer->GetRootInstance();

	// Update attraction forces
	if (m_pEffectNode->LocationAbs.type == LocationAbsType::None)
	{
	}
	else if (m_pEffectNode->LocationAbs.type == LocationAbsType::Gravity)
	{
		m_GlobalRevisionLocation = m_pEffectNode->LocationAbs.gravity * (m_LivingTime * m_LivingTime * 0.5f);
	}
	else if (m_pEffectNode->LocationAbs.type == LocationAbsType::AttractiveForce)
	{
		float force = m_pEffectNode->LocationAbs.attractiveForce.force;
		float control = m_pEffectNode->LocationAbs.attractiveForce.control;
		float minRange = m_pEffectNode->LocationAbs.attractiveForce.minRange;
		float maxRange = m_pEffectNode->LocationAbs.attractiveForce.maxRange;

		Vec3f position = m_GlobalPosition - m_GlobalVelocity + m_GlobalRevisionLocation;

		Vec3f targetDifference = instanceGlobal->GetTargetLocation() - position;
		float targetDistance = targetDifference.GetLength();
		if (targetDistance > 0.0f)
		{
			Vec3f targetDirection = targetDifference / targetDistance;

			if (minRange > 0.0f || maxRange > 0.0f)
			{
				if (targetDistance >= m_pEffectNode->LocationAbs.attractiveForce.maxRange)
				{
					force = 0.0f;
				}
				else if (targetDistance > m_pEffectNode->LocationAbs.attractiveForce.minRange)
				{
					force *= 1.0f - (targetDistance - minRange) / (maxRange - minRange);
				}
			}

			if (deltaFrame > 0)
			{
				float eps = 0.0001f;
				m_GlobalRevisionVelocity += targetDirection * force * deltaFrame;
				float currentVelocity = m_GlobalRevisionVelocity.GetLength() + eps;
				Vec3f currentDirection = m_GlobalRevisionVelocity / currentVelocity;

				m_GlobalRevisionVelocity = (targetDirection * control + currentDirection * (1.0f - control)) * currentVelocity;
				m_GlobalRevisionLocation += m_GlobalRevisionVelocity * deltaFrame;
			}
		}
	}

	Mat43f MatTraGlobal = Mat43f::Translation(m_GlobalRevisionLocation);
	m_GlobalMatrix43 *= MatTraGlobal;
	assert(m_GlobalMatrix43.IsValid());
}

void Instance::ApplyDynamicParameterToFixedLocation()
{
	if (m_pEffectNode->TranslationFixed.RefEq >= 0)
	{
		translation_values.fixed.location = ApplyEq(m_pEffectNode->GetEffect(),
													m_pContainer->GetRootInstance(),
													&m_randObject,
													m_pEffectNode->TranslationFixed.RefEq,
													m_pEffectNode->TranslationFixed.Position,
													m_pEffectNode->DynamicFactor.Tra,
													m_pEffectNode->DynamicFactor.TraInv);
	}
}

void Instance::ApplyDynamicParameterToFixedRotation()
{
	if (m_pEffectNode->RotationFixed.RefEq >= 0)
	{
		rotation_values.fixed.rotation = ApplyEq(m_pEffectNode->GetEffect(),
												 m_pContainer->GetRootInstance(),
												 &m_randObject,
												 m_pEffectNode->RotationFixed.RefEq,
												 m_pEffectNode->RotationFixed.Position,
												 m_pEffectNode->DynamicFactor.Rot,
												 m_pEffectNode->DynamicFactor.RotInv);
	}
}

void Instance::ApplyDynamicParameterToFixedScaling()
{
	if (m_pEffectNode->ScalingFixed.RefEq >= 0)
	{
		scaling_values.fixed.scale = ApplyEq(m_pEffectNode->GetEffect(),
											 m_pContainer->GetRootInstance(),
											 &m_randObject,
											 m_pEffectNode->ScalingFixed.RefEq,
											 m_pEffectNode->ScalingFixed.Position,
											 m_pEffectNode->DynamicFactor.Scale,
											 m_pEffectNode->DynamicFactor.ScaleInv);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Draw(Instance* next)
{
	assert(m_pEffectNode != NULL);

	if (!m_pEffectNode->IsRendered)
		return;

	if (m_sequenceNumber != ((ManagerImplemented*)m_pManager)->GetSequenceNumber())
	{
		CalculateMatrix(0);
	}

	m_pEffectNode->Rendering(*this, next, m_pManager);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Kill()
{
	if (m_State == INSTANCE_STATE_ACTIVE)
	{
		for (InstanceGroup* group = childrenGroups_; group != NULL; group = group->NextUsedByInstance)
		{
			group->IsReferencedFromInstance = false;
		}

		m_State = INSTANCE_STATE_REMOVING;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RectF Instance::GetUV(const int32_t index) const
{
	RectF uv(0.0f, 0.0f, 1.0f, 1.0f);

	const auto& UVType = m_pEffectNode->RendererCommon.UVTypes[index];
	const auto& UV = m_pEffectNode->RendererCommon.UVs[index];

	if (UVType == ParameterRendererCommon::UV_DEFAULT)
	{
		return RectF(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (UVType == ParameterRendererCommon::UV_FIXED)
	{
		uv = RectF(UV.Fixed.Position.x, UV.Fixed.Position.y, UV.Fixed.Position.w, UV.Fixed.Position.h);
	}
	else if (UVType == ParameterRendererCommon::UV_ANIMATION)
	{
		auto& uvTimeOffset = uvTimeOffsets[index];

		auto time = m_LivingTime + uvTimeOffset;

		int32_t frameNum = (int32_t)(time / UV.Animation.FrameLength);
		int32_t frameCount = UV.Animation.FrameCountX * UV.Animation.FrameCountY;

		if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_ONCE)
		{
			if (frameNum >= frameCount)
			{
				frameNum = frameCount - 1;
			}
		}
		else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_LOOP)
		{
			frameNum %= frameCount;
		}
		else if (UV.Animation.LoopType == UV.Animation.LOOPTYPE_REVERSELOOP)
		{
			bool rev = (frameNum / frameCount) % 2 == 1;
			frameNum %= frameCount;
			if (rev)
			{
				frameNum = frameCount - 1 - frameNum;
			}
		}

		int32_t frameX = frameNum % UV.Animation.FrameCountX;
		int32_t frameY = frameNum / UV.Animation.FrameCountX;

		uv = RectF(UV.Animation.Position.x + UV.Animation.Position.w * frameX,
				   UV.Animation.Position.y + UV.Animation.Position.h * frameY,
				   UV.Animation.Position.w,
				   UV.Animation.Position.h);
	}
	else if (UVType == ParameterRendererCommon::UV_SCROLL)
	{
		auto& uvAreaOffset = uvAreaOffsets[index];
		auto& uvScrollSpeed = uvScrollSpeeds[index];

		auto time = (int32_t)m_LivingTime;

		uv = RectF(uvAreaOffset.X + uvScrollSpeed.GetX() * time,
				   uvAreaOffset.Y + uvScrollSpeed.GetY() * time,
				   uvAreaOffset.Width,
				   uvAreaOffset.Height);
	}
	else if (UVType == ParameterRendererCommon::UV_FCURVE)
	{
		auto& uvAreaOffset = uvAreaOffsets[index];

		auto time = (int32_t)m_LivingTime;

		auto fcurvePos = UV.FCurve.Position->GetValues(m_LivingTime, m_LivedTime);
		auto fcurveSize = UV.FCurve.Size->GetValues(m_LivingTime, m_LivedTime);

		uv = RectF(uvAreaOffset.X + fcurvePos.GetX(),
				   uvAreaOffset.Y + fcurvePos.GetY(),
				   uvAreaOffset.Width + fcurveSize.GetX(),
				   uvAreaOffset.Height + fcurveSize.GetY());
	}

	// For webgl bug (it makes slow if sampling points are too far on WebGL)
	float far = 4.0;

	if (uv.X < -far && uv.X + uv.Width < -far)
	{
		uv.X += (-static_cast<int32_t>(uv.X) - far);
	}

	if (uv.X > far && uv.X + uv.Width > far)
	{
		uv.X -= (static_cast<int32_t>(uv.X) - far);
	}

	if (uv.Y < -far && uv.Y + uv.Height < -far)
	{
		uv.Y += (-static_cast<int32_t>(uv.Y) - far);
	}

	if (uv.Y > far && uv.Y + uv.Height > far)
	{
		uv.Y -= (static_cast<int32_t>(uv.Y) - far);
	}

	return uv;
}

std::array<float, 4> Instance::GetCustomData(int32_t index) const
{
	assert(0 <= index && index < 2);

	ParameterCustomData* parameterCustomData = nullptr;
	const InstanceCustomData* instanceCustomData = nullptr;

	if (index == 0)
	{
		parameterCustomData = &m_pEffectNode->RendererCommon.CustomData1;
		instanceCustomData = &customDataValues1;
	}
	else if (index == 1)
	{
		parameterCustomData = &m_pEffectNode->RendererCommon.CustomData2;
		instanceCustomData = &customDataValues2;
	}
	else
	{
		return std::array<float, 4>{0.0f, 0.0f, 0, 0};
	}

	if (parameterCustomData->Type == ParameterCustomDataType::None)
	{
		return std::array<float, 4>{0, 0, 0, 0};
	}
	else if (parameterCustomData->Type == ParameterCustomDataType::Fixed2D)
	{
		auto v = parameterCustomData->Fixed.Values;
		return std::array<float, 4>{v.x, v.y, 0, 0};
	}
	else if (parameterCustomData->Type == ParameterCustomDataType::Random2D)
	{
		auto v = instanceCustomData->random.value;
		return std::array<float, 4>{v.GetX(), v.GetY(), 0, 0};
	}
	else if (parameterCustomData->Type == ParameterCustomDataType::Easing2D)
	{
		Vec2f v = parameterCustomData->Easing.Values.getValue(
			instanceCustomData->easing.start, instanceCustomData->easing.end, m_LivingTime / m_LivedTime);
		return std::array<float, 4>{v.GetX(), v.GetY(), 0, 0};
	}
	else if (parameterCustomData->Type == ParameterCustomDataType::FCurve2D)
	{
		auto values = parameterCustomData->FCurve.Values->GetValues(m_LivingTime, m_LivedTime);
		return std::array<float, 4>{
			values.GetX() + instanceCustomData->fcruve.offset.GetX(), values.GetY() + instanceCustomData->fcruve.offset.GetY(), 0, 0};
	}
	else if (parameterCustomData->Type == ParameterCustomDataType::Fixed4D)
	{
		return parameterCustomData->Fixed4D;
	}
	else if (parameterCustomData->Type == ParameterCustomDataType::FCurveColor)
	{
		auto values = parameterCustomData->FCurveColor.Values->GetValues(m_LivingTime, m_LivedTime);
		return std::array<float, 4>{(values[0] + instanceCustomData->fcurveColor.offset[0]) / 255.0f,
									(values[1] + instanceCustomData->fcurveColor.offset[1]) / 255.0f,
									(values[2] + instanceCustomData->fcurveColor.offset[2]) / 255.0f,
									(values[3] + instanceCustomData->fcurveColor.offset[3]) / 255.0f};
	}
	else
	{
		assert(false);
	}

	return std::array<float, 4>{0, 0, 0, 0};
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
