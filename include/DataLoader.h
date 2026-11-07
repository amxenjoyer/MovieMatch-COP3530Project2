#ifndef MOVIEMATCH_DATALOADER_H
#define MOVIEMATCH_DATALOADER_H

#include <string>
#include <vector>

#include "Movie.h"
#include "Rating.h"

class DataLoader {
public:
    // load movies from movies.csv
    static std::vector<Movie> loadMovies(const std::string& filename);

    // load ratings from ratings.csv
    static std::vector<Rating> loadRatings(const std::string& filename);
};

#endif //MOVIEMATCH_DATALOADER_H
