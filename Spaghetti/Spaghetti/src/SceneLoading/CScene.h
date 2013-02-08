#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>

#include <sys/stat.h>
#include <time.h>

// Type def the map layout of the node data
typedef std::map<std::string, std::string>	NodeMap;

class CScene {

private:

	std::string								m_fileLocation;											//!< The location of this scenes data file
	time_t									m_editTime;												//!< The last time this scenes data file was edited
	std::vector<NodeMap*>					m_data;													//!< A list of node map data of the parsed scene

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

											//! Check to see if the scene has been edited, and re-parse if it has
	void									CheckForUpdate();
};