// file: SurfingFilesManager.h
#pragma once

#include "ofMain.h"
#include <functional>
#include "SurfingFilesManager.h"

class SurfingFilesManager {
public:
	SurfingFilesManager();
	~SurfingFilesManager();

	void setup();
	void exit(); // Auto-save settings

	// Callback for custom export logic (e.g., FBO → PNG)
	// Return: filename saved (empty if failed)
	using ExportCallback = std::function<std::string(const std::string & folder)>;
	void setExportCallback(ExportCallback cb) { exportCallback = cb; }

	// Public API
	void doExport();
	void doChooseFolder();
	void doOpenExportFolder();

	void setPathFolder(const std::string & path) { pathFolder = path; }
	std::string getPathFolder() const { return pathFolder.get(); }

	// GUI Integration
	ofParameterGroup params;

private:
	void setupParams();
	void setupCallbacks();

	// Parameters
	ofParameterGroup paramsExport;
	ofParameter<std::string> pathFolder;
	ofParameter<void> vChooseFolder;
	ofParameter<void> vExport;
	ofParameter<void> vOpenExportFolder;

	// Listeners
	ofEventListener e_vChooseFolder;
	ofEventListener e_vExport;
	ofEventListener e_vOpenExportFolder;

	// Export logic injected by user
	ExportCallback exportCallback;

	bool bDoneExit = false;
};
