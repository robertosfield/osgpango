// -*-c++-*- Copyright (C) 2010 osgPango Development Team
// $Id$

#include <iostream>
#include <OpenThreads/Thread>
#include <osg/MatrixTransform>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgPairo/Text>

const std::string LOREM_IPSUM[] = {
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod ",
	"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, ",
	"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. ",
	"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu ",
	"fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in ",
	"culpa qui officia deserunt mollit anim id est laborum. "
};

class Thread: public OpenThreads::Thread {
public:
	Thread(osgPairo::Text* text, unsigned int sleep):
	_text  (text),
	_sleep (sleep) {
	}

	virtual void run() {
		for(unsigned int i = 1; i < 6; i++) {
			std::cout << "Sleeping: " << _sleep << std::endl;

			// sleep(_sleep);

			_text->setText(LOREM_IPSUM[i]);
		}
	}

private:
	osg::ref_ptr<osgPairo::Text> _text;

	unsigned int _sleep;
};

osg::Camera* createOrthoCamera(float width, float height) {
	osg::Camera* camera = new osg::Camera();

	camera->getOrCreateStateSet()->setMode(
		GL_LIGHTING,
		osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF
	);

	camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, width, 0.0f, height));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	return camera;
}

int main(int argc, char** argv) {
	osgViewer::Viewer viewer;

	OpenThreads::Thread::Init();

	osgPairo::Font::init();

	osgPairo::Font* f  = new osgPairo::Font("Sans 20");
	osgPairo::Text* t1 = new osgPairo::Text(f);
	osgPairo::Text* t2 = new osgPairo::Text(f);

	t1->setText(LOREM_IPSUM[0]);
	t2->setText(LOREM_IPSUM[0]);

	t1->setWidth(500);
	t2->setWidth(500);

	osg::Group*  group  = new osg::Group();
	osg::Camera* camera = createOrthoCamera(1280, 1024);

	osg::MatrixTransform* mt1 = new osg::MatrixTransform(
		osg::Matrix::translate(osg::Vec3(t1->getOriginTranslated(), 0.0f))
	);

	osg::MatrixTransform* mt2 = new osg::MatrixTransform(
		osg::Matrix::translate(osg::Vec3(t2->getOriginTranslated() + osg::Vec2(0.0f, 100.0f), 0.0f))
	);

	mt1->addChild(t1);
	mt2->addChild(t2);

        viewer.addEventHandler(new osgViewer::StatsHandler());
        viewer.addEventHandler(new osgViewer::WindowSizeHandler());
        viewer.addEventHandler(new osgGA::StateSetManipulator(
                viewer.getCamera()->getOrCreateStateSet()
        ));

	camera->addChild(mt1);
	camera->addChild(mt2);

	group->addChild(camera);

	viewer.setSceneData(group);

	Thread thread1(t1, 1);
	Thread thread2(t2, 3);

	thread1.start();
	thread2.start();

	viewer.run();

	osgPairo::Font::cleanup();
	osgPairo::Text::cleanup();

	return 0;
}
