#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <string.h>


using namespace std;
using json = nlohmann::json;

size_t writecallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  size_t realsize = size * nmemb;
  string *buffer = (string *)userdata;
  buffer->append(ptr, realsize);
  return realsize; // Don't forget to return the correct number of bytes
}

string weatherData;
string cityName;

int main() {
  // Initialize curl and other variables
  CURL *curl;
  CURLcode res;
  string readBuffer;
  string apiKey = "TOKEN";

  // Get city name input from user
  printf("Enter City name: ");
  getline(cin, cityName);
  if (cityName.empty()) {
    printf("Enter a valid city name.\n");
    return 1; // Exit if the city name is empty
  }

  // Construct the API URL
  string url = "https://api.openweathermap.org/data/2.5/weather?q=" + cityName +
               "&appid=" + apiKey;

  // Initialize curl globally and the curl handle
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  if (curl) {
    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the curl request
    res = curl_easy_perform(curl); // Capture the result of the curl operation

    // Check the result of the request
    if (res == CURLE_OK) {
      // Successfully retrieved data
      try {
        auto jsonData =
            nlohmann::json::parse(readBuffer); // Parse the response into JSON

        // Output the weather information
        cout << "Weather in " << cityName << ":" << endl;
        cout << "Temp: " << jsonData["main"]["temp"] << "°K" << endl;
        cout << "Weather: " << jsonData["weather"][0]["description"] << endl;
      } catch (const nlohmann::json::exception &e) {
        cout << "Error parsing JSON: " << e.what() << endl;
      }
    } else {
      // Handle different curl errors
      if (res == CURLE_UNSUPPORTED_PROTOCOL) {
        printf("The URL you passed to libcurl used a protocol that this "
               "libcurl does not support.\n");
      } else if (res == CURLE_FAILED_INIT) {
        printf("Early initialization code failed.\n");
      } else if (res == CURLE_URL_MALFORMAT) {
        printf("The URL was not properly formatted.\n");
      } else if (res == CURLE_NOT_BUILT_IN) {
        printf("A requested feature, protocol or option was not found built "
               "into this libcurl.\n");
      } else {
        printf("An unknown error occurred: %s\n", curl_easy_strerror(res));
      }
    }

    // Clean up the curl handle
    curl_easy_cleanup(curl);
  } else {
    printf("Curl initialization failed.\n");
  }

  // Clean up global curl environment
  curl_global_cleanup();

  return 0;
}
