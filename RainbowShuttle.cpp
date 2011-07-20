#include <framework/kajak3D.h>
#include <render/OGLES/OGLES1_Graphics3D.h>
#include <render/OGLES/OGLES2_Graphics3D.h>
#include <render/Mesh.h>
#include <render/VertexBuffer.h>
#include <render/Camera.h>
#include <render/Background.h>
#include <render/RenderMathHelpers.h>
#include <render/PointListArray.h>

#include <HAL/Input.h>
#include <HAL/File.h>
#include <render/Loader_util.h>
#include <render/World.h>
#include <core/FileSystem.h>
#include <core/FileArchive.h>

#ifdef IPHONE
#include <core/TARArchive.h>
#endif

// required for Android devices
// ApkArchive doesn't exist on other platforms.
#if defined(ANDROID)
#include <core/ApkArchive.h>
#endif

#include "Shuttle.h"

int ScreenWidth = 800;
int ScreenHeight = 480;

const int	spriteCount  = 3000;
const float	vertexBufferScale = 10.0f;

/** My Application */
class MyApplication : public framework::App {
private:
  core::Ref< render::Camera > camera;	// objects are rendered via camera 
  core::Ref< render::Background> background;
  core::Ref<HAL::Input> inputButtons;
  core::Ref<HAL::Input> inputAccelerometer;
  core::Ref<render::World> world;
  core::Ref<Shuttle> shuttle;
  float	totalTime;

public:
  /** Application constructor */
  MyApplication()
    : totalTime(0.0f)
  {
    inputButtons =
      HAL::Input::createInputHandle(HAL::Input::INPUT_DEVICE_KEYBOARD);
    inputAccelerometer =
      HAL::Input::createInputHandle(HAL::Input::INPUT_DEVICE_ACCELEROMETER);
  }

  /** Application destructor */
  ~MyApplication() {
  }

  /** render method includes all drawing functionalities */
  void render(render::Graphics3D* g3d) {
    g3d->bindTarget(0); // bind graphics device
    g3d->clear(background); // clear the display

//    slm::vec3 forward(downUnit.x, -downUnit.z, -downUnit.y);
//    render::Camera* camera = world->getActiveCamera();
//    slm::vec3 cameraSide(slm::cross(forward, slm::vec3(0, 1, 0)));
//    slm::mat4 cameraTrans(
//    		slm::vec4(cameraSide, 0),
//    		slm::vec4(slm::cross(cameraSide, forward), 0),
//    		slm::vec4(-forward, 0),
//    		slm::vec4(forward * -10, 1));
//    camera->setTransform(cameraTrans);

    render::Camera* camera = world->getActiveCamera();
    slm::mat4 orientation(shuttle->orientation);
    slm::vec4 forward = orientation * slm::vec4(0, 1, 0, 0);
    slm::vec4 side = orientation * slm::vec4(1, 0, 0, 0);
    slm::vec4 up = orientation * slm::vec4(0, 0, 1, 0);
    slm::vec3 cameraPos(shuttle->position - 10 * forward.xyz());
//    slm::mat4 cameraTrans(
//    		slm::vec4(side.xyz(), cameraPos.x),
//    		slm::vec4(up.xyz(), cameraPos.y),
//    		slm::vec4(-forward.xyz(), cameraPos.z),
//    		slm::vec4(0, 0, 0, 1));
//    camera->setTransform(slm::transpose(cameraTrans));
//    slm::mat4 cameraTrans(
//    		side, up, forward, slm::vec4(cameraPos, 1));
//    camera->setTransform(cameraTrans);
    camera->setTransform(slm::inverse(slm::lookAtRH(cameraPos, shuttle->position, up.xyz())));
    g3d->render(world);

    g3d->releaseTarget(); // release graphics device
  }

  /** update method updates your application logic */
  bool update(float dt ) {
    totalTime += dt;

    float x = inputAccelerometer->readInput(HAL::Input::AXIS_X);
    float y = inputAccelerometer->readInput(HAL::Input::AXIS_Y);
    float z = inputAccelerometer->readInput(HAL::Input::AXIS_Z);
    slm::vec3 down(-y, x, z);
    if (x != 0 || y != 0 || z != 0) {
        slm::vec3 downUnit = slm::normalize(down);

        shuttle->rotate(-asinf(downUnit.x) * dt, -asinf(downUnit.z) * dt);
        shuttle->update(dt);
    }

    if (inputButtons->readInput(HAL::Input::KEY_BACK) != .0f)
      return false;
    return true;
  }

