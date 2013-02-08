#include "CSceneManager.h"

/*
*	\brief
*/
CSceneManager::CSceneManager()
{
	m_currentScene = 0;
}

/*
*	\brief
*/
CSceneManager::~CSceneManager()
{

}

/*
*	\brief
*/
bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

/*
*	\brief
*/
const int CSceneManager::LoadScenes( 
		std::string sceneFolder									//!< The folder to search for .scene files
	)
{
	size_t size = 0;
	char *cpath = nullptr;
	cpath = _getcwd(cpath, size);
	
	std::stringstream path;
	path << cpath << "\\" << sceneFolder << "\\";

	if (!dirExists(path.str()))
		return 0;

	path << "*";

	WIN32_FIND_DATA fileData;
	HANDLE file = FindFirstFile(path.str().c_str(), &fileData);

	if (file == INVALID_HANDLE_VALUE)
		return 0;

	do
	{
		std::string fileName = fileData.cFileName;
		if (fileName.rfind(".scene") == std::string::npos)
			continue;

		std::stringstream filePath;
		filePath << cpath << "\\" << sceneFolder << "\\" << fileName;

		CScene *newScene = new CScene();
		if (!newScene->Parse(filePath.str()))
		{
			delete newScene;
			continue;
		}

		m_scene.push_back(newScene);

	}
	while (FindNextFile(file, &fileData) != 0);

	return m_scene.size();
}

/*
*	\brief
*/
void CSceneManager::SetupCurrentScene( 
		OgreApplication *application, 
		CSpaghetti *spaghetti,
		CSpaghettiWorld *world
	)
{
	// get the current scene to load
	const CScene *const scene = m_scene[m_currentScene];
	if (scene == nullptr)
		return;

	// load in all the nodes of the scene
	const unsigned int noofNodes = scene->GetNoofNodes();
	for (unsigned int nodeIndex = 0; nodeIndex < noofNodes; ++nodeIndex)
	{
		NodeMap *const currentNodeMap = scene->GetNodeMap(nodeIndex);
		if (currentNodeMap == nullptr)
			continue;

		CreateNodeFromMap(application, spaghetti, world, currentNodeMap);
	}

}

/*
*	\brief
*/
void CSceneManager::CreateNodeFromMap(
		OgreApplication *application, 
		CSpaghetti *spaghetti,
		CSpaghettiWorld *world,
		NodeMap *nodeMap 
	)
{
	std::string nodeName = (*nodeMap)["name"];
	std::string meshName = (*nodeMap)["mesh"];

	if (Ogre::SceneNode *const newNode = application->CreateEntityFromMesh(meshName, nodeName))
	{
		// scale the node
		Ogre::Vector3 nodeScale = ParseVector3((*nodeMap)["scale"]);
		newNode->setScale(nodeScale);

		// get the body type to create
		std::string bodyTypeBuf = (*nodeMap)["collision"];
		RigidBodyType::Enum bodyType = RigidBodyType::Box;
		if (bodyTypeBuf.compare("box") == 0)
			bodyType = RigidBodyType::Box;
		else if (bodyTypeBuf.compare("sphere") == 0)
			bodyType = RigidBodyType::Sphere;

		CSpaghettiRigidBody *const body = spaghetti->CreateRigidBody(newNode, world, bodyType);
		if (body == nullptr)
			return;

		// add our new body to our list
		m_body.push_back(body);

		// set the initial position of the body
		Ogre::Vector3 position = ParseVector3((*nodeMap)["position"]);
		body->SetPosition(position);

		// set the initial rotation of the body
		Ogre::Vector3 rotation = ParseVector3((*nodeMap)["rotation"]);
		body->SetRotation(rotation);

		// set the mass of the object
		const float mass = static_cast<float>(::atof((*nodeMap)["mass"].c_str()));
		if (mass == 0) body->SetIsStatic(true); else body->SetMass(mass);

		// set the initial force
		Ogre::Vector3 force = ParseVector3((*nodeMap)["force"]);
		body->AddForceAtPoint(force, position);

		// set the initial torque
		Ogre::Vector3 torque = ParseVector3((*nodeMap)["torque"]);
		body->AddTorqueAtPoint(torque, position);
		
		// create the bounds
		if (bodyType == RigidBodyType::Box)
		{
			std::string entityName = meshName.substr(0, meshName.find("."));
			Ogre::Entity *const meshEntity = application->GetSceneManager()->getEntity(entityName);
			Ogre::AxisAlignedBox meshBoundingBox = meshEntity->getBoundingBox();

			Ogre::Vector3 boundingBoxCorners[NOOF_BOUNDINGBOX_CORNERS];
			const Ogre::Vector3 *const boundCorners = meshBoundingBox.getAllCorners();

			for (int corner = 0; corner < NOOF_BOUNDINGBOX_CORNERS; ++corner)
			{
				const Ogre::Vector3 currenCorner = boundCorners[corner];
				boundingBoxCorners[corner] = Ogre::Vector3(
					currenCorner.x * nodeScale.x, 
					currenCorner.y * nodeScale.y, 
					currenCorner.z * nodeScale.z
				);
			}

			CSpaghettiBoundsBox *const boundingBox = new CSpaghettiBoundsBox();
			boundingBox->SetCorners(boundingBoxCorners);
			body->SetBounds(boundingBox);
		}
		else if (bodyType == RigidBodyType::Sphere)
		{
			std::string entityName = meshName.substr(0, meshName.find("."));
			Ogre::Entity *const meshEntity = application->GetSceneManager()->getEntity(entityName);
			Ogre::AxisAlignedBox meshBoundingBox = meshEntity->getBoundingBox();
			float diameter = meshBoundingBox.getSize().x * nodeScale.x;

			CSpaghettiBoundsSphere *const boundingSphere = new CSpaghettiBoundsSphere();
			boundingSphere->SetDiameter(diameter);
			body->SetBounds(boundingSphere);
		}
	}
}

