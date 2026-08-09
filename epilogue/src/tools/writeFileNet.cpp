//
// Created by cher on 08/08/2026.
//

#include "writeFileNet.h"
#include <sstream>
#include <fstream>
#include <curl/curl.h>
//#include <switch.h>

using namespace std;

#define TESTFILEPATH "sdmc:/TestFileNet.txt"

static size_t write_data(const char *ptr, const size_t size, const size_t nmemb, void *userdata) {
    auto *stream = static_cast<std::ostringstream*>(userdata);
    const size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

std::string writeFileDebugNet(const string& URL, const string& filePath, const string& savePath)
{
    ostringstream buffer;
    string result;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (CURL* net = curl_easy_init())
    {
        curl_easy_setopt(net, CURLOPT_URL, "https://actinyde.duckdns.org/Others/Sample.txt");
        curl_easy_setopt(net, CURLOPT_USERAGENT, "libnx curl example/1.0");
        curl_easy_setopt(net, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(net, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(net, CURLOPT_SSL_VERIFYHOST, 0); curl_easy_setopt(net, CURLOPT_SSL_VERIFYPEER, 0); // Just until we get a http server...
        const CURLcode res = curl_easy_perform(net);
        curl_easy_cleanup(net);
        if (res != CURLE_OK)
        {
            result = "CURL_ERR";
            return curl_easy_strerror(res);
        }
        result = buffer.str();
        ofstream TestFile(savePath);
        TestFile << result;
        TestFile.close();
    }
    return result;
}
