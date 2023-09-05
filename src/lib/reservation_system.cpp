
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

#include "reservation_system.h"

///////////////////////////////////////////////////////////////////////////////

ReservationSystem::ReservationSystem(const std::string &filename)
{
    std::ifstream jsonFile(filename);
    Json::Value root;
    jsonFile >> root;

    const Json::Value &theatersJson = root["theaters"];
    for (const auto &theaterJson : theatersJson)
    {
        std::string theaterName = theaterJson["name"].asString();
        Theater theater(theaterName);

        const Json::Value &roomsJson = theaterJson["rooms"];
        for (const auto &roomJson : roomsJson)
        {
            std::string roomName = roomJson["name"].asString();
            std::string movieTitle = roomJson["movie"]["title"].asString();
            Movie movie(movieTitle);
            Room room(roomName);
            room.setPlayingMovie(std::make_shared<Movie>(movie));
            theater.addRoom(room);
        }
        theaters.push_back(theater);
    }
}

///////////////////////////////////////////////////////////////////////////////

void ReservationSystem::addMovie(std::shared_ptr<Movie> movie)
{
    movies.push_back(movie);
}

///////////////////////////////////////////////////////////////////////////////

void ReservationSystem::addTheater(const Theater &theater)
{
    theaters.push_back(theater);
}

///////////////////////////////////////////////////////////////////////////////

void ReservationSystem::addRoomToTheater(const std::string &theaterName, const Room &room)
{
    for (auto &theater : theaters)
    {
        if (theater.getName() == theaterName)
        {
            theater.addRoom(room);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

Json::Value ReservationSystem::getBookings(const std::string &theaterTitle, const std::string &movieTitle) const
{
    Json::Value bookings(Json::arrayValue);

    for (const auto &theater : theaters)
    {
        if (theater.getName() == theaterTitle)
        {
            for (const auto &room : theater.getRooms())
            {
                if (room.getPlayingMovie() && room.getPlayingMovie()->getTitle() == movieTitle)
                {
                    Json::Value roomBookings(Json::arrayValue);
                    for (int seatNumber = 0; seatNumber < NUMBER_OF_AVAILABLE_SEATS; ++seatNumber)
                    {
                        roomBookings.append(room.isSeatAvailable(seatNumber) ? 0 : 1);
                    }
                    bookings.append(roomBookings);
                }
            }
        }
    }
    return bookings;
}

///////////////////////////////////////////////////////////////////////////////

bool ReservationSystem::bookSeats(const std::string &theaterName, const std::string &movieName, const std::vector<int> &in_seats)
{
    for (auto &theater : theaters)
    {
        if (theater.getName() == theaterName)
        {
            for (auto &room : theater.getRooms())
            {
                if (room.getPlayingMovie() && room.getPlayingMovie()->getTitle() == movieName)
                {
                    // Check if ALL seats are available and reserve them
                    bool allSeatsAvailable = true;
                    std::vector<int> seats(in_seats);
                    // Sort the vector to bring duplicates together
                    std::sort(seats.begin(), seats.end());

                    // Use std::unique to rearrange elements and return the end of unique range
                    auto uniqueEnd = std::unique(seats.begin(), seats.end());

                    // Erase elements after the unique range
                    seats.erase(uniqueEnd, seats.end());
                    for (int seatNumber : seats)
                    {
                        if (!room.isSeatAvailable(seatNumber))
                        {
                            allSeatsAvailable = false; // At least one seat is not available
                            break;                     // Exit the loop, no need to check the rest
                        }
                    }

                    if (allSeatsAvailable)
                    {
                        // Reserve all the available seats
                        for (int seatNumber : seats)
                        {
                            if (!const_cast<Room &>(room).reserveSeat(seatNumber))
                            {
                                return false; // Midoperation error
                            }
                        }
                        return true; // Booking successful
                    }
                    else
                    {
                        return false; // At least one seat is not available
                    }
                }
            }
        }
    }
    return false; // No matching theater or room
}

///////////////////////////////////////////////////////////////////////////////

Json::Value ReservationSystem::getAllPlayingMoviesJson() const
{
    Json::Value movieTitles(Json::arrayValue);
    // Iterate through theaters and add movie titles
    for (const auto &theater : theaters)
    {
        for (const auto &room : theater.getRooms())
        {
            if (room.getPlayingMovie())
            {
                movieTitles.append(room.getPlayingMovie()->getTitle());
            }
        }
    }
    return movieTitles;
}

///////////////////////////////////////////////////////////////////////////////

Json::Value ReservationSystem::getTheatersShowingMovieJson(const std::string &movieTitle) const
{
    Json::Value theatersJson(Json::arrayValue);

    for (const auto &theater : theaters)
    {
        for (const auto &room : theater.getRooms())
        {
            if (room.getPlayingMovie() && room.getPlayingMovie()->getTitle() == movieTitle)
            {
                theatersJson.append(theater.getName());
                break; // Add theater only once if it has multiple rooms showing the same movie
            }
        }
    }

    return theatersJson;
}

///////////////////////////////////////////////////////////////////////////////