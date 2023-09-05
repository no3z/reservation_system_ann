
#include "classes.h"
#include <json/json.h>

///////////////////////////////////////////////////////////////////////////////////////
/// @brief This is the reservation system interface.
/// Here we add Theaters, rooms and movies and provide a booking mechanism
///////////////////////////////////////////////////////////////////////////////////////

class ReservationSystem
{
public:
    /// @brief Constructs our Reservation System
    /// @param filename of a json file with the theaters definition
    ReservationSystem(const std::string &filename);

    /// @brief Allows to book seats inside a theater movie room
    /// @param theaterName
    /// @param roomName
    /// @param seats
    /// @return
    bool bookSeats(const std::string &theaterName, const std::string &roomName, const std::vector<int> &seats);

    /// @brief Return the whole booking informatino of a theater movie room
    /// @param theaterTitle
    /// @param movieTitle
    /// @return
    Json::Value getBookings(const std::string &theaterTitle, const std::string &movieTitle) const;

    /// @brief Returns all playing movies in ALL theaters
    /// @return
    Json::Value getAllPlayingMoviesJson() const;

    /// @brief Returns a list of theaters showing a specific movie
    /// @param movieTitle
    /// @return
    Json::Value getTheatersShowingMovieJson(const std::string &movieTitle) const;

private:
    ReservationSystem() {}

    void addMovie(std::shared_ptr<Movie> movie);
    void addTheater(const Theater &theater);
    void addRoomToTheater(const std::string &theaterName, const Room &room);

    std::vector<std::shared_ptr<Movie>> movies;
    std::vector<Theater> theaters;
};

///////////////////////////////////////////////////////////////////////////////////////