#ifndef MOVIEMATCH_RATING_H
#define MOVIEMATCH_RATING_H

struct Rating {
    int userId;
    int movieId;
    float rating;
    long long timestamp;
};

#endif //MOVIEMATCH_RATING_H
