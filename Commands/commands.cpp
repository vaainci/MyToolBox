#include "commands.h"
#include <iostream>
#include <string>

// Networking & System includes
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <sys/statvfs.h>
#include <mntent.h>
#include <iomanip>
#include <set>
#include <algorithm>

#include <cstring>

std::string APP_VERSION = "V0.2.4";

Commands::Commands() {}

void Commands::help() {
    std::string bold = "\033[1m";
    std::string cyan = "\033[1;36m";
    std::string green = "\033[1;32m";
    std::string yellow = "\033[1;33m";
    std::string blue = "\033[1;34m";
    std::string reset = "\033[0m";

    std::cout << cyan << R"(
  __  __       _______          _ ____
 |  \/  |     |__   __|        | |  _ \
 | \  / |_   _   | | ___   ___ | | |_) | _____  __
 | |\/| | | | |  | |/ _ \ / _ \| |  _ < / _ \ \/ /
 | |  | | |_| |  | | (_) | (_) | | |_) | (_) >  <
 |_|  |_|\__, |  |_|\___/ \___/|_|____/ \___/_/\_\
          __/ |
         |___/                                     )" << reset << std::endl;

    std::cout << " " << bold << APP_VERSION << reset << " | Developed by David" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    std::cout << "\n" << yellow << "USAGE:" << reset << std::endl;
    std::cout << "  mtb <command> [options]" << std::endl;

    std::cout << "\n" << yellow << "AVAILABLE COMMANDS:" << reset << std::endl;
    std::cout << std::left << std::setw(12) << (green + "  ip") << reset << "Display your local and public network addresses." << std::endl;
    std::cout << std::left << std::setw(12) << (green + "  storage") << reset << "Monitor disk usage and partition health." << std::endl;

    std::cout << "\n" << yellow << "GENERAL OPTIONS:" << reset << std::endl;
    std::cout << std::left << std::setw(15) << (blue + "  -h, --help") << reset << "Show this help message and exit." << std::endl;
    std::cout << std::left << std::setw(15) << (blue + "  -v, --version") << reset << "Show version information." << std::endl;

    std::cout << "\n" << std::string(55, '-') << std::endl;

    std::string url = "https://github.com/vaainci/MyToolBox";
    std::string label = "github.com/vaainci/MyToolBox";

    std::cout << "For more info, visit: " << cyan
              << "\033]8;;" << url << "\033\\" << label << "\033]8;;\033\\"
              << reset << std::endl << std::endl;
}

void Commands::version()
{
    std::cout << APP_VERSION << std::endl;
}

void Commands::ip() {

    std::string cyan = "\033[1;36m";
    std::string green = "\033[1;32m";
    std::string blue = "\033[1;34m";
    std::string bold = "\033[1m";
    std::string reset = "\033[0m";

    std::cout << "\n" << bold << "--- Network Information ---" << reset << std::endl;

    struct ifaddrs *ifAddrStruct = nullptr;
    struct ifaddrs *ifa = nullptr;
    void *tmpAddrPtr = nullptr;
    std::string localIp = "Disconnected";
    std::string interfaceName = "";

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            std::string name = ifa->ifa_name;
            if (name != "lo") {
                localIp = addressBuffer;
                interfaceName = name;
                break;
            }
        }
    }
    if (ifAddrStruct) freeifaddrs(ifAddrStruct);

    std::cout << std::left << std::setw(15) << "Local IP" << " : "
              << green << localIp << reset;
    if (!interfaceName.empty()) {
        std::cout << " (" << cyan << interfaceName << reset << ")";
    }
    std::cout << std::endl;

    std::string publicIp = "Unavailable";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd >= 0) {
        struct timeval tv;
        tv.tv_sec = 1; tv.tv_usec = 500000; // 1.5s
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

        struct hostent *server = gethostbyname("ifconfig.me");
        if (server) {
            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(80);
            std::copy((char *)server->h_addr, (char *)server->h_addr + server->h_length, (char *)&serv_addr.sin_addr.s_addr);

            if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
                const char *msg = "GET /ip HTTP/1.1\r\nHost: ifconfig.me\r\nUser-Agent: curl/7.0.0\r\nAccept: */*\r\n\r\n";
                send(sockfd, msg, strlen(msg), 0);

                char buffer[1024];
                int n = recv(sockfd, buffer, 1023, 0);
                if (n > 0) {
                    buffer[n] = '\0';
                    std::string response(buffer);
                    size_t pos = response.find("\r\n\r\n");
                    if (pos != std::string::npos) {
                        publicIp = response.substr(pos + 4);
                        publicIp.erase(publicIp.find_last_not_of(" \n\r\t") + 1);
                    }
                }
            }
        }
        close(sockfd);
    }
    std::cout << std::left << std::setw(15) << "Public IP" << " : "
              << blue << publicIp << reset << std::endl << std::endl;
}

void Commands::storage() {
    struct mntent *ent;
    FILE *mounts = setmntent("/proc/mounts", "r");
    std::set<std::string> seenDevices;

    if (mounts == nullptr) return;

    std::cout << "\n\033[1m" << std::left
              << std::setw(15) << "LABEL"
              << std::setw(18) << "DEVICE"
              << std::setw(22) << "USAGE"
              << "STATUS" << "\033[0m" << std::endl;
    std::cout << std::string(65, '-') << std::endl;

    while ((ent = getmntent(mounts)) != nullptr) {
        std::string device = ent->mnt_fsname;
        std::string mountPoint = ent->mnt_dir;

        if (device.find("/dev/") != 0) continue;
        if (mountPoint.find("/var/") == 0 || mountPoint.find("/boot") == 0 || mountPoint.find("/run/") == 0) continue;
        if (seenDevices.count(device)) continue;

        struct statvfs stats;
        if (statvfs(mountPoint.c_str(), &stats) == 0) {
            seenDevices.insert(device);

            unsigned long long total = stats.f_blocks * stats.f_frsize;
            unsigned long long used = (stats.f_blocks - stats.f_bfree) * stats.f_frsize;
            double percent = (total > 0) ? (static_cast<double>(used) / total) * 100.0 : 0;

            std::string label = (mountPoint == "/") ? "SYSTEM" : mountPoint;
            if (label != "SYSTEM") {
                size_t lastSlash = label.find_last_of('/');
                if (lastSlash != std::string::npos) label = label.substr(lastSlash + 1);
            }
            for (auto & c: label) c = toupper(c);

            std::string color = "\033[32m";
            std::string statusText = "OK";
            if (percent > 75.0) { color = "\033[33m"; statusText = "WARN"; }
            if (percent > 90.0) { color = "\033[31m"; statusText = "CRIT"; }
            std::string reset = "\033[0m";

            std::cout << std::left << std::setw(15) << label;
            std::cout << std::left << std::setw(18) << device;

            int barWidth = 10;
            int pos = barWidth * (percent / 100.0);

            std::cout << color << "[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "■";
                else std::cout << " ";
            }
            std::cout << "] " << reset;

            std::cout << std::right << std::fixed << std::setprecision(1) << std::setw(5) << percent << "%  ";

            std::cout << color << statusText << reset << std::endl;
        }
    }
    endmntent(mounts);
    std::cout << std::endl;
}