#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <mutex>

const int NUMBER_OF_AVAILABLE_SEATS = 20;

///////////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents a movie
///////////////////////////////////////////////////////////////////////////////////////

class Movie
{
public:
    /// @brief Represents a cinema movie.
    Movie(const std::string &title) : title(title) {}

    /// @brief Returns the title of this movide
    const std::string &getTitle() const;

private:
    std::string title;
};

///////////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents a room inside a theater where a movie is shown
///////////////////////////////////////////////////////////////////////////////////////

class Room
{
public:
    /// @brief Create a Room with its room name
    /// @param roomName
    Room(const std::string &roomName) : roomName(roomName), seats(NUMBER_OF_AVAILABLE_SEATS, true) {}

    /// @brief a copy constructor to properly handle mutex
    Room(const Room &other)
        : roomName(other.roomName), seats(other.seats), playingMovie(other.playingMovie)
    {
    }

    /// @return the room name
    const std::string &getRoomName() const;

    /// @brief Returns the movie in this room.
    std::shared_ptr<Movie> getPlayingMovie() const;

    /// @brief Sets the current movie to this room
    void setPlayingMovie(std::shared_ptr<Movie> movie);

    /// @brief Checks room for seats available.
    /// @param seatNumber integer that goes from [0 - 'NUMBER_OF_AVAILABLE_SEATS']
    bool isSeatAvailable(int seatNumber) const
    {
        std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
        if (seatNumber >= 0 && seatNumber < seats.size())
        {
            return seats[seatNumber];
        }
        return false;
    }

    /// @brief Books one seat if not already booked
    bool reserveSeat(int seatNumber)
    {
        if (seatNumber >= 0 && seatNumber < seats.size() && seats[seatNumber])
        {
            std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
            seats[seatNumber] = false;
            return true; // Reservation successful
        }
        return false; // Reservation failed
    }

private:
    std::string roomName;
    std::shared_ptr<Movie> playingMovie; // Pointer to a movie
    std::vector<bool> seats;
    mutable std::mutex mutex; // Mutable since isSeatAvailable is const
};

///////////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents a theater location, with one or more rooms
///////////////////////////////////////////////////////////////////////////////////////

class Theater
{
public:
    /// @brief Create a theater class
    Theater(const std::string &name);

    /// @brief Return Theater name    
    const std::string &getName() const;

    /// @brief Adds a room to the theater
    void addRoom(const Room &room);

    /// @brief Gets the room vector for this theater
    const std::vector<Room> &getRooms() const;

private:
    std::string name;
    std::vector<Room> rooms; // Store the rooms in the theater
};
