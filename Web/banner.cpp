// HTML banner info by Nathan W Jones nat@davaosecurity.com
// Install libcurl: Make sure you have libcurl installed curl.se or install it via a package manager.
// Link against libcurl: When compiling your program, ensure you link against the libcurl library.

#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback function to write the response data to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

void GetHTMLHeadersAndBanners(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Print the headers
            char* contentType;
            curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);
            std::cout << "Content-Type: " << (contentType ? contentType : "unknown") << std::endl;

            // Print the HTML content (banners)
            std::cout << "HTML Content:\n" << readBuffer << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    std::string url;

    std::cout << "Enter the URL: ";
    std::cin >> url;

    GetHTMLHeadersAndBanners(url);
    return 0;
}