  /** this method is called when new graphic device is created */
  void createDevice(render::Graphics3D* g3d) {
    // configure file system 
    core::FileSystem* fs = core::FileSystem::get();
    assert(fs);

    // if this is Android device, we must use ApkArchive type
#if defined( ANDROID )
    fs->addArchive( "", KAJAK3D_NEW core::ApkArchive( HAL::File::getWorkingDirectory(), "assets/media/") );
#elif defined( IPHONE )
    fs->addArchive( "", KAJAK3D_NEW core::FileArchive( HAL::File::getWorkingDirectory(), false) );
    fs->addArchive( "", KAJAK3D_NEW core::TARArchive( "media.tar" ));
#else
    fs->addArchive( "", KAJAK3D_NEW core::FileArchive( HAL::File::getWorkingDirectory() + "media/", false ) );
#endif

    // create a 3D-camera
    camera = KAJAK3D_NEW render::Camera();

    camera->setPerspective(60.0f,
			   ScreenWidth / float(ScreenHeight),
			   0.1f,
			   10000.0f);

/*
    camera->setParallel(-ScreenWidth / 2, ScreenWidth / 2,
			-ScreenHeight / 2, ScreenHeight / 2,
			-1, 1);
*/

    core::Vector< core::Ref< render::Object3D > > objects;
    render::Loader::load( g3d, "shuttle.m3g", objects );
    core::Ref<render::World> shuttleWorld =
    		dynamic_cast<render::World*>(objects[0].ptr());
    assert(shuttleWorld);
    core::Ref<render::Group> shuttleModel = KAJAK3D_NEW render::Group();
    for (int i = 0 ; i < shuttleWorld->getChildCount() ; ++i) {
    	render::Mesh* modelMesh =
    			dynamic_cast<render::Mesh*>(shuttleWorld->getChild(i));
    	if (modelMesh) {
    		shuttleModel->addChild(modelMesh);
    	}
    }
    shuttle = KAJAK3D_NEW Shuttle(shuttleModel);

    // create a background
    background = KAJAK3D_NEW render::Background();
    assert(background);
    background->setColor(0xFF);

    core::Vector< core::Ref< render::Object3D > > objectsLevel;
    render::Loader::load(g3d, "level-test.m3g", objectsLevel);
    world = dynamic_cast<render::World*>(objectsLevel[0].ptr());
    assert(world);

//    world = KAJAK3D_NEW render::World();
//    core::Ref<render::Light> light = KAJAK3D_NEW render::Light();
//    light->translate(5, 5, 5);
//    world->addChild(light);
//    world->setBackground(background);
    world->addChild(shuttle->model);
//    core::Ref<render::Camera> camera = KAJAK3D_NEW render::Camera();
//    world->setActiveCamera(camera);
//    world->addChild(camera);

    // print info about Graphics
    const render::Graphics3DProperties& properties = g3d->getProperties();
    core::Debug::printf(core::Debug::APPLICATION,"Max point size size is = %d pixels\n",properties.maxPointSize);
    core::Debug::printf(core::Debug::APPLICATION,"Min point size size is = %d pixels\n",properties.minPointSize);
  }

  /** this method is called if application lost graphic device */
  void lostDevice() {
  }

  /** this method is called if application gets a new graphic device and graphics device is reconfigured */
  void resetDevice(render::Graphics3D* g3d) {
    (void)g3d;
  }

private:
};

/** Kajak3D main function which creates new framework object, configures the engine and creates an application */
framework::Kajak3DFramework* kajak3DMain(core::Kajak3DConfig* config) {
  // configure the engine
  // in Android device resolution is pre-set in config::HALConfig, 
  // and we don't want to overwrite it.
#ifndef WIN32
  ScreenWidth = config->HALConfig.displayWidth;
  ScreenHeight = config->HALConfig.displayHeight;
#else
  config->displayConfig.width		  = ScreenWidth;
  config->displayConfig.height	  = ScreenHeight;
#endif
  config->engineConfig.debugEnabled = true;
  config->graphicsConfig.cameraLightEnabled = false;
  // create new framework
  framework::Kajak3DFramework* fw = new framework::Kajak3DFramework(config);
  fw->setGraphics3D(KAJAK3D_NEW render::OGLES1_Graphics3D()); // create new graphics for framework
  fw->setApp(KAJAK3D_NEW MyApplication()); // create new application for framework
  return fw;
}
