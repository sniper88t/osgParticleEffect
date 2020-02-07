#pragma once

#include <osg/Node>
#include <osgParticle/ParticleSystem>

osg::Node* getExplosion(const osg::Vec3 &position);

enum class ExplosionType
{
	Mushroom1,
	Mushroom2,
	Groundburst,
	Landmine
};
osg::Node* createMushroom(const osg::Vec3& position, float scale);
osg::Node* createMushroomdown(const osg::Vec3& position, float scale);
osg::Node* createGroundburst(const osg::Vec3& position, float scale);
osg::Node* createLandmine(const osg::Vec3& position, float scale);

//////////////////////////////////////////////////////////////////////////////
// CUSTOM INTERPOLATOR CLASS
//////////////////////////////////////////////////////////////////////////////

class ExpInterpolator : public osgParticle::Interpolator
{
public:
	ExpInterpolator()
		: osgParticle::Interpolator(), _exponent(1.0f) {}

	ExpInterpolator(float exponent)
		: osgParticle::Interpolator(), _exponent(exponent) {}

	ExpInterpolator(const ExpInterpolator& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
		: osgParticle::Interpolator(copy, copyop), _exponent(copy._exponent) {}

	META_Object(osgParticle, ExpInterpolator);

	virtual float interpolate(float t, float y1, float y2) const
	{
		return y1 + (y2 - y1) * (1.0f - pow((1.0f - t), _exponent));
	}

	virtual osg::Vec2 interpolate(float t, const osg::Vec2& y1, const osg::Vec2& y2) const
	{
		return y1 + (y2 - y1) * (1.0f - pow((1.0f - t), _exponent));
	}

	virtual osg::Vec3 interpolate(float t, const osg::Vec3& y1, const osg::Vec3& y2) const
	{
		return y1 + (y2 - y1) * (1.0f - pow((1.0f - t), _exponent));
	}

	virtual osg::Vec4 interpolate(float t, const osg::Vec4& y1, const osg::Vec4& y2) const
	{
		return y1 + (y2 - y1) * (1.0f - pow((1.0f - t), _exponent));
	}

protected:
	virtual ~ExpInterpolator() {}

private:
	float _exponent;
};
