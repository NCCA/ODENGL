#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>
#include "Geom.h"
#include "MeshData.h"



NGLScene::NGLScene()
{
  setTitle("ODE Physics with NGL");
  m_animate=true;
  m_randomPlace=false;
  m_bboxDraw=false;
  m_wireframe=false;

  m_physics.reset( new PhysicsWorld(PhysicsWorld::HASH));
  m_physics->setMaxContacts(32);
  m_physics->setGravity(ngl::Vec3(0,-3,0));
  m_physics->createGroundPlane(ngl::Vec4(0,1,0,0));
  m_physics->setWorldContactSurfaceLayer(0.001f);
  ngl::Random *rng=ngl::Random::instance();
  rng->setSeed();

}
void NGLScene::addCube()
{
  ngl::Random *rng=ngl::Random::instance();

  Geom g(m_physics->getSpace());
  g.createBox(1.0f,1.0f,1.0f);
  dMass mass;
  dMassSetBox(&mass,1.0,1.0,1.0,1.0);
  ngl::Vec3 pos=rng->getRandomVec3();
  pos*=10;
  pos.m_y=10;
  if(m_randomPlace == false)
    pos.set(0.0f,10.0f,0.0f);
  RigidBody b1(m_physics->getWorld(),pos);
  b1.addGeom(g);
  b1.setMass(mass);
  b1.setLinearVelocity(0.0f,0.1f,0.0f);//ngl::Vec3(0,-rng->randomNumber(30),0));
  b1.setAngularVelocity(rng->getRandomVec3());
  //b1.setTorque(rng->getRandomVec3());
  m_physics->addBody(b1);

}

void NGLScene::addSphere()
{
  ngl::Random *rng=ngl::Random::instance();

  Geom g(m_physics->getSpace());
  g.createSphere(0.5);
  // this is the inertia matrix will use a helper function
  dMass mass;
  dMassSetSphere(&mass,1,0.5);
  ngl::Vec3 pos=rng->getRandomVec3();
  pos*=10;
  pos.m_y=10;
  if(m_randomPlace == false)
    pos.set(0,10,0);
  RigidBody b1(m_physics->getWorld(),pos);
  b1.addGeom(g);
  b1.setLinearVelocity(0,0.1,0.0);
  b1.setAngularVelocity(rng->getRandomVec3());
  b1.setMass(mass);
  m_physics->addBody(b1);

}


void NGLScene::addTeapot()
{
  ngl::Random *rng=ngl::Random::instance();

  Geom g(m_physics->getSpace());
  g.createMesh("teapot");
  dMass mass;
  ngl::BBox b=m_teapotMesh->getBBox();
  dMassSetBox(&mass,1,b.width(),b.height(),b.depth());
  ngl::Vec3 pos=rng->getRandomVec3();
  pos*=10;
  pos.m_y=10;
  if(m_randomPlace == false)
    pos.set(0,10,0);
  RigidBody b1(m_physics->getWorld(),pos);
  b1.setName("teapot");
  b1.addGeom(g);
  b1.setMass(mass);
  b1.setLinearVelocity(0,0.1,0.0);//rng->getRandomNormalizedVec3());//    ngl::Vec3(0,-rng->randomNumber(),0));
  //b1.setAngularVelocity(rng->getRandomNormalizedVec3());
  b1.setAngularVelocity(rng->getRandomVec3());
  m_physics->addBody(b1);

}

void NGLScene::addApple()
{
  ngl::Random *rng=ngl::Random::instance();

  Geom g(m_physics->getSpace());
  g.createMesh("apple");
  // this is the inertia matrix will use a helper function
  dMass mass;
  //dMassSetSphere(&mass,0.1+rng->randomPositiveNumber(28.0),1.0);
  ngl::BBox b=m_appleMesh->getBBox();
  dMassSetSphere(&mass,1,m_appleMesh->getSphereRadius());

  ngl::Vec3 pos=rng->getRandomVec3();
  pos*=10;
  pos.m_y=10;
  if(m_randomPlace == false)
    pos.set(0,10,0);
  RigidBody b1(m_physics->getWorld(),pos);
  b1.setName("apple");

  b1.addGeom(g);
  b1.setMass(mass);
  b1.setLinearVelocity(0,0.1,0.0);//rng->getRandomNormalizedVec3());//    ngl::Vec3(0,-rng->randomNumber(),0));
  b1.setAngularVelocity(rng->getRandomVec3());
  m_physics->addBody(b1);

}



NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL( int _w, int _h )
{
  m_cam.setShape( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}

void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::instance();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglToonShader"]->use();
  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

  (*shader)["nglDiffuseShader"]->use();
  shader->setUniform("Colour",1.0f,1.0f,0.0f,1.0f);
  shader->setUniform("lightPos",1.0f,1.0f,1.0f);
  shader->setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);

  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,5,20);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  // now load to our new camera
  m_cam.set(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(50,(float)720.0/576.0,0.05,350);
  //shader->setShaderParam3f("viewerPos",m_cam->getEye().m_x,m_cam->getEye().m_y,m_cam->getEye().m_z);
  // now create our light this is done after the camera so we can pass the
  // transpose of the projection matrix to the light to do correct eye space
  // transformations
  //ngl::Mat4 iv=m_cam->getViewMatrix();
  //iv.transpose();
  //m_light = new ngl::Light(ngl::Vec3(-2,5,2),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT );
  //m_light->setTransform(iv);
  // load these values to the shader as well
  //m_light->loadToShader("light");
  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  prim->createSphere("sphere",0.5,40);
 // prim->createTrianglePlane("plane",140,140,140,140,ngl::Vec3(0,1,0));
  prim->createLineGrid("plane",140,140,80);
  m_teapotMesh.reset(  new ngl::Obj("models/teapot.obj"));
  m_teapotMesh->createVAO();

  m_appleMesh.reset (new ngl::Obj("models/apple.obj"));
  m_appleMesh->createVAO();
  m_appleMesh->calcBoundingSphere();
  // we need to add the  mesh to the collision data for ODE
  // this copies the verts and indices to a global mesh
  // singleton and ODE will use them
  ngl::Obj *collisionMesh = new ngl::Obj("models/teapotCollisionMesh.obj");

  MeshData *mesh=MeshData::instance();
  mesh->addMesh("teapot",collisionMesh);
  delete collisionMesh;
  collisionMesh = new ngl::Obj("models/appleCollisionMesh.obj");
  mesh->addMesh("apple",collisionMesh);
  delete collisionMesh;

 startTimer(10);
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_globalTransformMatrix*m_bodyTransformMatrix;
  MV=  m_cam.getViewMatrix()*M;
  MVP= m_cam.getVPMatrix()*M;
  normalMatrix=MV;
  normalMatrix.inverse().transpose();
  shader->setUniform("MVP",MVP);
  shader->setUniform("normalMatrix",normalMatrix);

}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglDiffuseShader"]->use();

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  // multiply the rotations
  m_globalTransformMatrix=rotY*rotX;
  // add the translations
  m_globalTransformMatrix.m_m[3][0] = m_modelPos.m_x;
  m_globalTransformMatrix.m_m[3][1] = m_modelPos.m_y;
  m_globalTransformMatrix.m_m[3][2] = m_modelPos.m_z;

   // get the VBO instance and draw the built in teapot
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // draw
  std::vector<RigidBody>::iterator begin=m_physics->begin();
  std::vector<RigidBody>::iterator end=m_physics->end();
  //shader->setShaderParam4f("Colour",1,1,0,1);
  while(begin !=end)
  {

    if(m_wireframe)
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    m_bodyTransformMatrix= begin->getTransformMatrix();
    loadMatricesToShader();
    if(begin->getGeoType() == BOX )
    {
      shader->setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
      prim->draw("cube");
    }
    else if(begin->getGeoType() == SPHERE)
    {
      shader->setUniform("Colour",0.0f,0.0f,1.0f,1.0f);
      prim->draw("sphere");
    }
    else if(begin->getGeoType() == TRIANGLEMESH)
    {
      shader->setUniform("Colour",1.0f,1.0f,0.0f,1.0f);

      begin->setMeshTransfomation();
      if(begin->getName()=="teapot")
      {
        m_teapotMesh->draw();
        if(m_bboxDraw)
          m_teapotMesh->drawBBox();
      }
      else if(begin->getName() == "apple")
      {
        shader->setUniform("Colour",0.0f,0.6f,0.f,1.f);

        m_appleMesh->draw();
      }
    }
    ++begin;
  }
  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

  m_bodyTransformMatrix.identity();
  loadMatricesToShader();

  prim->draw("plane");

}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Space : toggleAnimation(); break;
  case Qt::Key_X : stepAnimation(); break;
  case Qt::Key_1 : addCube(); break;
  case Qt::Key_2 : addSphere(); break;
  case Qt::Key_3 : addTeapot(); break;
  case Qt::Key_4 : addApple(); break;

  case Qt::Key_B : toggleBBox(); break;
  case Qt::Key_R : toggleRandomPlace(); break;
  case Qt::Key_0 : resetSim(); break;
  case Qt::Key_Up : getPhysicsWorld()->addForceToAllBodies(ngl::Vec3(0,40,0)); break;
  case Qt::Key_Down : getPhysicsWorld()->addForceToAllBodies(ngl::Vec3(0,-40,0)); break;
  case Qt::Key_Left : getPhysicsWorld()->addForceToAllBodies(ngl::Vec3(-40,0,0)); break;
  case Qt::Key_Right : getPhysicsWorld()->addForceToAllBodies(ngl::Vec3(40,0,0)); break;
  case Qt::Key_I : getPhysicsWorld()->addForceToAllBodies(ngl::Vec3(0,0,-40)); break;
  case Qt::Key_O : getPhysicsWorld()->addForceToAllBodies(ngl::Vec3(0,0,40)); break;

  default : break;
  }
  //if (isExposed())
  update();
}

void NGLScene::timerEvent(QTimerEvent *)
{
  if(m_animate == true)
  {
    m_physics->doCollisions();
    m_physics->quickStep(0.05f);
    update();
  }
}
void NGLScene::stepAnimation()
{
  m_physics->doCollisions();
  m_physics->quickStep(0.05f);
  update();
}
//----------------------------------------------------------------------------------------------------------------------


