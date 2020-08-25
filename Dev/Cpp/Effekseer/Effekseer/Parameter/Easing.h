
#ifndef __EFFEKSEER_EASING_H__
#define __EFFEKSEER_EASING_H__

#include "../Effekseer.Base.Pre.h"
#include "../Effekseer.InternalStruct.h"
#include "../Utils/Effekseer.BinaryReader.h"
#include "Effekseer.Parameters.h"

namespace Effekseer
{

enum class Easing3Type : int32_t
{
	StartEndSpeed = 0,
	Linear = 1,
};

struct InstanceEasing3
{
	Vec3f start;
	Vec3f middle;
	Vec3f end;
};

struct ParameterEasing3
{
private:
	float getEasingStartEndValue(float t) const
	{
		return params[0] * t * t * t + params[1] * t * t + params[2] * t;
	}

	float getEasingLinearValue(float t) const
	{
		return t;
	}

	Vec3f get2Point(const InstanceEasing3& v, float t) const
	{
		Vec3f size = v.end - v.start;
		return v.start + size * t;
	}

	//! calculate with spline
	Vec3f get3Point(const InstanceEasing3& v, float t) const
	{
		std::array<Effekseer::Vec3f, 3> a = {v.start, v.middle, v.end};
		std::array<Effekseer::Vec3f, 3> b;
		std::array<Effekseer::Vec3f, 3> c;
		std::array<Effekseer::Vec3f, 3> d;
		std::array<Effekseer::Vec3f, 3> w;
		std::array<bool, 3> isSame;

		isSame[0] = a[1] == a[0];
		isSame[1] = a[2] == a[1];

		for (size_t i = 1; i < a.size() - 1; i++)
		{
			c[i] = (a[i - 1] + a[i] * (-2.0f) + a[i + 1]) * 3.0f;
		}

		for (size_t i = 1; i < a.size() - 1; i++)
		{
			auto tmp = Effekseer::Vec3f(4.0f, 4.0f, 4.0f) - w[i - 1];
			c[i] = (c[i] - c[i - 1]) / tmp;
			w[i] = Effekseer::Vec3f(1.0f, 1.0f, 1.0f) / tmp;
		}

		for (size_t i = (a.size() - 1) - 1; i > 0; i--)
		{
			c[i] = c[i] - c[i + 1] * w[i];
		}

		for (size_t i = 0; i < a.size() - 1; i++)
		{
			d[i] = (c[i + 1] - c[i]) / 3.0;
			b[i] = a[i + 1] - a[i] - c[i] - d[i];
		}

		t *= 2.0f;

		int32_t j = (int32_t)floorf(t);

		if (j < 0)
		{
			j = 0;
		}

		if (j > (int32_t)a.size())
		{
			j = (int32_t)a.size() - 1;
		}

		auto dt = t - j;

		if (j < (int32_t)isSame.size() && isSame[j])
			return a[j];

		return a[j] + (b[j] + (c[j] + d[j] * dt) * dt) * dt;
	}

public:
	RefMinMax RefEqS;
	RefMinMax RefEqE;
	RefMinMax RefEqM;

	random_vector3d start;
	random_vector3d end;
	random_vector3d middle;

	Easing3Type type;
	std::array<float, 4> params;

	int32_t channelCount = 0;
	std::array<int, 3> channelIDs;

	bool isMiddleEnabled = false;

	void Load(uint8_t* pos, int32_t size, int32_t version)
	{
		BinaryReader<true> reader(pos, size);

		if (version >= 14)
		{
			reader.Read<RefMinMax>(RefEqS);
			reader.Read<RefMinMax>(RefEqE);
		}

		reader.Read<random_vector3d>(start);
		reader.Read<random_vector3d>(end);

		// middle parameter
		if (version >= 1600)
		{
			int32_t isMiddleEnabledNum = 0;
			reader.Read<int32_t>(isMiddleEnabledNum);
			isMiddleEnabled = isMiddleEnabledNum > 0;

			if (isMiddleEnabled)
			{
				reader.Read<RefMinMax>(RefEqM);
				reader.Read<random_vector3d>(middle);
			}
		}

		if (version >= 1600)
		{
			reader.Read<Easing3Type>(type);

			if (type == Easing3Type::StartEndSpeed)
			{
				reader.Read<float>(params[0]);
				reader.Read<float>(params[1]);
				reader.Read<float>(params[2]);
			}
		}
		else
		{
			reader.Read<float>(params[0]);
			reader.Read<float>(params[1]);
			reader.Read<float>(params[2]);
		}

		if (version >= 1600)
		{
			int channel = 0;
			reader.Read<int>(channel);

			channelIDs[0] = channel & 0xff;
			channelIDs[1] = (channel >> 8) & 0xff;
			channelIDs[2] = (channel >> 16) & 0xff;

			for (int i = 0; i < 3; i++)
			{
				channelCount = std::max(channelCount, channelIDs[i]);
			}
			channelCount++;
		}
		else
		{
			channelCount = 3;
			channelIDs[0] = 0;
			channelIDs[1] = 1;
			channelIDs[2] = 2;
		}
	}

	void Init(InstanceEasing3& instance, Effect* e, InstanceGlobal* instg, Instance* parent, IRandObject* rand, const std::array<float, 3>& scale, const std::array<float, 3>& scaleInv)
	{
		auto rvs = ApplyEq(e,
						   instg,
						   parent,
						   rand,
						   RefEqS,
						   start,
						   scale,
						   scaleInv);
		auto rve = ApplyEq(e,
						   instg,
						   parent,
						   rand,
						   RefEqE,
						   end,
						   scale,
						   scaleInv);

		instance.start = rvs.getValue(*rand);
		instance.end = rve.getValue(*rand);

		if (isMiddleEnabled)
		{
			auto rvm = ApplyEq(e,
							   instg,
							   parent,
							   rand,
							   RefEqM,
							   end,
							   scale,
							   scaleInv);

			instance.middle = rvm.getValue(*rand);
		}
	}

	Vec3f GetValue(const InstanceEasing3& instance, float time)
	{
		auto t = 0.0f;
		if (type == Easing3Type::StartEndSpeed)
		{
			t = getEasingStartEndValue(time);
		}
		else if (type == Easing3Type::Linear)
		{
			t = getEasingLinearValue(time);
		}

		if (isMiddleEnabled)
		{
			return get3Point(instance, t);
		}

		return get2Point(instance, t);
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETERS_H__