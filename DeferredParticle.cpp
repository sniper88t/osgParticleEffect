// DeferredParticle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#include <osg/Group>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/io_utils>

#include <osgUtil/Optimizer>

#include <osgDB/ReadFile>

#include <osgParticle/ExplosionEffect>
#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/SmokeTrailEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/ParticleSystemUpdater>
#include <osg/Camera>
#include <osg/LightSource>

#include "Explosion.h"

osg::Vec3 computeTerrainIntersection(osg::Node* subgraph, float x, float y)
{
	const osg::BoundingSphere& bs = subgraph->getBound();
	float zMax = bs.center().z() + bs.radius();
	float zMin = bs.center().z() - bs.radius();

	osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
		new osgUtil::LineSegmentIntersector(osg::Vec3(x, y, zMin), osg::Vec3(x, y, zMax));

	osgUtil::IntersectionVisitor iv(intersector.get());

	subgraph->accept(iv);

	if (intersector->containsIntersections())
	{
		return intersector->getFirstIntersection().getWorldIntersectPoint();
	}

	return osg::Vec3(x, y, 0.0f);
}

//////////////////////////////////////////////////////////////////////////////
// MAIN SCENE GRAPH BUILDING FUNCTION
//////////////////////////////////////////////////////////////////////////////

void build_world(osg::Group* root)
{
	osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
	ls->getLight()->setPosition(osg::Vec4(1, -1, 1, 0)); // make 4thcoord 1 for point
	ls->getLight()->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1.0));
	ls->getLight()->setDiffuse(osg::Vec4(0.7, 0.4, 0.6, 1.0));
	ls->getLight()->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	root->addChild(ls.get());

    osg::Geode* terrainGeode = new osg::Geode;
    // create terrain
    {
        osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
        osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile("Images/lz.rgb");
        if (image)
        {
            osg::Texture2D* texture = new osg::Texture2D;
            texture->setImage(image);
            stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
        }

        terrainGeode->setStateSet(stateset);

        float size = 100; // 10km;
        float scale = size / 39.0f; // 10km;
        float z_scale = scale * 3.0f;

        osg::HeightField* grid = new osg::HeightField;
        grid->allocate(38, 39);
        grid->setXInterval(scale);
        grid->setYInterval(scale);

        terrainGeode->addDrawable(new osg::ShapeDrawable(grid));
        /*
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);

        terrainGeode->setStateSet(stateset);


		float radius = 0.8f;
		float height = 1.0f;

		osg::TessellationHints* hints = new osg::TessellationHints;
		hints->setDetailRatio(0.5f);

        terrainGeode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 2 * radius), hints));
        */

        root->addChild(terrainGeode);
    }

    {
		osg::Vec3 center(0.0f, 0.0f, 0.0f);
		float radius = 100.0f;

		osg::Vec3 position = center +
			osg::Vec3(radius * (((float)rand() / (float)RAND_MAX) - 0.5) * 2.0,
				radius * (((float)rand() / (float)RAND_MAX) - 0.5) * 2.0,
				0.0f);
        position = { 10.0f, 10.0f, 0.0f };

		root->addChild(getExplosion(position));


    }

	//osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//geode->addDrawable(ps.get());
	//root->addChild(geode.get());

}

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:

	PickHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::PUSH):
		{
			osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			pick(viewer, ea);
		}
		return false;

		default:
			return false;
		}
	}

	void pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea)
	{
		osg::Group* root = dynamic_cast<osg::Group*>(viewer->getSceneData());
		if (!root) return;

		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (viewer->computeIntersections(ea, intersections))
		{
			const osgUtil::LineSegmentIntersector::Intersection& hit = *intersections.begin();

			bool handleMovingModels = false;
			const osg::NodePath& nodePath = hit.nodePath;
			for (osg::NodePath::const_iterator nitr = nodePath.begin();
				nitr != nodePath.end();
				++nitr)
			{
				const osg::Transform* transform = dynamic_cast<const osg::Transform*>(*nitr);
				if (transform)
				{
					if (transform->getDataVariance() == osg::Object::DYNAMIC) handleMovingModels = true;
				}
			}

			osg::Vec3 position = handleMovingModels ? hit.getLocalIntersectPoint() : hit.getWorldIntersectPoint();

			{
				// when we don't have moving models we can simple insert the particle effect into the root of the scene graph
				root->addChild(getExplosion(position));
			}

		}
	}

protected:
	virtual ~PickHandler() {}
};


//////////////////////////////////////////////////////////////////////////////
// main()
//////////////////////////////////////////////////////////////////////////////

int main()
{
    // construct the viewer.
    osgViewer::Viewer viewer;

	// register the pick handler
	viewer.addEventHandler(new PickHandler());
	viewer.setUpViewInWindow(100, 100, 1300, 1300);

    osg::Group* root = new osg::Group;
    build_world(root);

    osgUtil::Optimizer optimizer;
    optimizer.optimize(root);

    // add a viewport to the viewer and attach the scene graph.
    viewer.setSceneData(root);

	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the window size toggle handler
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

    // add the state manipulator
    //viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	
	osg::ref_ptr<osg::Camera> myCam = new osg::Camera;
	myCam->setClearColor(osg::Vec4(0, 0, 0, 1)); // black background
	// set dimensions of the view volume
	myCam->setProjectionMatrixAsPerspective(35, 4.0 / 3.0, 0.1, 100);
	// set position and orientation of the viewer
	myCam->setViewMatrixAsLookAt(
		osg::Vec3 (180, 180, 80), // eye above xy-plane
		osg::Vec3(0, 0, 0), // gaze at origin
		osg::Vec3(0, 0, 1)); // usual up vector
	viewer.setCamera(myCam); // attach camera to the viewer
	viewer.realize();
	while (!viewer.done()) {
		viewer.frame();
	}


    return viewer.run();
}

