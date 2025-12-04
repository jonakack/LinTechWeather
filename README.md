# LinTechWeather TCP/HTTP Server/Client, API
Under development.

# Running Server Instructions

## Quick Start

### Production Server (Port 10380)
```bash
# Start server in current terminal (blocking)
make run

# Or start server in detached tmux session (background)
make run-tmux
```

### Local Development Server (Port 8080)
```bash
# Start local development server (current terminal)
make run-local
```

## Server Management

### Check if tmux server is running
```bash
tmux list-sessions
```

### Attach to see server output
```bash
tmux attach -t weather-server
```

### Detach from tmux session (server keeps running)
Press: **Ctrl+B**, then **D**

### Stop the tmux server
```bash
make kill-server
```

## Port Information

| Command | Port | Purpose |
|---------|------|---------|
| `make run` / `make run-tmux` | **10380** | Production deployment |
| `make run-local` | **8080** | Local development |

## Tips

- Use **`make run-local`** for development - easier to access on port 8080
- Use **`make run-tmux`** for production - server runs in background
- Use **`make run`** for debugging - see output directly in terminal

# Step-by-step guide:

You call this link and replace `<cityname>` with the city you want to search for:

```
IF LOCAL: http://localhost:8080/api/v1/geo?city=<cityname>

IF LIVE: http://kontoret.onvo.se:10380/api/v1/geo?city=<cityname>
```

The server responds (if the name is a valid city) with for example:
```json
{ "city":"cityname","country":"country","lat":"latitude","lon":"longitude" }
```

You call the next link with these coordinates:

```
IF LOCAL: http://localhost:8080/api/v1/weather?lat=<latitude>&lon=<longitude>

IF LIVE: http://kontoret.onvo.se:10380/api/v1/weather?lat=<latitude>&lon=<longitude>
```

The server responds with for example:
```json
{ "tempC":"temperature","weather_code":"weathercode","updatedAt":"dateTimeZ" }
```

NOTE: Assume that the time is currently GMT. This may change later in development. 

## Example of error response:
```json
{ "error": { "code": 404, "message": "city not found" } }
```

## Call server via browser with one of these links:
http://localhost:8080/api/v1/geo?city=Stockholm

http://localhost:8080/api/v1/weather?lat=59.3293&lon=18.0686

http://localhost:8080/invalidpath

http://kontoret.onvo.se:10380/api/v1/geo?city=Stockholm

http://kontoret.onvo.se:10380/api/v1/weather?lat=59.3293&lon=18.0686


## Or via terminal with this command and curl installed:
curl -l '127.0.0.1:8080/api/v1/geo?city=Stockholm'

curl -l '127.0.0.1:8080/api/v1/weather?lat=59.3293&lon=18.0686'

curl -l '127.0.0.1:8080/api/v1/invalidpath'

## First Endpoint
### Called with for example "/api/v1/geo?city=Stockholm". Then the server responds this to the client:
{ "city":"Stockholm","country":"SE","lat":59.3293,"lon":18.0686 }

## Second Endpoint
### Called with for example "/api/v1/weather?lat=59.3293&lon=18.0686". Then the server responds this to the client:
{ "tempC":7.0,"weather_code":"0","updatedAt":"2025-11-02T09:00:00Z" }

# Weather Code Description
### The weather code given has the following meaning:
0 Clear sky

1, 2, 3 Mainly clear, partly cloudy, and overcast

45, 48 Fog and depositing rime fog

51, 53, 55 Drizzle: Light, moderate, and dense intensity

56, 57 Freezing Drizzle: Light and dense intensity

61, 63, 65 Rain: Slight, moderate and heavy intensity

66, 67 Freezing Rain: Light and heavy intensity

71, 73, 75 Snow fall: Slight, moderate, and heavy intensity

77 Snow grains

80, 81, 82 Rain showers: Slight, moderate, and violent

85, 86 Snow showers slight and heavy

95 * Thunderstorm: Slight or moderate


96, 99 * Thunderstorm with slight and heavy hail
