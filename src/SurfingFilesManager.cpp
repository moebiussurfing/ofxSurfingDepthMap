// file: SurfingFilesManager.cpp
#include "SurfingFilesManager.h"
#include "ofxSurfingDepthMapSerializers.h" // For settings save/load

//--------------------------------------------------------------
SurfingFilesManager::SurfingFilesManager() {
}

//--------------------------------------------------------------
SurfingFilesManager::~SurfingFilesManager() {
	if (!bDoneExit) exit();
}

//--------------------------------------------------------------
void SurfingFilesManager::setup() {
	setupParams();
	setupCallbacks();

	// Load saved settings
	ofxSurfingDepthMapSerializers::loadSettings(params);
}

//--------------------------------------------------------------
void SurfingFilesManager::setupParams() {
	paramsExport.setName("Export");
	paramsExport.add(vChooseFolder.set("Set Folder"));
	paramsExport.add(pathFolder.set("Folder", ""));
	paramsExport.add(vOpenExportFolder.set("Open Folder"));
	paramsExport.add(vExport.set("Export"));

	params.setName("SurfingFilesManager");
	params.add(paramsExport);
}

//--------------------------------------------------------------
void SurfingFilesManager::setupCallbacks() {
	e_vChooseFolder = vChooseFolder.newListener([this](const void * sender) {
		doChooseFolder();
	});

	e_vOpenExportFolder = vOpenExportFolder.newListener([this](const void * sender) {
		doOpenExportFolder();
	});

	e_vExport = vExport.newListener([this](const void * sender) {
		doExport();
	});
}

//--------------------------------------------------------------
void SurfingFilesManager::doExport() {
	if (!exportCallback) {
		ofLogError("SurfingFilesManager") << "No export callback set!";
		return;
	}

	std::string folder = (pathFolder.get().empty())
		? ofToDataPath("", true)
		: pathFolder.get();

	std::string savedFile = exportCallback(folder);

	if (!savedFile.empty()) {
		ofLogNotice("SurfingFilesManager") << "Exported to: " << savedFile;

		// Auto-open the file (cross-platform)
		std::string quotedPath = "\"" + ofFilePath::getAbsolutePath(savedFile) + "\"";

#ifdef TARGET_OSX
		ofSystem("open " + quotedPath);
#elif defined(TARGET_WIN32)
		ofSystem("start \"\" " + quotedPath);
#elif defined(TARGET_LINUX)
		ofSystem("xdg-open " + quotedPath);
#endif
	}
}

//--------------------------------------------------------------
void SurfingFilesManager::doChooseFolder() {
	ofFileDialogResult result = ofSystemLoadDialog("Select output folder", true);

	if (result.bSuccess) {
		pathFolder = result.getPath();
		ofLogNotice("SurfingFilesManager") << "Selected folder: " << pathFolder.get();
	}
}

//--------------------------------------------------------------
void SurfingFilesManager::doOpenExportFolder() {
	std::string folderStr = (pathFolder.get().empty())
		? ofToDataPath("", true)
		: pathFolder.get();

	ofFile folder(folderStr);
	if (!folder.isDirectory()) {
		folderStr = folder.getEnclosingDirectory();
	}
	folderStr = folder.getAbsolutePath();

	ofLogNotice("SurfingFilesManager") << "Opening folder: " << folderStr;

#ifdef TARGET_OSX
	std::string cmd = "open \"" + folderStr + "\"";
	system(cmd.c_str());
#elif defined(TARGET_LINUX)
	std::string cmd = "xdg-open \"" + folderStr + "\"";
	system(cmd.c_str());
#elif defined(_WIN32)
	std::string cmd = "start \"\" \"" + folderStr + "\"";
	system(cmd.c_str());
#endif
}

//--------------------------------------------------------------
void SurfingFilesManager::exit() {
	ofxSurfingDepthMapSerializers::saveSettings(params);
	bDoneExit = true;
}
