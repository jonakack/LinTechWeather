

Anropa server via webbläsaren med en av dessa länkar:
http://localhost:8080/api/v1/geo?city=Stockholm
http://localhost:8080/api/v1/weather?lat=59.3293&lon=18.0686
http://localhost:8080/invalidpath

Eller via en ny terminal med detta kommando och curl installerat:
curl -l '127.0.0.1:8080/api/v1/geo?city=Stockholm'
curl -l '127.0.0.1:8080/api/v1/weather?lat=59.3293&lon=18.0686'
curl -l '127.0.0.1:8080/api/v1/invalidpath'
