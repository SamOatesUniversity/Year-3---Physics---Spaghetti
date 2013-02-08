#include "CSceneManager.h"

CSceneManager::CSceneManager()
{

}

CSceneManager::~CSceneManager()
{

}

bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

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

