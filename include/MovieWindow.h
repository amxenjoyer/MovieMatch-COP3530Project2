
#ifndef MOVIEWINDOW_H
#define MOVIEWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

#include <memory>
#include "MovieCatalog.h"

class MovieWindow : public QWidget {
private:
    // movie data
    std::shared_ptr<MovieCatalog> catalog;

    // UI components
    QLineEdit* searchBar;
    QComboBox* genreBox;
    QComboBox* sortBox;
    QComboBox* structureBox;
    QSpinBox* minRatings;
    QSpinBox* resultLimit;

    QLabel* statusLabel;
    QLabel* resultLabel;
    QLabel* timeLabel;

    QGridLayout* movieGrid;
    QPushButton* loadButton;

    // functions
    void loadMovies();
    void searchMovies();
    void resetFilters();
    void clearMovies();
    QWidget* createMovieCard(const MovieDisplay& movie);

public:
    explicit MovieWindow(QWidget* parent = nullptr);
};

#endif
