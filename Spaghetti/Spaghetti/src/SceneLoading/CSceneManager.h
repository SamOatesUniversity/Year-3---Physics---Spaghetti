#pragma once

#include <Windows.h>
#include <direct.h>
#include <vector>
#include <sstream>

#include "CScene.h"

class CSceneManager {

private:

	std::vector<CScene*>					m_scene;								//!< A list of loaded scenes

public:
											//! Class constructor
											CSceneManager();

											//! Class destructor
											~CSceneManager();

											//! Load all the .scene files from a given folder
											//! Returns the number of scenes that were loaded
	const int								LoadScenes(
												std::string sceneFolder				//!< The folder to search for .scene files
											);
};