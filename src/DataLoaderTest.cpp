#include <iostream>
#include "DataLoader.h"

int main() {
    try {
        std::vector<Movie> movies = DataLoader::loadMovies("../data/movies.csv");
        std::cout << "Movies loaded: " << movies.size() << std::endl;

        std::vector<Rating> ratings = DataLoader::loadRatings("../data/ratings.csv");
        std::cout << "Ratings Loaded: " << ratings.size() << std::endl;

        // show first movie
        if (!movies.empty()) {
            std::cout << "\nFirst movie: " << movies[0].title << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}