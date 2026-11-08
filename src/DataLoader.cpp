#include "DataLoader.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
    // split csv line
    std::vector<std::string> parseCSVLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;
        bool insideQuotes = false;

        for (int i = 0; i < line.length(); i++) {
            char current = line[i];

            if (current == '"') {
                if (insideQuotes && i + 1 < line.length() && line[i+1] == '"') {
                    field += '"';
                    i++;
                } else {
                    insideQuotes = !insideQuotes;
                }
            } else if (current == ',' && !insideQuotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += current;
            }
        }

        fields.push_back(field);
        return fields;
    }
}

std::vector<Movie> DataLoader::loadMovies(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open movies file: " + filename);
    }

    std::vector<Movie> movies;
    std::string line;

    // skip csv header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::vector<std::string> fields = parseCSVLine(line);

        if (fields.size() != 3) {
            continue;
        }

        Movie movie;
        movie.movieId = std::stoi(fields[0]);
        movie.title = fields[1];
        movie.genres = fields[2];

        movies.push_back(movie);
    }

    return movies;
}

std::vector<Rating> DataLoader::loadRatings(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open ratings file: " + filename);
    }

    std::vector<Rating> ratings;
    ratings.reserve(32000204);
    std::string line;

    // skip csv header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::vector<std::string> fields = parseCSVLine(line);

        if (fields.size() != 4) {
            continue;
        }

        Rating rating;
        rating.userId = std::stoi(fields[0]);
        rating.movieId = std::stoi(fields[1]);
        rating.rating = std::stof(fields[2]);
        rating.timestamp = std::stoll(fields[3]);

        ratings.push_back(rating);
    }

    return ratings;
}