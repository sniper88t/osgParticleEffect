#include "Explosion.h"

#include <osg/Group>

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>

#include <osgParticle/ExplosionEffect>
#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/SmokeTrailEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/Interpolator>


osg::Node* getExplosion(const osg::Vec3& position)
{
	osg::Group* root = new osg::Group;

	int	explosionType = 3 * ((float)rand() / (float)RAND_MAX);
	float scale = 10.0f * ((float)rand() / (float)RAND_MAX);
	scale += 10.0f;

	if (explosionType == 0)
	{
		osgParticle::SmokeEffect* smoke = new osgParticle::SmokeEffect(position, scale * 0.5f, 9.0f);
		osgParticle::FireEffect* fire = new osgParticle::FireEffect(position, scale * 1.5f, 5.0f);
		smoke->setParticleDuration(4.5f);
		smoke->getEmitter()->setLifeTime(1.0f);
		smoke->getEmitter()->setEndless(false);
		smoke->getParticleSystem()->getDefaultParticleTemplate().setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.9f, 0.9f, 1.0f), osg::Vec4(0.5f, 0.4f, 0.4f, 0.0f)));
		fire->setParticleDuration(0.3f);
		fire->getEmitter()->setLifeTime(0.3f);
		fire->getEmitter()->setEndless(false);

		root->addChild(smoke);
		root->addChild(fire);
		root->addChild(createMushroom(position, scale));
		root->addChild(createMushroomdown(position, scale));
	}
	else if (explosionType == 1)
	{
		osgParticle::SmokeEffect* smoke = new osgParticle::SmokeEffect(position, scale * 0.5f, 9.0f);
		osgParticle::FireEffect* fire = new osgParticle::FireEffect(position, scale * 1.5f, 5.0f);
		smoke->setParticleDuration(4.5f);
		smoke->getEmitter()->setLifeTime(1.0f);
		smoke->getEmitter()->setEndless(false);
		smoke->getParticleSystem()->getDefaultParticleTemplate().setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.9f, 0.9f, 1.0f), osg::Vec4(0.5f, 0.4f, 0.4f, 0.0f)));
		fire->setParticleDuration(0.3f);
		fire->getEmitter()->setLifeTime(0.3f);
		fire->getEmitter()->setEndless(false);

		root->addChild(smoke);
		root->addChild(fire);

		root->addChild(createGroundburst(position, scale));
	}
	else
	{
		osgParticle::FireEffect* fire = new osgParticle::FireEffect(position, scale * 0.8f, 5.0f);
		fire->setParticleDuration(0.1f);
		fire->getEmitter()->setLifeTime(0.1f);
		fire->getEmitter()->setEndless(false);

		root->addChild(fire);

		root->addChild(createLandmine(position, scale));
	}

	return root;
};


