#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <json-c/json.h> // For JSON parsing

#define API_KEY "9d138ef758378bf9d63431c3d35cda61"
#define API_URL_CURRENT "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric"
#define API_URL_HISTORICAL "http://api.openweathermap.org/data/2.5/forcast?lat=%s&lon=%s&dt=%ld&appid=%s&units=imperial"
#define API_URL_GEOCODE "http://api.openweathermap.org/geo/1.0/direct?q=%s&appid=%s"

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

// Function to fetch weather data
void fetch_weather(const char *url) {
    CURL *curl;
    CURLcode res;
    struct string s;
    
    init_string(&s);

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
            // Parse JSON response
            struct json_object *parsed_json;
            struct json_object *coord_obj, *weather_array, *main_obj, *wind_obj, *clouds_obj, *sys_obj;
            struct json_object *lon_obj, *lat_obj, *temp_obj, *feels_like_obj, *humidity_obj, *weather_desc_obj;
            struct json_object *speed_obj, *deg_obj, *all_obj, *sunrise_obj, *sunset_obj;

            parsed_json = json_tokener_parse(s.ptr);
            if (!parsed_json) {
                fprintf(stderr, "Failed to parse JSON response.\n");
            } else {
                // Extract and print weather data
                printf("\n--- Weather Report ---\n");

                // Coordinates
                json_object_object_get_ex(parsed_json, "coord", &coord_obj);
                if (coord_obj) {
                    json_object_object_get_ex(coord_obj, "lon", &lon_obj);
                    json_object_object_get_ex(coord_obj, "lat", &lat_obj);
                    printf("Coordinates: Longitude: %s, Latitude: %s\n",
                           json_object_to_json_string(lon_obj),
                           json_object_to_json_string(lat_obj));
                }

                // Weather description
                json_object_object_get_ex(parsed_json, "weather", &weather_array);
                if (weather_array) {
                    struct json_object *weather_obj = json_object_array_get_idx(weather_array, 0);
                    json_object_object_get_ex(weather_obj, "description", &weather_desc_obj);
                    printf("Weather: %s\n", json_object_get_string(weather_desc_obj));
                }

                // Temperature and Humidity
                json_object_object_get_ex(parsed_json, "main", &main_obj);
                if (main_obj) {
                    json_object_object_get_ex(main_obj, "temp", &temp_obj);
                    json_object_object_get_ex(main_obj, "feels_like", &feels_like_obj);
                    json_object_object_get_ex(main_obj, "humidity", &humidity_obj);
                    printf("Temperature: %.2f°C\n", json_object_get_double(temp_obj));
                    printf("Feels Like: %.2f°C\n", json_object_get_double(feels_like_obj));
                    printf("Humidity: %d%%\n", json_object_get_int(humidity_obj));
                }

                // Wind
                json_object_object_get_ex(parsed_json, "wind", &wind_obj);
                if (wind_obj) {
                    json_object_object_get_ex(wind_obj, "speed", &speed_obj);
                    json_object_object_get_ex(wind_obj, "deg", &deg_obj);
                    printf("Wind: %.2f m/s at %d°\n",
                           json_object_get_double(speed_obj),
                           json_object_get_int(deg_obj));
                }

                // Clouds
                json_object_object_get_ex(parsed_json, "clouds", &clouds_obj);
                if (clouds_obj) {
                    json_object_object_get_ex(clouds_obj, "all", &all_obj);
                    printf("Cloud Cover: %d%%\n", json_object_get_int(all_obj));
                }

                // Sunrise and Sunset
                json_object_object_get_ex(parsed_json, "sys", &sys_obj);
                if (sys_obj) {
                    json_object_object_get_ex(sys_obj, "sunrise", &sunrise_obj);
                    json_object_object_get_ex(sys_obj, "sunset", &sunset_obj);

                    time_t sunrise = json_object_get_int64(sunrise_obj);
                    time_t sunset = json_object_get_int64(sunset_obj);

                    printf("Sunrise: %s", ctime(&sunrise));
                    printf("Sunset: %s", ctime(&sunset));
                }

                printf("----------------------\n");
                json_object_put(parsed_json); // Free JSON object
            }
        }

        curl_easy_cleanup(curl);
    }

    free(s.ptr);
    curl_global_cleanup();
}


// Function to fetch current weather
void get_current_weather(const char *location) {
    char url[512];
    snprintf(url, sizeof(url), API_URL_CURRENT, location, API_KEY);
    printf("Fetching current weather for %s...\n", location);
    fetch_weather(url);
}

// Function to fetch weather by date
void get_weather_by_date(const char *latitude, const char *longitude, time_t timestamp) {
    char url[512];
    snprintf(url, sizeof(url), API_URL_HISTORICAL, latitude, longitude, timestamp, API_KEY);
    printf("Fetching weather for the specified date (timestamp: %ld)...\n", timestamp);
    fetch_weather(url);
}

// Function to fetch coordinates based on location (Replaced existing function)
int get_coordinates(const char *location, char *latitude, char *longitude) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    char url[512];
    snprintf(url, sizeof(url), API_URL_GEOCODE, location, API_KEY);
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
        struct json_object *lat_obj;
        struct json_object *lon_obj;

        parsed_json = json_tokener_parse(s.ptr);
        if (!parsed_json) {
            fprintf(stderr, "DEBUG: Failed to parse JSON response.\n");
        } else if (json_object_array_length(parsed_json) == 0) {
            fprintf(stderr, "DEBUG: No results found for the location.\n");
        } else {
            struct json_object *first_result = json_object_array_get_idx(parsed_json, 0);
            if (!json_object_object_get_ex(first_result, "lat", &lat_obj) ||
                !json_object_object_get_ex(first_result, "lon", &lon_obj)) {
                fprintf(stderr, "DEBUG: 'lat' or 'lon' keys not found in JSON response.\n");
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

// Main function
int main() {
    char location[100];
    char latitude[20], longitude[20];
    int choice;

    printf("Enter location (city name): ");
    scanf("%s", location);

    if (!get_coordinates(location, latitude, longitude)) {
        fprintf(stderr, "Failed to get coordinates for location: %s\n", location);
        return 1;
    }

    printf("Coordinates for %s: Latitude=%s, Longitude=%s\n", location, latitude, longitude);

    printf("\nChoose an option:\n");
    printf("1. View current weather\n");
    printf("2. View weather for a specific date in the past\n");
    printf("3. View tomorrow's weather\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        get_current_weather(location);
    } else if (choice == 2) {
        int year, month, day;
        printf("Enter date (YYYY MM DD): ");
        scanf("%d %d %d", &year, &month, &day);

        struct tm date = {0};
        date.tm_year = year - 1900;
        date.tm_mon = month - 1;
        date.tm_mday = day;
        time_t timestamp = mktime(&date);

        if (timestamp == -1) {
            fprintf(stderr, "Invalid date provided.\n");
        } else {
            get_weather_by_date(latitude, longitude, timestamp);
        }
    } else if (choice == 3) {
        time_t now = time(NULL);
        time_t tomorrow = now + 24 * 60 * 60;

        get_weather_by_date(latitude, longitude, tomorrow);
    } else {
        printf("Invalid choice. Exiting.\n");
    }

    return 0;
}
