#ifndef MOVIEMATCH_RATINGKEY_H
#define MOVIEMATCH_RATINGKEY_H

struct RatingKey {
    int userId;
    int movieId;

    // check if two keys are equal
    bool operator==(const RatingKey& other) const {
        return userId == other.userId && movieId == other.movieId;
    }

    // compare keys for ordering
    bool operator<(const RatingKey& other) const {
        if (userId != other.userId) {
            return userId < other.userId;
        }

        return movieId < other.movieId;
    }
};

#endif //MOVIEMATCH_RATINGKEY_H
