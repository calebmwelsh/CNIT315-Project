# OpenWeather Statistics in C

This repository contains a C program that retrieves and displays the weather statistics for a specified city from the OpenWeather API. The program allows you to input the city name and fetches the current temperature, weather conditions, and humidity for that city.

## Features

- Fetches the current weather data from OpenWeather API.
- Displays the following information:
  - Temperature (in Celsius)
  - Weather conditions (e.g., Clear, Rain, Cloudy)
  - Humidity (percentage)
- Allows users to input the city name.

## Prerequisites

To run this program, you need:

- **C compiler** (e.g., GCC)
- **libcurl** (for making HTTP requests to OpenWeather API)
- An **OpenWeather API key** (free tier available)

## Setup Instructions

### Step 1: Get an OpenWeather API Key

1. Go to [OpenWeather](https://openweathermap.org/) and create an account.
2. Navigate to your account and get your **API key** (referred to as `API_KEY` in the code).

### Step 2: Install Dependencies

To make HTTP requests, this program uses the `libcurl` library. Follow the steps to install it:

#### On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev
```

#### On Windows:
Follow the instructions in the libcurl download page.

### Step 3: Clone the Repository
Clone the repository to your local machine:

git clone [https://github.com/your-username/openweather-c.git](https://github.com/calebmwelsh/CNIT315-Project.git)
cd CNIT315-Project

### Step 4: Build the Program
Compile the C program using gcc (or another C compiler):

gcc -o main main.c -lcurl

### Step 5: Configure the API Key
In the source code (weather_program.c), replace the placeholder YOUR_API_KEY_HERE with your OpenWeather API key.

#define API_KEY "YOUR_API_KEY_HERE"

### Step 6: Run the Program
Once you've compiled the program, you can run it from the terminal:

./main

The program will prompt you to enter a city name. After entering the city, it will display the weather information for that city.

#### Example Output:
Enter the city name: London
Fetching weather data for London...
Current Temperature: 15.0°C
Weather: Clear sky
Humidity: 65%

## Usage
- Run the program by executing the compiled executable file.
- Input the desired city name when prompted.
- The program will display the weather statistics (temperature, weather condition, and humidity).

## Troubleshooting
- Ensure your API_KEY is valid and properly set in the code.
- Make sure you have an internet connection for API requests to succeed.
- If you get errors regarding libcurl, ensure it's installed correctly and linked during compilation.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
