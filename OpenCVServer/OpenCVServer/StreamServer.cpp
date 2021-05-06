#include "StreamServer.h"

StreamServer::StreamServer()
{
}

StreamServer::~StreamServer()
{
	// No longer need server socket
	if (listenSocket != INVALID_SOCKET)
	{
		closesocket(listenSocket);
	}
	for (SOCKET clientSocket : clientSockets)
	{
		closesocket(clientSocket);
	}
	WSACleanup();

	cap.release();
	destroyAllWindows();
}

ServerStatus init()
{
	// Socket initialize
	WSADATA wsaData;
	int iResult;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
		return FAIL_INITIALIZED;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
		WSACleanup();
		return FAIL_INITIALIZED;
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return FAIL_INITIALIZED;
	}

	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return FAIL_INITIALIZED;
	}

	freeaddrinfo(result);

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return FAIL_INITIALIZED;
	}

	// OpenCV initialize
	// 640 480 3 921600
	int deviceID = 0;
	int apiID = cv::CAP_ANY;
	cap.set(CAP_PROP_FRAME_WIDTH, VIDEO_WIDTH);
	cap.set(CAP_PROP_FRAME_HEIGHT, VIDEO_HEIGHT);

	cap.open(deviceID, apiID);
	if (!cap.isOpened())
	{
		std::cerr << "open camera with error" << std::endl;
		return FAIL_INITIALIZED;
	}

	return INITIALIZED;
}

void acceptSocket()
{
	// Accept a client socket
	SOCKADDR_IN  addr;
	int addrlen = sizeof(addr);
	while (status == RUNNING)
	{
		SOCKET newClientSocket = accept(listenSocket, (sockaddr*)&addr, &addrlen);
		if (newClientSocket == INVALID_SOCKET) {
			std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
			status = SERVER_ERROR;
			closesocket(listenSocket);
			break;
		}

		clientSockets.push_back(newClientSocket);
		std::cout << "Accepted a new client (total: " << clientSockets.size() << ")" << std::endl;
	}
}

void sendVideoStream()
{
	int iResult;
	Mat frame;
	std::vector<std::vector<SOCKET>::iterator> leavedClientIters;
	// Send the video stream
	while (status == RUNNING && cap.isOpened())
	{
		cap.read(frame);
		if (frame.empty())
		{
			std::cerr << "video stream failed with error" << std::endl;
			status = SERVER_ERROR;
			closesocket(listenSocket);
			break;
		}

		imshow("Server", frame);
		if (waitKey(1) >= 0) {
			status = SERVER_STOP;
			closesocket(listenSocket);
			break;
		}

		leavedClientIters.clear();
		for (std::vector<SOCKET>::iterator it = clientSockets.begin(); it != clientSockets.end(); ++it)
		{
			iResult = send((SOCKET)*it, (char*)frame.data, VIDEO_BUFLEN, 0);
			if (iResult == SOCKET_ERROR)
			{
				leavedClientIters.push_back(it);
				continue;
			}
		}

		for (std::vector<SOCKET>::iterator leavedClientIter : leavedClientIters)
		{
			clientSockets.erase(leavedClientIter);
			std::cout << "Leave a client (total: " << clientSockets.size() << ")" << std::endl;
		}
	}
}

void StreamServer::run()
{
	ServerStatus initResult = init();
	if (initResult != INITIALIZED)
	{
		std::cout << "The server initialization is not completed. Can't run the StreamServer." << std::endl;

		return;
	}

	status = RUNNING;
	std::thread acceptSocketThread(acceptSocket);
	std::thread sendVideoStreamThread(sendVideoStream);

	acceptSocketThread.join();
	sendVideoStreamThread.join();
}
