#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <cmath>

#include <thread>
#include <chrono>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//const int TOLERANCE{ 20 };

const int shiny[3]{ 173, 201, 248 };    //赤
const int shiny_sub[3]{ 0, 0, 253 };    //白
const int command1[3]{ 156, 196, 178 };
const int command2[3]{ 158, 169, 186 };
const int finish1[3]{ 156, 198, 169 };
const int finish2[3]{ 159, 177, 169 };
const int back1[3]{ 173, 22, 249 };
const int back2[3]{ 166, 29, 248 };
const int back3[3]{ 159, 30, 247 };
const int Pouse[3]{ 0, 0, 0 };//黒

class UDPSender {
    SOCKET sock;
    sockaddr_in addr;   //宛先ラベル
    int status;
public:
    UDPSender(const char* ip, int port) {
        WSADATA wsa{};
        status = WSAStartup(MAKEWORD(2, 2), &wsa);
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &addr.sin_addr);
    }
    ~UDPSender() {
        closesocket(sock);
        WSACleanup();
    }
    int getStatus() const {
        return status;
    }
    void send(char cmd) {
        sendto(sock, &cmd, sizeof(cmd), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    }
};

cv::Vec3b getHSVAt(cv::Mat& bgrFrame, int x, int y) {
    cv::Mat bgrPixel(1, 1, CV_8UC3, bgrFrame.at<cv::Vec3b>(y, x));
    cv::Mat hsvPixel{};

    cv::cvtColor(bgrPixel, hsvPixel, cv::COLOR_BGR2HSV);
    return hsvPixel.at<cv::Vec3b>(0, 0);
}

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        cv::Mat* frame{static_cast<cv::Mat*>(userdata)};
        if (frame->empty()) {
            return;
        }


        cv::Vec3b hsv{ getHSVAt(*frame, x, y) };
        std::cout << "Target HSV -> { " << static_cast<int>(hsv[0]) << ", "
            << static_cast<int>(hsv[1]) << ", " << static_cast<int>(hsv[2]) << " }; // [X: " << x << ", Y: " << y << "]" << std::endl;
    }
}

bool checkHSV(cv::Vec3b hsvPixel, const int* targetHSV) {
    int h{ hsvPixel[0] }; // 色相
    int s{ hsvPixel[1] }; // 彩度
    int v{ hsvPixel[2] }; // 明度

    if (targetHSV[2] < 30) { //黒
        return (v < 40);
    }

    if (targetHSV[1] < 30 && targetHSV[2] > 200) { //白
        return (s < 40 && v > 200);
    }

    int diffH = std::abs(h - targetHSV[0]);

    if (diffH > 90) {
        diffH = 180 - diffH;
    }

    return (diffH <= 10 &&
        std::abs(s - targetHSV[1]) <= 40 &&
        std::abs(v - targetHSV[2]) <= 40);
}

void rest_sendManager(UDPSender& receiver, char cmd, cv::VideoCapture& cap, cv::Mat& bgrFrame) {    //1.2秒間、60fpsで送信する
    auto burstStart = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - burstStart < std::chrono::milliseconds(1200)) {
        receiver.send(cmd);

        cap >> bgrFrame;
		static int frameCount = 0;
		frameCount++;
        if (frameCount % 6 == 0) {    //UIのために表示し続ける
            cv::imshow("Switch Screen", bgrFrame);
			frameCount = 0;
        }

        cv::waitKey(16);
    }
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    UDPSender coreS3("192.168.11.13", 12345);//CoreS3のIPアドレスとポート番号
    if (coreS3.getStatus() != 0) {
        std::cerr << "WSAStartup failed. Error: " << WSAGetLastError() << std::endl;
        return -1;
    }
    cv::VideoCapture cap(0, cv::CAP_DSHOW);

    if (!cap.isOpened()) {
        std::cerr << "Capture board not found" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

    cv::Mat bgrFrame;
    cv::namedWindow("Switch Screen", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Switch Screen", onMouse, &bgrFrame);

    auto lastSoloTime{ std::chrono::steady_clock::now() };
    int frameCount = 0;
    bool Cooldown = false;
    bool back = false;

    while (true) {
        cap >> bgrFrame;

        if (bgrFrame.empty()) {
            std::cerr << "Frame is empty." << std::endl;
            break;
        }

        frameCount++;

        if (frameCount % 6 == 0) {
            cv::imshow("Switch Screen", bgrFrame);
            frameCount = 0;
        }
        cv::waitKey(1);


        cv::Vec3b shiny1 = getHSVAt(bgrFrame, 114, 404);
        cv::Vec3b shiny2 = getHSVAt(bgrFrame, 123, 393);
        cv::Vec3b shiny3 = getHSVAt(bgrFrame, 118, 397);
        if (checkHSV(shiny1, shiny) && checkHSV(shiny2, shiny) && checkHSV(shiny3, shiny_sub)) {
            std::cout << "Shiny Detected!" << std::endl;
            rest_sendManager(coreS3, 'S', cap, bgrFrame);
            Cooldown = true;
            lastSoloTime = std::chrono::steady_clock::now();
        }

        cv::Vec3b ps1 = getHSVAt(bgrFrame, 1065, 471);
        cv::Vec3b ps2 = getHSVAt(bgrFrame, 506, 374);
        cv::Vec3b ps3 = getHSVAt(bgrFrame, 50, 300);
        if (checkHSV(ps1, Pouse) && checkHSV(ps2, Pouse) && checkHSV(ps3, Pouse)) {
            static auto lastSent = std::chrono::steady_clock::now();

            if (std::chrono::steady_clock::now() - lastSent > std::chrono::milliseconds(16)) {
                coreS3.send('P');
                lastSent = std::chrono::steady_clock::now();
            }
            continue;
        }

        if (Cooldown) {
            auto now{ std::chrono::steady_clock::now() };
            auto elapsed{ std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSoloTime).count() };
			if (elapsed > 18000) {  //18秒のクールダウン
                Cooldown = false;
                std::cout << "INFO: cooldown finished. Ready to detect again." << std::endl;
            }
            continue;
        }


        cv::Vec3b solo1 = getHSVAt(bgrFrame, 23, 392);
        cv::Vec3b solo2 = getHSVAt(bgrFrame, 35, 12);
        if (checkHSV(solo1, command1) && checkHSV(solo2, command2)) {
            std::cout << "INFO: Solo Battle Detected!" << std::endl;
            rest_sendManager(coreS3, 'C', cap, bgrFrame);
            Cooldown = true;
            lastSoloTime = std::chrono::steady_clock::now();
        }
   
        cv::Vec3b fin1 = getHSVAt(bgrFrame, 1169, 472);
        cv::Vec3b fin2 = getHSVAt(bgrFrame, 855, 202);
        cv::Vec3b fin3 = getHSVAt(bgrFrame, 643, 102);
        if (checkHSV(fin1, finish1) && checkHSV(fin2, finish2) && checkHSV(fin3, Pouse)) {
            std::cout << "FINISH was covered..." << std::endl;
            rest_sendManager(coreS3, 'F', cap, bgrFrame);
            Cooldown = true;
            back = !back;
            lastSoloTime = std::chrono::steady_clock::now();
        }
        
    }

    return 0;
}