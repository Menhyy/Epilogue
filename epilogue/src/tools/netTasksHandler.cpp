//
// Created by cher on 08/08/2026.
//

#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include "netTasksHandler.h"
#include <borealis.hpp>

using namespace std;

static size_t write_data(const char *ptr, const size_t size, const size_t nmemb, void *userdata) {
    auto *stream = static_cast<std::ostringstream*>(userdata);
    const size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

std::string writeFileNet(const string& URL, const string& filePath, const string& savePath)
{
    ostringstream buffer;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (CURL* net = curl_easy_init())
    {
        FILE* TestFile = fopen(savePath.c_str(), "wb"); //Because we need to use fopen instead of ofstreams...
        if (!TestFile)
        {
            brls::Logger::error("Could not open file {}", savePath);
            return "CURL_ERR";
        }
        const std::string fullURL = URL + filePath;
        curl_easy_setopt(net, CURLOPT_URL, fullURL.c_str());
        curl_easy_setopt(net, CURLOPT_USERAGENT, "libnx curl example/1.0");
        curl_easy_setopt(net, CURLOPT_WRITEDATA, TestFile);
        curl_easy_setopt(net, CURLOPT_SSL_VERIFYHOST, 0); curl_easy_setopt(net, CURLOPT_SSL_VERIFYPEER, 0); // Just until we get a http server...
        const CURLcode res = curl_easy_perform(net);
        curl_easy_cleanup(net);
        fclose(TestFile);
        if (res != CURLE_OK)
        {
            remove(savePath.c_str());
            brls::Application::notify("Failed to fetch file from network.\nPlease check logs!");
            brls::Logger::error(curl_easy_strerror(res));
            return "CURL_ERR";
        }
    }else
    {
        brls::Logger::error("CURL could not initialize.");
        brls::Application::notify("Failed to fetch file from network.\nPlease check logs!");
        return "CURL_ERR";
    }
    return "OK";
}

std::string readFileNet(const string& URL, const string& filePath)
{
    ostringstream buffer;
    string result;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (CURL* net = curl_easy_init())
    {
        const std::string fullURL = URL + filePath;
        curl_easy_setopt(net, CURLOPT_URL, fullURL.c_str());
        curl_easy_setopt(net, CURLOPT_USERAGENT, "libnx curl example/1.0");
        curl_easy_setopt(net, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(net, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(net, CURLOPT_SSL_VERIFYHOST, 0); curl_easy_setopt(net, CURLOPT_SSL_VERIFYPEER, 0); // Just until we get a http server...
        const CURLcode res = curl_easy_perform(net);
        curl_easy_cleanup(net);
        if (res != CURLE_OK)
        {
            result = "CURL_ERR";
            brls::Logger::error(curl_easy_strerror(res));
            brls::Application::notify("Failed to fetch file from network.\nPlease check logs!");
            return result;
        }
        result = buffer.str();
    }else
    {
        result = "CURL_ERR";
        brls::Logger::error("CURL could not initialize.");
        brls::Application::notify("Failed to fetch file from network.\nPlease check logs!");
        return result;
    }
    return result;
}
