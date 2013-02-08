#include "CScene.h"

/*
*	\brief Class constructor
*/
CScene::CScene()
{

}

/*
*	\brief Class destructor
*/
CScene::~CScene()
{

}

/*
*	\brief Parse a scene file
*/
const bool CScene::Parse( 
		std::string sceneFileLocation					//!< The location of the scene file to parse
	)
{

	m_fileLocation = sceneFileLocation;

	std::ifstream filestream;
	filestream.open(sceneFileLocation);

	if (!filestream.is_open())
		return false;

	m_data.clear();

	NodeMap *currentNodeMap = new NodeMap;
	m_data.push_back(currentNodeMap);

	while (!filestream.eof())
	{
		std::string fileLine;
		getline(filestream, fileLine);
		ParseLine(&currentNodeMap, fileLine);
	}

	filestream.close();

	// store the last edit time of the file
	struct stat attrib;			
	stat(sceneFileLocation.c_str(), &attrib);

	tm tempTime;
	gmtime_s(&tempTime, &attrib.st_mtime);

	m_editTime = mktime(&tempTime);

	return true;
}

/*
*	\brief Parse a line of the scene file
*/
void CScene::ParseLine( 
		NodeMap **nodeMap,								//!< The current node map we are parsing the line for
		std::string line								//!< The line to parse 
	)
{
	NodeMap *nm = (*nodeMap);

	// to start with look for the <NAME> tag
	std::smatch nameMatch;
	if (std::regex_search(line, nameMatch, std::regex("(<)(.*?)(>)"))) //(?<=<)(.*?)(?=>)
	{
		if ((*nm).count("name") != 0)
		{
			NodeMap *newNode = new NodeMap;
			m_data.push_back(newNode);
			nm = newNode;
			(*nodeMap) = newNode;
		}

		(*nm)["name"] = line.substr(1, line.length() - 2);
		return;
	}

	// ok we aren't looking at a new node name. continue loading data for the current node
	size_t colonIndex = line.find(":");
	if (colonIndex == std::string::npos)
		return;

	std::string dataKey = line.substr(0, colonIndex);
	std::string dataValue = line.substr(colonIndex + 1);
	(*nm)[dataKey] = dataValue;
}

/*
*	\brief Get the number of nodes in the scene
*/
const unsigned int CScene::GetNoofNodes() const
{
	return m_data.size();
}

/*
*	\brief Get a node by a given index
*/
NodeMap *CScene::GetNodeMap( 
		unsigned int nodeIndex									//!< The index of the node to get
	) const
{
	return m_data[nodeIndex];
}

/*
*	\brief Check to see if the scene has been edited, and re-parse if it has
*/
void CScene::CheckForUpdate()
{
	// store the last edit time of the file
	struct stat attrib;			
	stat(m_fileLocation.c_str(), &attrib);

	tm tempTime;
	gmtime_s(&tempTime, &attrib.st_mtime);

	time_t lastEditIme = mktime(&tempTime);

	if (difftime(m_editTime, lastEditIme) != 0)
	{
		const unsigned int noodDataNodes = m_data.size();
		for (unsigned int dataIndex = 0; dataIndex < noodDataNodes; ++dataIndex)
		{
			delete m_data[dataIndex];
		}
		m_data.clear();

		Parse(m_fileLocation);
	}
}
