#include "Camera.h"

CCamera::CCamera(
		Ogre::SceneNode* node,
		Ogre::Camera* camera
	)
{
	m_sceneNode = node;
	m_camera = camera;

	m_rotation = Ogre::Vector3::ZERO;
}

CCamera::~CCamera(void)
{

}

void CCamera::Update( 
		OIS::Keyboard *kb, 
		OIS::Mouse *ms 
	)
{
	Ogre::Vector3 currentPosition = m_sceneNode->getPosition();
	Ogre::Quaternion currentRotation = m_sceneNode->getOrientation();

	Ogre::Vector3 right = Ogre::Vector3::UNIT_X;
	Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;
	Ogre::Vector3 look = Ogre::Vector3::UNIT_Z;

	right = currentRotation * right;
	up = currentRotation * up;
	look = currentRotation * look;

	float forwardScale = 0.0f;
	float sideScale = 0.0f;
	float verticalScale = 0.0f;

	if (kb->isKeyDown(OIS::KC_W) || kb->isKeyDown(OIS::KC_UP))
		forwardScale = 1.0f;
	else if (kb->isKeyDown(OIS::KC_S) || kb->isKeyDown(OIS::KC_DOWN))
		forwardScale = -1.0f;

	if (kb->isKeyDown(OIS::KC_D) || kb->isKeyDown(OIS::KC_RIGHT))
		sideScale = -1.0f;
	else if (kb->isKeyDown(OIS::KC_A) || kb->isKeyDown(OIS::KC_LEFT))
		sideScale = 1.0f;

	if (kb->isKeyDown(OIS::KC_Q) || kb->isKeyDown(OIS::KC_HOME))
		verticalScale = 1.0f;
	else if (kb->isKeyDown(OIS::KC_Z) || kb->isKeyDown(OIS::KC_END))
		verticalScale = -1.0f;

	currentPosition += (look * forwardScale);
	currentPosition += (up * verticalScale);
	currentPosition += (right * sideScale);

	m_sceneNode->setPosition(currentPosition);
	m_camera->lookAt(currentPosition + look);
	
	if (ms->getMouseState().X.abs > ms->getMouseState().width)
		return;

	if (ms->getMouseState().Y.abs > ms->getMouseState().height)
		return;

	const int msX = ms->getMouseState().X.rel;
	const int msY = ms->getMouseState().Y.rel;

	m_rotation.x += msX;
	m_rotation.y += msY;

	Ogre::Quaternion newRotationX, newRotationY;
	newRotationX.FromAngleAxis((Ogre::Radian)(m_rotation.x * -0.0017f), Ogre::Vector3::UNIT_Y);
	newRotationY.FromAngleAxis((Ogre::Radian)(m_rotation.y *  0.0017f), Ogre::Vector3::UNIT_X);

	m_sceneNode->setOrientation(newRotationX * newRotationY);
}
