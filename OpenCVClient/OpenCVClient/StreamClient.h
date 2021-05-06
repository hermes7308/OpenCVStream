#pragma once
#define WIN32_LEAN_AND_MEAN

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_CHANNELS 3
#define VIDEO_BUFLEN (VIDEO_WIDTH*VIDEO_HEIGHT*VIDEO_CHANNELS)

using namespace cv;

class StreamClient
{
public:
	StreamClient(char* serverAddress);
	void run();
private:
	char* mServerAddress;
};

