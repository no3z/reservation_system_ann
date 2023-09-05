#include "gtest/gtest.h"
#include "classes.h"

TEST(MovieTest, getTitle) {
    Movie movie("Inception");
    EXPECT_EQ(movie.getTitle(), "Inception");
}

TEST(RoomTest, RoomInitialization) {
    Room room("Gold");
    EXPECT_EQ(room.getRoomName(), "Gold");
    EXPECT_EQ(room.getPlayingMovie(), nullptr);
    for (int i = 0; i < NUMBER_OF_AVAILABLE_SEATS; i++) {
        EXPECT_TRUE(room.isSeatAvailable(i));
    }
}

TEST(RoomTest, setPlayingMovie) {
    Room room("Gold");
    std::shared_ptr<Movie> movie = std::make_shared<Movie>("Inception");
    room.setPlayingMovie(movie);
    EXPECT_EQ(room.getPlayingMovie()->getTitle(), "Inception");
}

TEST(RoomTest, reserveSeat) {
    Room room("Gold");
    EXPECT_TRUE(room.reserveSeat(5));
    EXPECT_FALSE(room.isSeatAvailable(5));
    EXPECT_FALSE(room.reserveSeat(5)); // Try to reserve again, should fail
}

TEST(TheaterTest, addRoom) {
    Theater theater("Cineplex");
    Room room1("Gold");
    Room room2("Silver");
    theater.addRoom(room1);
    theater.addRoom(room2);
    const auto& rooms = theater.getRooms();
    EXPECT_EQ(rooms.size(), 2);
    EXPECT_EQ(rooms[0].getRoomName(), "Gold");
    EXPECT_EQ(rooms[1].getRoomName(), "Silver");
}