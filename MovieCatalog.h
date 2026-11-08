
#pragma once

#include "Movie.h"
#include "HashTable.h"
#include "BTree.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// movie info
struct MovieDisplay {
    Movie movie;
    double averageRating = 0.0;
    std::uint32_t ratingCount = 0;
};

class MovieCatalog {
public:
    // load and calculate the movie ratings
    static std::shared_ptr<MovieCatalog> load(
        const std::string& folder
    );

    //return movies
    const std::vector<MovieDisplay>& movies() const {
        return records_;
    }

    // check the data structures for the movie
    bool hashContains(int movieId) const;
    bool treeContains(int movieId) const;

private:
    MovieCatalog() = default;

    std::vector<MovieDisplay> records_;

    std::unique_ptr<HashTable> hash_;
    std::unique_ptr<BTree> tree_;
};
