# Cinema Reservation System

### Multithreaded asynchronous server in C++.

## DEVELOPMENT INSTALLATION

**Testing system**: Ubuntu 22, with cmake, conan2, gtest-dev, doxygen and build-essentials packages installed.

To configure and compile the **server**:

- On workspace directory:

```
conan profile detect 
conan install . -of build-debug -s build_type=Debug -b missing
cd build-debug
cmake -S .. -DCMAKE_TOOLCHAIN_FILE=build-debug/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

Once compiled execute the program like:

- On workspace/build-debug:

```
./ReservationSystem ../src/data/data2.json
```

The server executable expects a [json file with the definition of the theaters, rooms and movies for each room](https://github.com/no3z/reservation_system_ann/blob/main/src/data/data.json) 
Provided are 2 json files in '`src/data`' directory.

To launch the **client** a typical python3 installation is needed.

Go to `./client` directory and launch the client script:

```
python3 client_async.py
```

## ENDUSER INSTALLATION

Both Dockerfile for the client and the server are provided.
A docker-compose file is also provided which will generate and execute both programs.

**Docker-compose steps**:

- On Workspace directory:
```
docker-compose up
```

**Dockerfiles steps**:

- On Workspace directory:
  ```
  docker build -f Dockerfile.server -t reservation_system_server .
  docker build -f Dockerfile.client -t reservation_system_client .

  docker run --network host reservation_system_server /data/data2.json
  docker run --network host reservation_system_client 
  ```

Docker container generation will copy provided '`src/data/data2.json`' into the container `/data/` directory.
To change or modify this file we can mount a file in our container `-v myfile.json:/data/file.json` and pass that as the argument to the docker 'reservation_system_server' run command.



## SERVER TECHNICAL DETAILS
The server code provides a HTTP server designed to handle request related to the movie reservation system.
It is built on [C++ Asio library](https://think-async.com/Asio/) and [JsonCpp](https://github.com/open-source-parsers/jsoncpp)
The server is designed to operate with multiple threads, asynchronous and capable of handling multiple client requests concurrently using a thread pool.

### Reservation system class design:
- A Movie represents a film with its title.
- A Room represents an individual cinema room, keeping track of what movie is currently showing and which seats are available or reserved. It ensures thread-safety when reserving or checking the availability of seats using a mutex.
- A Theater represents a collection of cinema rooms. Each theater has a name and a list of rooms where movies can be shown.
- The ReservationSystem class manages the functionality of our movie theater booking system. Interfaces with theaters, rooms, movies, and provides a mechanism for booking and checking the status of seat reservations.

### Server endpoints

```
Endpoint: /movies
Method: GET
Functionality: Retrieves a list of all movies that are currently playing in all theaters.
Response: Sends a JSON response containing all the currently playing movies or sends a 404 Not Found if the endpoint doesn't match.
```
```
Endpoint: /find
Method: POST
Functionality: Given a specific movie title in the request body, it finds all theaters that are currently showing that movie.
Request Body Example: { "movie": "Some Movie Title" }
Response: Sends a JSON response containing the list of theaters showing the movie.
```

```
Endpoint: /bookings
Method: POST
Functionality: Given a movie title and a theater title in the request body, it retrieves all booking information about the specific movie in the mentioned theater.
Request Body Example: { "movie": "Some Movie Title", "theater": "Some Theater Name" }
Response: Sends a JSON response containing the booking information for the specific movie in the theater.
```

```
Endpoint: /seats
Method: POST
Functionality: Allows a user to book specific seats for a given movie in a specified theater. The request body should contain the movie title, theater name, and a list of seat numbers to be booked.
Request Body Example: { "movie": "Some Movie Title", "theater": "Some Theater Name", "seats": [1, 2, 3, 4] }
Response: If the seat reservation is successful, it sends an HTTP OK response. If there are no available seats, an error response is sent.
```

- Error Handling:
If the request method doesn't match any of the above endpoints or if it isn't GET or POST, it sends a 405 Method Not Allowed response.
The server also contains checks for ensuring that request body content is in the expected format (e.g., ensuring the "seats" is an array of integers).

## CLIENT TECHNICAL DETAILS
A simple python async client is available in the `client/` directory.

It performs the following actions per client:
- GET request to the `/movies` endpoint to fetch a list of movies.
- From the received movie list, a movie is randomly selected.
- POST request is sent to the `/find` endpoint with the selected movie to get a list of theaters showing it.
- A theater is randomly selected from the returned list.
- Randomly generates a set of seat numbers and sends a POST request to the `/seats` endpoint for booking these seats for the selected movie in the chosen theater.
- Sends another POST request to the `/bookings` endpoint to retrieve booking details for the selected movie in the chosen theater.
- Calculates and prints the mean latency of the requests made during the function's execution.


## Example files
### Reservation system json definition

```
{
    "theaters": [
        {
            "name": "Theater A",
            "rooms": [
                {
                    "name": "Room 1",
                    "movie": {
                        "title": "Movie X"
                    }
                },
                {
                    "name": "Room 2",
                    "movie": {
                        "title": "Movie Y"
                    }
                }
            ]
        }
    ]
}
```
