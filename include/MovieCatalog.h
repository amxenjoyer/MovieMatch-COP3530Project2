
#ifndef MOVIEMATCH_MOVIECATALOG_H
#define MOVIEMATCH_MOVIECATALOG_H

#include <vector>
#include <memory>
#include <string>
#include <cstddef>
#include <cstdint>

#include "Movie.h"
#include "HashTable.h"
#include "BTree.h"

struct MovieDisplay {
    Movie movie;
    double averageRating = 0.0;
    std::uint32_t ratingCount = 0;
};

struct IndexedRating {
    int userId;
    int movieId;
    double rating;
};

class MovieCatalog {
private:
    std::vector<MovieDisplay> records_;

    // Existing movie indexes
    std::unique_ptr<HashTable> hash_;
    std::unique_ptr<BTree> tree_;

    // New rating indexes
    std::vector<IndexedRating> indexedRatings_;
    std::unique_ptr<HashTable> ratingHash_;
    std::unique_ptr<RatingBTree> ratingTree_;

    MovieCatalog() = default;

public:
    static std::shared_ptr<MovieCatalog> load(
        const std::string& folder
    );

    const std::vector<MovieDisplay>& movies() const {
        return records_;
    }

    // Existing movie lookup methods
    bool hashContains(int movieId) const;
    bool treeContains(int movieId) const;

    // New rating lookup methods
    std::size_t indexedRatingCount() const;

    bool hashContainsRating(int userId, int movieId) const;
    bool treeContainsRating(int userId, int movieId) const;

    const std::vector<IndexedRating>& indexedRatings() const {
        return indexedRatings_;
    }
};

#endif
