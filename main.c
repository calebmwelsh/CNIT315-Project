// Caleb Welsh other names
// cmwelsh@purdue.edu other emails



// Description:
// The program finds 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_KEY "9d138ef758378bf9d63431c3d35cda61"
#define API_URL "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric"

// Structure to hold the response data
struct string {
    char *ptr;
    size_t len;
};

// Function to initialize the response string
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

// Function to handle data received by cURL
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

// Function to make the API call and get weather information
void get_weather(const char *location) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    char url[256];
    snprintf(url, sizeof(url), API_URL, location, API_KEY);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Weather Data: %s\n", s.ptr);  // Raw JSON response
            // You can add JSON parsing here to display specific details like temperature and humidity
        }

        curl_easy_cleanup(curl);
    }

    free(s.ptr);
    curl_global_cleanup();
}

int main() {
    char location[100];
    printf("Enter location (city name): ");
    scanf("%s", location);

    printf("Fetching weather data for %s...\n", location);
    get_weather(location);

    return 0;
}
