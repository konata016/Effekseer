﻿

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.CustomAllocator.h"
#include <assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

void* InstanceGlobal::operator new(size_t size)
{
	assert(sizeof(InstanceGlobal) == size);
	return GetMallocFunc()(size);
}

void InstanceGlobal::operator delete(void* p) {GetFreeFunc()(p, sizeof(InstanceGlobal)); }

InstanceGlobal::InstanceGlobal()
	: m_instanceCount	( 0 )
	, m_updatedFrame	( 0 )
	, m_rootContainer	( NULL )
{ 
	dynamicInputParameters.fill(0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGlobal::~InstanceGlobal()
{
	
}

std::array<float, 4> InstanceGlobal::GetDynamicParameter(int32_t index) {
	assert(0 <= index && index < dynamicParameters.size());
	return dynamicParameters[index];
}

void InstanceGlobal::SetSeed(int64_t seed)
{
	m_seed = seed;
}

float InstanceGlobal::GetRand()
{
	const int a = 1103515245;
	const int c = 12345;
	const int m = 2147483647;
	
	m_seed = (m_seed * a + c) & m;
	auto ret = m_seed % 0x7fff;

	return (float)ret / (float) (0x7fff - 1);
}

float InstanceGlobal::GetRand(float min_, float max_)
{
	return GetRand() * (max_ - min_) + min_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::AddUpdatedFrame( float frame )
{
	m_updatedFrame += frame;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::IncInstanceCount()
{
	m_instanceCount++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::DecInstanceCount()
{
	m_instanceCount--;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGlobal::GetInstanceCount()
{
	return m_instanceCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float InstanceGlobal::GetUpdatedFrame()
{
	return m_updatedFrame;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* InstanceGlobal::GetRootContainer() const
{
	return m_rootContainer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGlobal::SetRootContainer( InstanceContainer* container )
{
	m_rootContainer = container;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const Vector3D& InstanceGlobal::GetTargetLocation() const
{
	return m_targetLocation;
}

void InstanceGlobal::SetTargetLocation( const Vector3D& location )
{
	m_targetLocation = location;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------