
#include "MovieWindow.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

//making pop-up window

MovieWindow::MovieWindow(QWidget* parent)
    : QWidget(parent) {

    setWindowTitle("MovieMatch");
    resize(1100, 800);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(15);

    //header
    QLabel* title = new QLabel("MOVIEMATCH");
    title->setObjectName("logo");

    QLabel* subtitle = new QLabel(
        "Find your next favorite movie!"
    );

    QHBoxLayout* topRow = new QHBoxLayout();

    loadButton = new QPushButton("Reload Movies");

    topRow->addWidget(title);
    topRow->addStretch();
    topRow->addWidget(loadButton);

    mainLayout->addLayout(topRow);
    mainLayout->addWidget(subtitle);

    //search bar ui
    searchBar = new QLineEdit();
    searchBar->setPlaceholderText(
        "Search for a movie..."
    );
    searchBar->setClearButtonEnabled(true);

    mainLayout->addWidget(searchBar);

    //filter
    QHBoxLayout* filters = new QHBoxLayout();

    genreBox = new QComboBox();
    genreBox->addItems({
        "All Genres", "Action", "Adventure",
        "Animation", "Children", "Comedy",
        "Crime", "Documentary", "Drama",
        "Fantasy", "Film-Noir", "Horror",
        "IMAX", "Musical", "Mystery",
        "Romance", "Sci-Fi", "Thriller",
        "War", "Western"
    });

    sortBox = new QComboBox();
    sortBox->addItems({
        "Highest Rated",
        "Lowest Rated",
        "Most Ratings",
        "Title A-Z"
    });

    structureBox = new QComboBox();
    structureBox->addItems({
        "Hash Table",
        "B-Tree"
    });

    minRatings = new QSpinBox();
    minRatings->setRange(0, 100000000);
    minRatings->setValue(100);
    minRatings->setSingleStep(100);

    resultLimit = new QSpinBox();
    resultLimit->setRange(1, 100);
    resultLimit->setValue(12);

    // add controls
    auto addFilter = [&](const QString& name, QWidget* input) {
        QVBoxLayout* column = new QVBoxLayout();
        column->addWidget(new QLabel(name));
        column->addWidget(input);
        filters->addLayout(column);
    };

    addFilter("Genre", genreBox);
    addFilter("Sort By", sortBox);
    addFilter("Data Structure", structureBox);
    addFilter("Min Ratings", minRatings);
    addFilter("Show Results", resultLimit);

    mainLayout->addLayout(filters);

    // button functianlity
    QPushButton* searchButton =
        new QPushButton("Search Movies");

    QPushButton* resetButton =
        new QPushButton("Reset Filters");

    resetButton->setObjectName("reset");

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(searchButton);
    buttons->addWidget(resetButton);
    buttons->addStretch();

    mainLayout->addLayout(buttons);

    //status
    statusLabel = new QLabel("Loading movie data...");
    resultLabel = new QLabel("Discover Movies");
    timeLabel = new QLabel("");

    resultLabel->setObjectName("section");

    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(resultLabel);
    mainLayout->addWidget(timeLabel);

    //result page
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);

    QWidget* container = new QWidget();
    movieGrid = new QGridLayout(container);

    movieGrid->setSpacing(15);
    movieGrid->setAlignment(Qt::AlignTop);

    scroll->setWidget(container);
    mainLayout->addWidget(scroll);

    //UI styling
    setStyleSheet(R"(
        QWidget {
            background-color: #101827;
            color: white;
            font-family: Segoe UI;
            font-size: 14px;
        }

        QLabel#logo {
            color: #ff607b;
            font-size: 29px;
            font-weight: bold;
        }

        QLabel#section {
            font-size: 20px;
            font-weight: bold;
        }

        QLineEdit, QComboBox, QSpinBox {
            background-color: #263348;
            border: 1px solid #42516b;
            border-radius: 8px;
            padding: 9px;
            color: white;
        }

        QComboBox QAbstractItemView {
            background-color: #263348;
            color: white;
            selection-background-color: #e74765;
        }

        QPushButton {
            background-color: #e74765;
            border: none;
            border-radius: 8px;
            padding: 11px 20px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #ff6984;
        }

        QPushButton#reset {
            background-color: #39465a;
        }

        QFrame#movieCard {
            background-color: #202d40;
            border: 1px solid #43516a;
            border-radius: 12px;
        }

        QFrame#movieCard QLabel {
            background: transparent;
        }

        QScrollArea {
            border: none;
        }
    )");

    // button functianlity
    connect(searchButton, &QPushButton::clicked,
            this, [this]() { searchMovies(); });

    connect(resetButton, &QPushButton::clicked,
            this, [this]() { resetFilters(); });

    connect(loadButton, &QPushButton::clicked,
            this, [this]() { loadMovies(); });

    connect(searchBar, &QLineEdit::returnPressed,
            this, [this]() { searchMovies(); });

    //loading movies
    QTimer::singleShot(0, this, [this]() {
        loadMovies();
    });
}

