
#include "MovieCatalog.h"
#include "DataLoader.h"
#include "RatingKey.h"

#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace {

    RatingKey movieKey(int movieId) {
        // Use userId 0 as a synthetic key
        // to index movies using the existing HashTable.
        RatingKey key{};
        key.userId = 0;
        key.movieId = movieId;
        return key;
    }

}

std::shared_ptr<MovieCatalog> MovieCatalog::load(
    const std::string& folder
) {
    const std::string moviesPath = folder + "/movies.csv";
    const std::string ratingsPath = folder + "/ratings.csv";

    auto catalog = std::shared_ptr<MovieCatalog>(
        new MovieCatalog()
    );

    // use DataLoader for movies.csv
    std::vector<Movie> movies =
        DataLoader::loadMovies(moviesPath);

    catalog->records_.reserve(movies.size());

    std::unordered_map<int, std::size_t> idToIndex;
    idToIndex.reserve(movies.size());

    for (Movie& movie : movies) {
        auto index = catalog->records_.size();

        idToIndex.emplace(movie.movieId, index);

        catalog->records_.push_back({
            std::move(movie), 0.0, 0
        });
    }

    // read the ratings file
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
    std::getline(file, line);

    while (std::getline(file, line)) {
        const auto first = line.find(',');

        if (first == std::string::npos) {
            continue;
        }

        const auto second =
            line.find(',', first + 1);

        if (second == std::string::npos) {
            continue;
        }

        const auto third =
            line.find(',', second + 1);

        if (third == std::string::npos) {
            continue;
        }

        try {
            int movieId = std::stoi(
                line.substr(
                    first + 1,
                    second - first - 1
                )
            );

            auto it = idToIndex.find(movieId);

            if (it == idToIndex.end()) {
                continue;
            }

            double rating = std::stod(
                line.substr(
                    second + 1,
                    third - second - 1
                )
            );

            totals[it->second] += rating;

            ++catalog->records_[
                it->second
            ].ratingCount;
        }
        catch (const std::exception&) {
        }
    }

    if (file.bad()) {
        throw std::runtime_error(
            "Error while reading ratings.csv"
        );
    }

    //average rating
    for (std::size_t i = 0;
         i < catalog->records_.size(); ++i) {

        auto count =
            catalog->records_[i].ratingCount;

        if (count > 0) {
            catalog->records_[i].averageRating =
                totals[i] / count;
        }
    }

    // movie indexes
    catalog->hash_ = std::make_unique<HashTable>(
        movies.size() * 2 + 1
    );

    catalog->tree_ = std::make_unique<BTree>();

    for (std::size_t i = 0;
         i < catalog->records_.size(); ++i) {

        int id =
            catalog->records_[i].movie.movieId;

        if (!catalog->hash_->insert(
                movieKey(id), i)) {
            throw std::runtime_error(
                "Hash Table insertion failed"
            );
        }

        if (!catalog->tree_->insert(id)) {
            throw std::runtime_error(
                "B-Tree insertion failed"
            );
        }
    }

    return catalog;
}

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
