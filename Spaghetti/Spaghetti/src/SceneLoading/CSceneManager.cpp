#include "CSceneManager.h"

/*
*	\brief
*/
CSceneManager::CSceneManager()
{
	m_currentScene = 0;
	m_selectedNode = nullptr;
}

/*
*	\brief
*/
CSceneManager::~CSceneManager()
{
	const unsigned int noofScenes = m_scene.size();
	for (unsigned int sceneIndex = 0; sceneIndex < noofScenes; ++sceneIndex)
		delete m_scene[sceneIndex];

	m_scene.clear();
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

	if (m_scene.size())
	{
		Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

		// Create a panel
		Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "FullScreenPanel"));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(0, 0);
		panel->setDimensions(1.0, 1.0);

		// Create an overlay, and add the panel
		Ogre::Overlay* overlay = overlayManager.create("FullOverlay");
		overlay->add2D(panel);

		int yOffset = 10;
		m_infoPanel[PanelInformation::WorldInformation].name = "World-Information";
		m_infoPanel[PanelInformation::SelectBodyInformation].name = "Select-Body-Information";
		m_infoPanel[PanelInformation::Help].name = "Help-Information";

		UpdatePanelInformation(nullptr);

		m_infoPanel[PanelInformation::Help].content << "Help\n---------------------------------------\n" << "Return Key -> (Un)Pause World\n" << "Keys 1 - 9 -> Change Scene\n" << "WASD/Arrows -> Move Camera\n" << "Mouse Move -> Rotate Camera\n" <<
			"Mouse Click -> Select Body Under Cursor";

		for (unsigned int panelIndex = 0; panelIndex < PanelInformation::Noof; ++panelIndex)
		{
			// Create a text area
			m_infoPanel[panelIndex].textArea = static_cast<Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", m_infoPanel[panelIndex].name));
			m_infoPanel[panelIndex].textArea->setMetricsMode(Ogre::GMM_PIXELS);
			m_infoPanel[panelIndex].textArea->setPosition(10, yOffset);
			m_infoPanel[panelIndex].textArea->setDimensions(1.0, 1.0);
			m_infoPanel[panelIndex].textArea->setCaption(m_infoPanel[panelIndex].content.str());
			m_infoPanel[panelIndex].textArea->setCharHeight(16);
			m_infoPanel[panelIndex].textArea->setFontName("StarWars");
			m_infoPanel[panelIndex].textArea->setColourBottom(Ogre::ColourValue(0.3, 0.5, 0.3));
			m_infoPanel[panelIndex].textArea->setColourTop(Ogre::ColourValue(0.5, 0.7, 0.5));

			yOffset += 200;

			// Add the text area to the panel
			panel->addChild(m_infoPanel[panelIndex].textArea);
		}

		// Show the overlay
		overlay->show();
	}

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
	CScene *const scene = m_scene[m_currentScene];
	if (scene == nullptr)
		return;

	scene->CheckForUpdate();

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

