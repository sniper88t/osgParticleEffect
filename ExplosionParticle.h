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
osg::Node* createExplosion(const osg::Vec3& position, ExplosionType explosionType = ExplosionType::Mushroom1);