/*
*	\brief
*/
Ogre::Vector3 CSceneManager::ParseVector3( 
		std::string vectorbuffer										//!<
	)
{
	Ogre::Vector3 result = Ogre::Vector3::ZERO;

	int noofParts = 1;
	size_t startPos = 0, lastPos = 0;
	while((lastPos = vectorbuffer.find(",", lastPos + 1)) != std::string::npos)
	{
		std::string val = vectorbuffer.substr(startPos, lastPos - startPos);
		result[noofParts - 1] = static_cast<Ogre::Real>(::atof(val.c_str()));
		++noofParts;
		startPos = lastPos + 1;
	}

	if (noofParts != 3)
		return Ogre::Vector3(1, 1, 1);

	std::string val = vectorbuffer.substr(startPos, vectorbuffer.length() - startPos);
	result[noofParts - 1] = static_cast<Ogre::Real>(::atof(val.c_str()));

	return result;
}

void CSceneManager::Update()
{
	const unsigned int noofBodies = m_body.size();
	for (unsigned int bodyIndex = 0; bodyIndex < noofBodies; ++bodyIndex)
	{
		CSpaghettiRigidBody *const body = m_body[bodyIndex];

		Ogre::SceneNode *const node = static_cast<Ogre::SceneNode*>(body->GetRenderObject());
		Ogre::Vector3 position = body->GetPosition();
		Ogre::Quaternion orientation = body->GetOrientation();

		node->setPosition(position);
		node->setOrientation(orientation);
	}
}

void CSceneManager::SetCurrentScene( 
		int index,												//!< The index to set the current scene too
		OgreApplication *application,
		CSpaghetti *spaghetti,
		CSpaghettiWorld *world
	)
{
	m_currentScene = (index - 1) % m_scene.size();
	ReleaseActiveScene(application, spaghetti, world);
	SetupCurrentScene(application, spaghetti, world);
}

void CSceneManager::ReleaseActiveScene( 
		OgreApplication *const application, 
		CSpaghetti *const spaghetti, 
		CSpaghettiWorld *const world 
	)
{
	const unsigned int noofBodies = m_body.size();
	for (unsigned int bodyIndex = 0; bodyIndex < noofBodies; ++bodyIndex)
	{
		CSpaghettiRigidBody *const body = m_body[bodyIndex];
		Ogre::SceneNode *const node = static_cast<Ogre::SceneNode*>(body->GetRenderObject());
		application->DestroyNode(node);
	}
	application->GetSceneManager()->destroyAllEntities();

	world->Release();
	m_body.clear();
}