//get movielens dataset

void MovieWindow::loadMovies() {

    // add the data sets to the data folder
    QDir folder(QApplication::applicationDirPath());
    QString dataPath;

    for (int i = 0; i < 6; i++) {
        QString path = folder.filePath("data");

        if (QFileInfo::exists(path + "/movies.csv") &&
            QFileInfo::exists(path + "/ratings.csv")) {
            dataPath = path;
            break;
        }

        if (!folder.cdUp()) {
            break;
        }
    }

    if (dataPath.isEmpty()) {
        statusLabel->setText("MovieLens dataset not found.");

        QMessageBox::warning(
            this,
            "Missing Dataset",
            "Put movies.csv and ratings.csv "
            "inside the project's data folder."
        );
        return;
    }

    statusLabel->setText("Loading MovieLens data...");
    loadButton->setEnabled(false);

    //message for movies loaded
    QApplication::processEvents();

    try {
        catalog = MovieCatalog::load(
            dataPath.toStdString()
        );

        statusLabel->setText(
            QString("Loaded %1 movies!")
                .arg(catalog->movies().size())
        );

        searchMovies();
    }
    catch (const exception& e) {
        QMessageBox::critical(
            this,
            "Loading Error",
            e.what()
        );
        statusLabel->setText("Failed to load movies.");
    }

    loadButton->setEnabled(true);
}

//movie dislpay

QWidget* MovieWindow::createMovieCard(
    const MovieDisplay& movie
) {
    QFrame* card = new QFrame();
    card->setObjectName("movieCard");
    card->setMinimumWidth(220);

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(8);

    QLabel* title = new QLabel(
        QString::fromStdString(movie.movie.title)
    );
    title->setWordWrap(true);
    title->setStyleSheet(
        "font-size: 17px; font-weight: bold;"
    );

    QLabel* genres = new QLabel(
        QString::fromStdString(movie.movie.genres)
    );
    genres->setWordWrap(true);
    genres->setStyleSheet("color: #b0bbcb;");

    QLabel* rating = new QLabel(
        QString("Rating: %1 / 5")
            .arg(movie.averageRating, 0, 'f', 2)
    );
    rating->setStyleSheet(
        "color: #ffd16b; font-weight: bold;"
    );

    QLabel* count = new QLabel(
        QString("%1 ratings")
            .arg(movie.ratingCount)
    );

    layout->addWidget(title);
    layout->addWidget(genres);
    layout->addStretch();
    layout->addWidget(rating);
    layout->addWidget(count);

    return card;
}

//clear old results

