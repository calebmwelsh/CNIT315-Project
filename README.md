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
