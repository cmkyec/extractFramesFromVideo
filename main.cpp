#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <opencv2\opencv.hpp>
#include "json\include\json.h"

struct param
{
	std::string videoPath;
	std::string saveFolder;
	int extractFrequency;
};

bool readConfig(std::string configFilePath, struct param& param)
{
	std::ifstream configFile(configFilePath.c_str());
	if (!configFile.is_open()) {
		std::cerr << "can not open the config file: " << configFilePath << std::endl;
		return false;
	}
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(configFile, root)) {
		std::string errMsg = reader.getFormatedErrorMessages();
		std::cerr << "parse config file failed." << std::endl;
		std::cerr << errMsg << std::endl;
		return false;
	}

	param.videoPath = root["videoPath"].asString();
	param.saveFolder = root["saveFolder"].asString();
	param.extractFrequency = root["extractFrequency"].asInt();
	return true;
}

bool createSaveFolder(std::string saveFolder)
{
	BOOL res = ::CreateDirectory(saveFolder.c_str(), NULL);
	if (!res && ::GetLastError() != ERROR_ALREADY_EXISTS) {
		std::cerr << "create save folder failed. please check the save folder path."
			  << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	std::string configFilePath;
	if (argc < 2) {
		configFilePath = "./extractFrames.config";
		std::cout << "use the default config file: ./extractFrames.config" << std::endl;
		std::cout << "specify the config file by extractFramesFromVideo.exe path_to_config_file"
			  << std::endl;
	}
	else {
		configFilePath = argv[1];
	}
	struct param param;
	if (!readConfig(configFilePath, param)) return -1;
	std::cout << "video path: " << param.videoPath << std::endl;
	std::cout << "save folder: " << param.saveFolder << std::endl;
	std::cout << "extract frequency: " << param.extractFrequency << std::endl;

	if (!createSaveFolder(param.saveFolder)) return -1;
	
	cv::VideoCapture video(param.videoPath);
	if (!video.isOpened()) {
		std::cerr << "can not open the video: " << param.videoPath << std::endl;
		return -1;
	}

	cv::Mat frame;
	char savepath[200] = { 0 };
	cv::namedWindow("show", 0);
	int index = 1, count = 1;
	while (true) {
		video >> frame;
		if (!frame.data) break;
		if ((index++ % param.extractFrequency) == 0) {
			sprintf_s(savepath, sizeof(savepath), "%s\\%05d.png", 
				param.saveFolder.c_str(), count++);
			cv::imwrite(savepath, frame);
		}
		cv::imshow("show", frame);
		if (cv::waitKey(30) == 27) break;
	}
	return 0;
}