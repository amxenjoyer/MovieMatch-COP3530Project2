
#include "MovieCatalog.h"
#include "DataLoader.h"
#include "RatingKey.h"

#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <string>

namespace {

    RatingKey movieKey(int movieId) {
        // Synthetic key for the existing movie index.
        return RatingKey{0, movieId};
    }

}

std::shared_ptr<MovieCatalog> MovieCatalog::load(
    const std::string& folder
) {
    const std::string moviesPath = folder + "/movies.csv";
    const std::string ratingsPath = folder + "/ratings.csv";

    const std::size_t MAX_INDEXED_RATINGS = 150000;

    auto catalog = std::shared_ptr<MovieCatalog>(
        new MovieCatalog()
    );

    // Load movie information.
    std::vector<Movie> movies =
        DataLoader::loadMovies(moviesPath);

    catalog->records_.reserve(movies.size());

    std::unordered_map<int, std::size_t> idToIndex;
    idToIndex.reserve(movies.size());

    for (Movie& movie : movies) {
        std::size_t index = catalog->records_.size();

        idToIndex.emplace(movie.movieId, index);

        catalog->records_.push_back({
            std::move(movie), 0.0, 0
        });
    }

    // Set up rating indexes.
    catalog->indexedRatings_.reserve(MAX_INDEXED_RATINGS);

    catalog->ratingHash_ = std::make_unique<HashTable>(
        MAX_INDEXED_RATINGS * 2 + 1
    );

    catalog->ratingTree_ =
        std::make_unique<RatingBTree>();

    // Read ratings.csv one line at a time.
    std::ifstream file(ratingsPath);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open ratings.csv in: " + folder
        );
    }

    std::vector<double> totals(
        catalog->records_.size(), 0.0
    );

    std::string line;

    // Skip CSV header.
    std::getline(file, line);

    while (std::getline(file, line)) {
        const auto first = line.find(',');

        if (first == std::string::npos) {
            continue;
        }

        const auto second = line.find(',', first + 1);

        if (second == std::string::npos) {
            continue;
        }

        const auto third = line.find(',', second + 1);

        if (third == std::string::npos) {
            continue;
        }

        int userId;
        int movieId;
        double rating;

        // Parse only the values needed from this row.
        try {
            userId = std::stoi(
                line.substr(0, first)
            );

            movieId = std::stoi(
                line.substr(
                    first + 1,
                    second - first - 1
                )
            );

            rating = std::stod(
                line.substr(
                    second + 1,
                    third - second - 1
                )
            );
        }
        catch (const std::exception&) {
            continue;
        }

        // Find this rating's movie.
        auto it = idToIndex.find(movieId);

        if (it == idToIndex.end()) {
            continue;
        }

        // Calculate averages using the full ratings file.
        std::size_t movieIndex = it->second;

        totals[movieIndex] += rating;

        ++catalog->records_[movieIndex].ratingCount;

        // Index 150,000 distinct real rating combinations.
        if (catalog->indexedRatings_.size() <
            MAX_INDEXED_RATINGS) {

            RatingKey key{userId, movieId};

            // Avoid indexing duplicate user/movie pairs.
            std::size_t existingIndex = 0;

            if (catalog->ratingHash_->search(
                    key, existingIndex)) {
                continue;
            }

            std::size_t ratingIndex =
                catalog->indexedRatings_.size();

            catalog->indexedRatings_.push_back({
                userId, movieId, rating
            });

            if (!catalog->ratingHash_->insert(
                    key, ratingIndex)) {
                throw std::runtime_error(
                    "Rating Hash Table insertion failed"
                );
            }

            if (!catalog->ratingTree_->insert(key)) {
                throw std::runtime_error(
                    "Rating B-Tree insertion failed"
                );
            }
        }
    }

    if (file.bad()) {
        throw std::runtime_error(
            "Error while reading ratings.csv"
        );
    }

    // Calculate the average rating for each movie.
    for (std::size_t i = 0;
         i < catalog->records_.size(); ++i) {

        auto count = catalog->records_[i].ratingCount;

        if (count > 0) {
            catalog->records_[i].averageRating =
                totals[i] / count;
        }
    }

    // Build the original movie indexes.
    catalog->hash_ = std::make_unique<HashTable>(
        movies.size() * 2 + 1
    );

    catalog->tree_ = std::make_unique<BTree>();

    for (std::size_t i = 0;
         i < catalog->records_.size(); ++i) {

        int id = catalog->records_[i].movie.movieId;

        if (!catalog->hash_->insert(
                movieKey(id), i)) {
            throw std::runtime_error(
                "Movie Hash Table insertion failed"
            );
        }

        if (!catalog->tree_->insert(id)) {
            throw std::runtime_error(
                "Movie B-Tree insertion failed"
            );
        }
    }

    // Verify both rating indexes contain the same count.
    if (catalog->indexedRatings_.size() !=
            catalog->ratingTree_->size() ||
        catalog->indexedRatings_.size() !=
            catalog->ratingHash_->getSize()) {

        throw std::runtime_error(
            "Rating indexes have different record counts"
        );
    }

    return catalog;
}

// Existing movie lookups

bool MovieCatalog::hashContains(int movieId) const {
    std::size_t ignoredIndex = 0;

    return hash_ &&
        hash_->search(
            movieKey(movieId),
            ignoredIndex
        );
}

bool MovieCatalog::treeContains(int movieId) const {
    return tree_ && tree_->search(movieId);
}

// New rating lookups

std::size_t MovieCatalog::indexedRatingCount() const {
    return indexedRatings_.size();
}

bool MovieCatalog::hashContainsRating(
    int userId, int movieId
) const {
    if (!ratingHash_) {
        return false;
    }

    RatingKey key{userId, movieId};
    std::size_t ratingIndex = 0;

    return ratingHash_->search(key, ratingIndex);
}

bool MovieCatalog::treeContainsRating(
    int userId, int movieId
) const {
    if (!ratingTree_) {
        return false;
    }

    RatingKey key{userId, movieId};

    return ratingTree_->search(key);
}
