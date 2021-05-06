#pragma once
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

// Socket
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Common
#include <iostream>
#include <vector>
#include <thread>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_CHANNELS 3
#define VIDEO_BUFLEN (VIDEO_WIDTH*VIDEO_HEIGHT*VIDEO_CHANNELS)

using namespace cv;

enum ServerStatus
{
	NOT_INITIALIZED = 0,
	FAIL_INITIALIZED,
	INITIALIZED,
	RUNNING,
	SERVER_ERROR,
	SERVER_STOP
};

static ServerStatus status{ ServerStatus::NOT_INITIALIZED };
static SOCKET listenSocket = INVALID_SOCKET;
static std::vector<SOCKET> clientSockets;
static VideoCapture cap;

class StreamServer
{
public:
	StreamServer();
	~StreamServer();
	void run();
private:
};

