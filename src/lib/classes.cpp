#include <iostream>
#include "classes.h"

///////////////////////////////////////////////////////////////////////////////
// Movie Implementation
///////////////////////////////////////////////////////////////////////////////

const std::string &Movie::getTitle() const
{
    return title;
}

///////////////////////////////////////////////////////////////////////////////
// Room Implementation
///////////////////////////////////////////////////////////////////////////////

const std::string &Room::getRoomName() const
{
    return roomName;
}

std::shared_ptr<Movie> Room::getPlayingMovie() const
{
    return playingMovie;
}

void Room::setPlayingMovie(std::shared_ptr<Movie> movie)
{
    playingMovie = movie;
}

///////////////////////////////////////////////////////////////////////////////
// Theater Implementation
///////////////////////////////////////////////////////////////////////////////

Theater::Theater(const std::string &name) : name(name) {}

const std::string &Theater::getName() const
{
    return name;
}

void Theater::addRoom(const Room &room)
{
    rooms.push_back(room);
}

const std::vector<Room> &Theater::getRooms() const
{
    return rooms;
}