void MovieWindow::clearMovies() {

    while (QLayoutItem* item = movieGrid->takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

//search and filter movies

void MovieWindow::searchMovies() {

    if (!catalog) {
        return;
    }

    clearMovies();

    QString keyword = searchBar->text().trimmed();

    string selectedGenre = "";

    if (genreBox->currentIndex() != 0) {
        selectedGenre =
            genreBox->currentText().toStdString();
    }

    vector<const MovieDisplay*> results;

    // Search all movies
    for (const MovieDisplay& movie : catalog->movies()) {


        if (movie.ratingCount == 0) {
            continue;
        }

        // min ratings
        if (movie.ratingCount <
            static_cast<unsigned int>(
                minRatings->value())) {
            continue;
        }

        // title search
        QString title =
            QString::fromStdString(movie.movie.title);

        if (!title.contains(
                keyword, Qt::CaseInsensitive)) {
            continue;
        }

        // genre filter
        if (!selectedGenre.empty()) {
            string genres = "|" + movie.movie.genres + "|";
            string wanted = "|" + selectedGenre + "|";

            if (genres.find(wanted) == string::npos) {
                continue;
            }
        }

        results.push_back(&movie);
    }

    //hash table vs B-Tree

    auto start = chrono::steady_clock::now();

    int hashFound = 0;

    for (const MovieDisplay* movie : results) {
        if (catalog->hashContains(movie->movie.movieId)) {
            hashFound++;
        }
    }

    auto middle = chrono::steady_clock::now();

    int treeFound = 0;

    for (const MovieDisplay* movie : results) {
        if (catalog->treeContains(movie->movie.movieId)) {
            treeFound++;
        }
    }

    auto finish = chrono::steady_clock::now();

    auto hashTime =
        chrono::duration_cast<chrono::microseconds>(
            middle - start
        ).count();

    auto treeTime =
        chrono::duration_cast<chrono::microseconds>(
            finish - middle
        ).count();

    timeLabel->setText(
        QString("Movie ID lookups: Hash Table %1 us (%2 found) | "
                "B-Tree %3 us (%4 found)")
            .arg(hashTime)
            .arg(hashFound)
            .arg(treeTime)
            .arg(treeFound)
    );

    // keep same structure
    bool useHash = structureBox->currentIndex() == 0;

    vector<const MovieDisplay*> foundMovies;

    for (const MovieDisplay* movie : results) {
        bool found;

        if (useHash) {
            found = catalog->hashContains(
                movie->movie.movieId
            );
        }
        else {
            found = catalog->treeContains(
                movie->movie.movieId
            );
        }

        if (found) {
            foundMovies.push_back(movie);
        }
    }

    //sorting movies

    int choice = sortBox->currentIndex();

    sort(foundMovies.begin(), foundMovies.end(),
        [choice](const MovieDisplay* a,
                 const MovieDisplay* b) {

            if (choice == 0) {
                if (a->averageRating != b->averageRating) {
                    return a->averageRating > b->averageRating;
                }
                if (a->ratingCount != b->ratingCount) {
                    return a->ratingCount > b->ratingCount;
                }
            }

            if (choice == 1) {
                if (a->averageRating != b->averageRating) {
                    return a->averageRating < b->averageRating;
                }
            }

            if (choice == 2) {
                if (a->ratingCount != b->ratingCount) {
                    return a->ratingCount > b->ratingCount;
                }
            }

            if (choice == 3) {
                QString titleA =
                    QString::fromStdString(a->movie.title);
                QString titleB =
                    QString::fromStdString(b->movie.title);

                int compare = QString::compare(
                    titleA, titleB, Qt::CaseInsensitive
                );

                if (compare != 0) {
                    return compare < 0;
                }
            }

            return a->movie.movieId < b->movie.movieId;
        }
    );

    //movie display components

    int count = min(
        static_cast<int>(foundMovies.size()),
        resultLimit->value()
    );

    resultLabel->setText(
        QString("%1 movies found - showing %2")
            .arg(foundMovies.size())
            .arg(count)
    );

    if (count == 0) {

        if (count == 0) {
            QLabel* message = new QLabel(
                "No movies found matching your search.\n\n"
                        "Try checking your spelling"
            );

            message->setAlignment(Qt::AlignCenter);
            message->setWordWrap(true);

            message->setStyleSheet(
                "color: #b0bbcb;"
                "font-size: 16px;"
                "padding: 30px;"
            );

            movieGrid->addWidget(message, 0, 0);
            return;
        }

    }

    for (int i = 0; i < count; i++) {
        int row = i / 3;
        int column = i % 3;

        movieGrid->addWidget(
            createMovieCard(*foundMovies[i]),
            row, column
        );
    }
}

//reseting filters

void MovieWindow::resetFilters() {

    searchBar->clear();
    genreBox->setCurrentIndex(0);
    sortBox->setCurrentIndex(0);
    structureBox->setCurrentIndex(0);

    minRatings->setValue(100);
    resultLimit->setValue(12);

    searchMovies();
}
