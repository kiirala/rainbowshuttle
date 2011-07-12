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

int ScreenWidth = 800;
int ScreenHeight = 480;

const int	spriteCount  = 3000;
const float	vertexBufferScale = 10.0f;

/** My Application */
class MyApplication : public framework::App {
private:
  core::Ref< render::Camera > camera;	// objects are rendered via camera 
  core::Ref< render::Mesh > mesh; // mesh is a 3D object
  core::Ref< render::Background> background;
  core::Ref<HAL::Input> inputButtons;
  core::Ref<HAL::Input> inputAccelerometer;
  slm::mat4 cameraTransform; // camera position
  float	totalTime;

public:
  /** Application constructor */
  MyApplication()
    : cameraTransform(1.0f),totalTime(0.0f)
  {
  }

  /** Application destructor */
  ~MyApplication() {
  }

  /** render method includes all drawing functionalities */
  void render(render::Graphics3D* g3d) {
    g3d->bindTarget(0); // bind graphics device
    g3d->clear(background); // clear the display

    g3d->setCamera(camera,cameraTransform); // set active 3D-camera
    //g3d->render(mesh, slm::mat4(1.0f));	// render 3D-object
    slm::vec3 a(0, 0, 0);
    slm::vec3 b(10 * sinf(totalTime), 10, 0);
    slm::vec3 c(-10, -10, 0);
    g3d->drawLine(a, b, 0xffffff);
    g3d->drawLine(a, c, 0xff);
    slm::vec3 center(0, 0, 0);
    float x = inputAccelerometer->readInput(HAL::Input::AXIS_X);
    float y = inputAccelerometer->readInput(HAL::Input::AXIS_Y);
    float z = inputAccelerometer->readInput(HAL::Input::AXIS_Z);
    slm::vec3 dir(y * 10,
		  -x * 10,
		  0.0);
    g3d->drawLine(center, dir, 0xffffff);
    
    g3d->releaseTarget(); // release graphics device
  }

  /** update method updates your application logic */
  bool update(float dt ) {
    totalTime += dt;

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
    /*
    camera->setPerspective(60.0f,
			   ScreenWidth / float(ScreenHeight),
			   0.1f,
			   10000.0f);
    */
    camera->setParallel(-ScreenWidth / 2, ScreenWidth / 2,
			-ScreenHeight / 2, ScreenHeight / 2,
			-1, 1);

    // create a background
    background = KAJAK3D_NEW render::Background();
    assert(background);
    background->setColor(0xFF0000);

    inputButtons = HAL::Input::createInputHandle(HAL::Input::INPUT_DEVICE_KEYBOARD);
    inputAccelerometer = HAL::Input::createInputHandle(HAL::Input::INPUT_DEVICE_ACCELEROMETER);

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
