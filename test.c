#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h> // For JSON parsing

#define API_KEY "9d138ef758378bf9d63431c3d35cda61"
#define API_URL_COORDS "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s"

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

// Function to fetch coordinates based on location
int get_coordinates(const char *location, char *latitude, char *longitude) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    char url[512];
    snprintf(url, sizeof(url), API_URL_COORDS, location, API_KEY);
    printf("DEBUG: URL used for API request: %s\n", url);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            free(s.ptr);
            curl_global_cleanup();
            return 0;
        }

        printf("DEBUG: API Response: %s\n", s.ptr);

        struct json_object *parsed_json;
        struct json_object *coord_obj;
        struct json_object *lat_obj;
        struct json_object *lon_obj;

        parsed_json = json_tokener_parse(s.ptr);
        if (!parsed_json) {
            fprintf(stderr, "DEBUG: Failed to parse JSON response.\n");
        } else if (!json_object_object_get_ex(parsed_json, "coord", &coord_obj)) {
            fprintf(stderr, "DEBUG: 'coord' key not found in JSON response.\n");
        } else if (!json_object_object_get_ex(coord_obj, "lat", &lat_obj) ||
                   !json_object_object_get_ex(coord_obj, "lon", &lon_obj)) {
            fprintf(stderr, "DEBUG: 'lat' or 'lon' keys not found in 'coord' object.\n");
        } else {
            strcpy(latitude, json_object_get_string(lat_obj));
            strcpy(longitude, json_object_get_string(lon_obj));
            printf("DEBUG: Extracted Coordinates - Latitude: %s, Longitude: %s\n", latitude, longitude);
            json_object_put(parsed_json); // Clean up JSON object
            curl_easy_cleanup(curl);
            free(s.ptr);
            curl_global_cleanup();
            return 1;
        }

        json_object_put(parsed_json); // Clean up JSON object
    } else {
        fprintf(stderr, "DEBUG: Failed to initialize cURL.\n");
    }

    curl_easy_cleanup(curl);
    free(s.ptr);
    curl_global_cleanup();
    return 0;
}

int main() {
    char location[100];
    char latitude[20], longitude[20];

    printf("Enter location (city name): ");
    scanf("%s", location);

    if (get_coordinates(location, latitude, longitude)) {
        printf("Coordinates for %s: Latitude = %s, Longitude = %s\n", location, latitude, longitude);
    } else {
        printf("Failed to get coordinates for location: %s\n", location);
    }

    return 0;
}
