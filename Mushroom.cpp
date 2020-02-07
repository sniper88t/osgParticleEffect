#include "Explosion.h"

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>


//////////////////////////////////////////////////////////////////////////////
// CUSTOM OPERATOR CLASS
//////////////////////////////////////////////////////////////////////////////

class MushroomOperator : public osgParticle::Operator {
public:
	MushroomOperator()
		: osgParticle::Operator(), _center(0, 0, 0), _axis(0, 0, 1), _wind(0, 0, 0) {}

	MushroomOperator(const MushroomOperator& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
		: osgParticle::Operator(copy, copyop), _center(copy._center), _axis(copy._axis), _wind(copy._wind) {}

	META_Object(osgParticle, MushroomOperator);

	void setCenter(const osg::Vec3& c)
	{
		_center = c;
	}

	void setAxis(const osg::Vec3& a)
	{
		_axis = a / a.length();
	}

	void setWind(const osg::Vec3& wind)
	{
		_wind = wind;
	}

	void beginOperate(osgParticle::Program* prg)
	{
		if (prg->getReferenceFrame() == osgParticle::Program::RELATIVE_RF) {
			_wcenter = prg->transformLocalToWorld(_center);
			_waxis = prg->rotateLocalToWorld(_axis);
			
		}
		else {
			_wcenter = _center;
			_waxis = _axis;
		}
	}
	void operate(osgParticle::Particle* P, double dt)
	{
		osg::Vec3 vCenter = P->getPosition() - _wcenter;
		vCenter.normalize();

		float R = osg::Vec2(vCenter.x(), vCenter.y()).length();
		osg::Vec3 vVel = P->getVelocity();
		if (P->getAge() > 0.3)
			P->setVelocity(osg::Vec3(0.0f, 0.0f, 2.5f) + vCenter * 2.0f);
		else
		{
			P->setVelocity(osg::Vec3(vVel.x(), vVel.y(), vVel.z() * 0.93));
			P->addVelocity(osg::Vec3(0.0f, 0.0f, 0.5f));
		}
		P->setAngle(P->getAngle() + osg::Vec3(0, 0, R * 0.01));
	}

protected:
	virtual ~MushroomOperator() {}

private:
	osg::Vec3 _center;
	osg::Vec3 _wcenter;
	osg::Vec3 _axis;
	osg::Vec3 _waxis;
	osg::Vec3 _wind;
};

osg::Node* createMushroom(const osg::Vec3& position, float scale)
{
	float lifeTime = 5.0f;

	float emitterStartTime = 0.0f;
	float emitterLifeTime = 0.05f;
	float minCount = scale * 17.0f;
	float maxCount = scale * 19.0f;
	float minTheta = 0.0f;
	float maxTheta = osg::PI_2;
	float minSpeed = scale * 1.7f;
	float maxSpeed = scale * 2.0f;

	float intensity = 2;
	float radius = 0.2f * scale;
	float density = 1.2f; // kg/m^3
	bool  useLocalParticleSystem = true;

	osg::Group* root = new osg::Group;

	osgParticle::Particle particle;

	particle.setLifeTime(lifeTime);
	particle.setSizeInterpolator(new ExpInterpolator(128.0f));
	particle.setSizeRange(osgParticle::rangef(0.0f, radius));
	particle.setAlphaRange(osgParticle::rangef(0.1f, 1.0f));
	particle.setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.9f, 0.9f, 1.0f), osg::Vec4(0.5f, 0.4f, 0.4f, 0.0f)));
	particle.setMass(density * radius * radius * radius * osg::PI * 4.0f / 3.0f);

	osg::ref_ptr<osgParticle::ParticleSystem> particleSystem = new osgParticle::ParticleSystem;
	particleSystem->setDefaultAttributes("Images/smoke.rgb", false, false);
	particleSystem->setDefaultParticleTemplate(particle);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter;
	emitter->setParticleSystem(particleSystem.get());
	emitter->setReferenceFrame(useLocalParticleSystem ? osgParticle::ParticleProcessor::ABSOLUTE_RF : osgParticle::ParticleProcessor::RELATIVE_RF);
	emitter->setStartTime(emitterStartTime);
	emitter->setLifeTime(emitterLifeTime);
	emitter->setEndless(false);

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter;
	counter->setRateRange(minCount / emitterLifeTime, maxCount / emitterLifeTime);
	emitter->setCounter(counter.get());

	osg::ref_ptr<osgParticle::SectorPlacer> placer = new osgParticle::SectorPlacer;
	placer->setCenter(position);
	//placer->setRadiusRange(0.0f, 0.0f);
	//placer->setPhiRange(0, 2 * osg::PI);
	emitter->setPlacer(placer.get());

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter;
	shooter->setThetaRange(minTheta, maxTheta);
	shooter->setInitialSpeedRange(minSpeed, maxSpeed);		//shooter->setInitialSpeedRange(1.0f * scale, 10.0f * scale);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());


	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram;
	program->setParticleSystem(particleSystem);

	// create an operator that simulates the gravity acceleration.
	//osg::ref_ptr<osgParticle::AccelOperator> gravity = new osgParticle::AccelOperator;
	//gravity->setToGravity();
	//program->addOperator(gravity.get());

	// add a fluid operator to simulate air friction.
	//osg::ref_ptr<osgParticle::FluidFrictionOperator> fluid = new osgParticle::FluidFrictionOperator;
	//fluid->setFluidToAir();
	//program->addOperator(fluid.get());

	// add a motion operator to simulate particle moving.
	osg::ref_ptr<MushroomOperator> motion = new MushroomOperator;
	motion->setCenter(position);
	program->addOperator(motion);


	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> particleUpdater = new osgParticle::ParticleSystemUpdater;
	particleUpdater->addParticleSystem(particleSystem.get());
	root->addChild(particleUpdater.get());

	if (useLocalParticleSystem)
	{
		particleSystem->setParticleScaleReferenceFrame(osgParticle::ParticleSystem::LOCAL_COORDINATES);

		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(particleSystem.get());
		root->addChild(geode.get());
	}

	return root;
}
