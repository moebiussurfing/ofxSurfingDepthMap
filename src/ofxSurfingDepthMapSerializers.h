#pragma once
#include "ofMain.h"

//------

namespace ofxSurfingDepthMapSerializers {

//------

/*
* Settings Management
* 
	Settings de/serializers for ofParameterGroup.
*/

// Load
//--------------------------------------------------------------
inline bool loadSettings(ofParameterGroup & parameters, string path = "") {
	if (path == "") path = parameters.getName() + ".json";

	//verify that the file exist
	ofFile f;
	bool b = f.doesFileExist(path);
	if (b)
		ofLogVerbose("ofxSurfing") << "Found settings file: " << path << " for ofParameterGroup: " << parameters.getName();
	else
		ofLogError("ofxSurfing") << "File " << path
								 << " for ofParameterGroup " << parameters.getName() << " not found!";
	//load if exist
	if (b) {
		ofJson settings;
		settings = ofLoadJson(path);
		ofDeserialize(settings, parameters);
	}

	return b;
}

// Save
//--------------------------------------------------------------
inline bool saveSettings(ofParameterGroup & parameters, string path = "") {
	if (path == "") path = parameters.getName() + ".json";

	//if the destination folder do not exists, will be created.
	if (!ofDirectory::doesDirectoryExist(ofFilePath::getEnclosingDirectory(path))) {
		if (ofFilePath::createEnclosingDirectory(path))
			ofLogWarning("ofxSurfing") << "Created enclosing folder for: " << path;
		else
			ofLogError("ofxSurfing") << "Unable to create enclosing folder for: " << path;
	}
	//try to save the file
	ofJson settings;
	ofSerialize(settings, parameters);
	bool b = ofSavePrettyJson(path, settings);
	if (b)
		ofLogVerbose("ofxSurfing") << "Saved ofParameterGroup: " << parameters.getName() << " to " << path;
	else
		ofLogError("ofxSurfing") << "Error saving: " << parameters.getName() << " to " << path;

	return b;
}

// Create if a folder path is found or not
//--------------------------------------------------------------
inline void checkFolderOrCreate(string path) {
	if (!ofDirectory::doesDirectoryExist(ofFilePath::getEnclosingDirectory(path))) {
		if (ofFilePath::createEnclosingDirectory(path, true))
			ofLogWarning("ofxSurfing") << "Created enclosing folder for: " << path;
		else
			ofLogError("ofxSurfing") << "Unable to create enclosing folder for: " << path;
	}
	ofLogVerbose("ofxSurfing") << "Found enclosing folder for: " << path;
}

// Create if a file path is found or not
//--------------------------------------------------------------
inline bool checkFileExist(string path) {
	ofFile f2;
	bool b2 = f2.doesFileExist(path);
	if (b2) {
		ofLogVerbose("ofxSurfing") << "Found file: " << path;
	} else {
		ofLogWarning("ofxSurfing") << "File: " << path << " not found!";
	}
	return b2;
}

// LEGACY
// for ofxSurfingHelpers
inline bool loadGroup(ofParameterGroup & parameters, string path = "") {
	return loadSettings(parameters, path);
}
inline bool saveGroup(ofParameterGroup & parameters, string path = "") {
	return saveSettings(parameters, path);
}
inline bool load(ofParameterGroup & parameters, string path = "") {
	return loadSettings(parameters, path);
}
inline bool save(ofParameterGroup & parameters, string path = "") {
	return saveSettings(parameters, path);
}

}
