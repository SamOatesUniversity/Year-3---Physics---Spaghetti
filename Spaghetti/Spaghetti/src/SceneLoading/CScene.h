#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>

typedef std::map<std::string, std::string>	NodeMap;

class CScene {

private:

	std::vector<NodeMap*>					m_data;

private:

											//! Parse a line of the scene file
	void									ParseLine(
												NodeMap **nodeMap,									//!< The current node map we are parsing the line for
												std::string line									//!< The line to parse
											);

public:
											//! Class Constructor
											CScene();

											//! Class destructor
											~CScene();

											//! Parse a scene file
	const bool								Parse(
												std::string sceneFileLocation						//!< The location of the scene file to parse
											);

											//! Get the number of nodes in the scene
	const unsigned int						GetNoofNodes() const;

											//! Get a node by a given index
	NodeMap									*GetNodeMap( 
												unsigned int nodeIndex								//!< The index of the node to get
											) const;
};