#include "CScene.h"

CScene::CScene()
{

}

CScene::~CScene()
{

}

const bool CScene::Parse( 
		std::string sceneFileLocation					//!< The location of the scene file to parse
	)
{

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
	return true;
}

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

	// ok we arn't looking at a new node name. continue loading data for the current node
	size_t colonIndex = line.find(":");
	if (colonIndex == std::string::npos)
		return;

	std::string dataKey = line.substr(0, colonIndex);
	std::string dataValue = line.substr(colonIndex + 1);
	(*nm)[dataKey] = dataValue;
}