void CSceneManager::Update(
		CSpaghettiWorld *world
	)
{
	UpdatePanelInformation(world);
	m_infoPanel[PanelInformation::WorldInformation].textArea->setCaption(m_infoPanel[PanelInformation::WorldInformation].content.str());

	const unsigned int noofBodies = m_body.size();
	for (unsigned int bodyIndex = 0; bodyIndex < noofBodies; ++bodyIndex)
	{
		CSpaghettiRigidBody *const body = m_body[bodyIndex];

		Ogre::SceneNode *const node = static_cast<Ogre::SceneNode*>(body->GetRenderObject());
		Ogre::Vector3 position = body->GetPosition();
		Ogre::Quaternion orientation = body->GetOrientation();

		if (m_selectedNode == body->GetRenderObject())
		{
			// draw bounds
			if (body->GetBounds()->GetType() == BoundsType::Sphere)
			{
				CSpaghettiBoundsSphere *sphere = static_cast<CSpaghettiBoundsSphere*>(body->GetBounds());
				DebugDrawer::getSingleton().drawSphere(sphere->GetPosition(), sphere->GetRadius(), Ogre::ColourValue::Red, false);
			}
			else if (body->GetBounds()->GetType() == BoundsType::Box)
			{
				CSpaghettiBoundsBox *box = static_cast<CSpaghettiBoundsBox*>(body->GetBounds());
				Ogre::Vector3 boxCorners[NOOF_BOUNDINGBOX_CORNERS];
				for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
				{
					boxCorners[cornerIndex] = body->GetPosition() + box->GetCorner(cornerIndex);
				}

				Ogre::Vector3 min = box->GetMin();
				Ogre::Vector3 max = box->GetMax();

				Ogre::Vector3 aaBoxCorners[NOOF_BOUNDINGBOX_CORNERS];
				aaBoxCorners[0] = body->GetPosition() + Ogre::Vector3(min.x, min.y, min.z);
				aaBoxCorners[1] = body->GetPosition() + Ogre::Vector3(min.x, max.y, min.z);
				aaBoxCorners[2] = body->GetPosition() + Ogre::Vector3(max.x, max.y, min.z);
				aaBoxCorners[3] = body->GetPosition() + Ogre::Vector3(max.x, min.y, min.z);
				aaBoxCorners[4] = body->GetPosition() + Ogre::Vector3(max.x, max.y, max.z);
				aaBoxCorners[5] = body->GetPosition() + Ogre::Vector3(min.x, max.y, max.z);
				aaBoxCorners[6] = body->GetPosition() + Ogre::Vector3(min.x, min.y, max.z);
				aaBoxCorners[7] = body->GetPosition() + Ogre::Vector3(max.x, min.y, max.z);

				DebugDrawer::getSingleton().drawCuboid(boxCorners, Ogre::ColourValue::Red, false);
				DebugDrawer::getSingleton().drawCuboid(aaBoxCorners, Ogre::ColourValue::Green, false);
			}

			m_infoPanel[PanelInformation::SelectBodyInformation].textArea->setCaption(m_infoPanel[PanelInformation::SelectBodyInformation].content.str());
		}

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
	m_selectedNode = nullptr;

	UpdatePanelInformation(world);
	m_infoPanel[PanelInformation::WorldInformation].textArea->setCaption(m_infoPanel[PanelInformation::WorldInformation].content.str());
	m_infoPanel[PanelInformation::SelectBodyInformation].textArea->setCaption(m_infoPanel[PanelInformation::SelectBodyInformation].content.str());

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

void CSceneManager::UpdatePanelInformation(
		CSpaghettiWorld *world
	)
{
	m_infoPanel[PanelInformation::WorldInformation].content.str(std::string());
	m_infoPanel[PanelInformation::WorldInformation].content << "World Information\n---------------------------------------\n";
	if (world != nullptr)
	{
		m_infoPanel[PanelInformation::WorldInformation].content << "Gravity: " << world->GetGravity() << "\n";
		m_infoPanel[PanelInformation::WorldInformation].content << "Paused: " << (world->IsPaused() ? "True" : "False") << "\n";
		m_infoPanel[PanelInformation::WorldInformation].content << "Number of Bodies: " << m_body.size() << "\n";
	}

	m_infoPanel[PanelInformation::SelectBodyInformation].content.str(std::string());
	m_infoPanel[PanelInformation::SelectBodyInformation].content << "Body Information\n---------------------------------------\n";
	if (m_selectedNode != nullptr)
	{
		m_infoPanel[PanelInformation::SelectBodyInformation].content << "Name: " << m_selectedNode->getName() << "\n";
		m_infoPanel[PanelInformation::SelectBodyInformation].content << "Position: " << m_selectedNode->getPosition() << "\n";
		m_infoPanel[PanelInformation::SelectBodyInformation].content << "Orientation: " << m_selectedNode->getOrientation() << "\n";

		CSpaghettiRigidBody *body = BodyFromScreneNode(m_selectedNode);
		if (body != nullptr)
		{
			m_infoPanel[PanelInformation::SelectBodyInformation].content << "Static: " << (body->IsStatic() ? "True" : "False") << "\n";
			m_infoPanel[PanelInformation::SelectBodyInformation].content << "Mass: " << body->GetMass() << "\n";
			m_infoPanel[PanelInformation::SelectBodyInformation].content << "Velocity: " << body->GetVelocity() << "\n";
			m_infoPanel[PanelInformation::SelectBodyInformation].content << "Angular Velocity: " << body->GetAngularVelocity() << "\n";
		}
	}
}

CSpaghettiRigidBody * CSceneManager::BodyFromScreneNode( 
		Ogre::SceneNode* node 
	)
{
	const unsigned int noofBodies = m_body.size();
	for (unsigned int bodyIndex = 0; bodyIndex < noofBodies; ++bodyIndex)
	{
		CSpaghettiRigidBody *const body = m_body[bodyIndex];
		if (m_selectedNode == body->GetRenderObject())
			return body;
	}

	return nullptr;
}

