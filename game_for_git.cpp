#include <bits/stdc++.h>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int get_rand(int l, int r) {
    int d = r - l + 1;
    return (rng() % d) + l;
}

const int M = 10000; // chance: [0; 10000] = [0; 1]

struct Game {
    enum game_status {
        NOT_STARTED,
        ONGOING,
        FINISHED
    };

    enum game_mode {
        EASY,
        MEDIUM,
        HARD,
        VERY_HARD
    };

    struct Board {
        struct Cell {
            bool good;
            bool opened;

            Cell() {good = true, opened = false;}
            Cell(bool is_good) {good = is_good, opened = false;}

            bool is_good() {return good;}
            bool is_opened() {return opened;}

            void open() {opened = true;}
            void set_good(bool is_good) {good = is_good;}
        };

        int n, m;
        int goods_chance;
        int passed;
        vector <vector <Cell>> board;

        Board() {
            n = m = 0;
            board = {};
            goods_chance = M;
            passed = 0;
        }

        void init(int rows, int cols, int goods_c) {
            n = rows, m = cols;
            goods_chance = goods_c;

            assert(0 < n and 0 < m);
            assert(0 <= goods_chance and goods_chance <= M);

            board.resize(n, vector <Cell> (m));
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    bool is_good = get_rand(0, M) < goods_chance;
                    board[i][j].set_good(is_good);
                }
            }
        }

        bool open_cell(int i, int j) {
            assert(0 <= i and i < n);
            assert(0 <= j and j < m);

            bool res = board[i][j].is_good();
            open_row(i);
            return res;
        }

        void open_row(int i) {
            assert(0 <= i and i < n);

            for (int j = 0; j < m; j++)
                board[i][j].open();
        }

        int get_passed() {return passed;}
    };

    int get_chance(game_mode gamemode) {
        if (gamemode == game_mode::EASY) {
            return 7500;
        } else if (gamemode == game_mode::MEDIUM) {
            return 5000;
        } else if (gamemode == game_mode::HARD) {
            return 3750;
        } else if (gamemode == game_mode::VERY_HARD) {
            return 2500;
        } else assert(false);

        return -1;
    }

    Game() {status = game_status::NOT_STARTED;};

    Board board;
    game_status status;
    game_mode mode;
    float bet;
    int passed;
    float income;

    Board get_board() {return board;}
    game_status get_game_status() {return status;}
    float get_bet() {return bet;}
    float get_income() {return income;}

    void start(int rows, int cols, float set_bet, game_mode set_mode) {
        mode = set_mode;
        bet = set_bet;
        float chance = get_chance(mode);
        board.init(rows, cols, chance);
        income = 0;
        status = game_status::ONGOING;
    }

    bool make_move(int i, int j) {
        assert(get_game_status() == game_status::ONGOING);

        bool res = board.open_cell(i, j);
        return res;
    }

    float end() {
        assert(get_game_status() != game_status::FINISHED);

        // calculate income

        status = game_status::FINISHED;
        return income;
    }
};

struct Account {
    float balance;
    vector <Game> history;
    Game current_game;

    // default
    float default_bet;
    int default_rows, default_cols;
    Game::game_mode default_mode;

    Account() {
        balance = 0;
        history = {};
        current_game = Game();

        default_bet = 0;
        default_rows = 6;
        default_cols = 4;
        default_mode = Game::game_mode::MEDIUM;
    }

    void set_default_mode(Game::game_mode new_mode) {default_mode = new_mode;}
    void set_default_size(int new_rows, int new_cols) {
        assert(-1 < new_rows and -1 < new_cols);
        if (new_rows > 0)
            default_rows = new_rows;
        if (new_cols > 0)
            default_cols = new_cols;
    }
    void set_default_bet(float new_bet) {default_bet = new_bet;}
    void set_balance(float new_bal) {balance = new_bal;}
    void add_balance(float added) {balance += added;}

    void end_current_game() {
        balance += current_game.get_income();
        history.push_back(current_game);
        current_game.status = Game::NOT_STARTED;
    }

    void start_new_game() {
        end_current_game();
        auto [rows, cols] = get_default_size();
        current_game.start(rows, cols, bet, Game::EASY);
    }

    Game::game_mode get_default_mode() {return default_mode;}
    pair <int, int> get_default_size() {return {default_rows, default_cols};}
    float get_default_bet() {return default_bet;}
    float get_balance() {return balance;}
};

struct Gameplay {
    Account account;
    Gameplay() {
        account.set_balance(100.0);
        account.set_default_bet(10.0);
        account.set_default_size(6, 4);
        account.set_default_mode(Game::game_mode::MEDIUM);

        cout << "Witaj w grze! Twoje konto zostało utworzone z początkowym saldem 100 jednostek.\n";
        cout << "Domyślny rozmiar planszy to 6x4, poziom trudności to MEDIUM, a stawka to 10 jednostek.\n";
    }
    void start() {
    while (true) {
        cout << "\nTwój balans: " << account.get_balance() << " jednostek.\n";
        cout << "Wybierz jedną z opcji:\n";
        cout << "1. Rozpocznij nową grę\n";
        cout << "2. Wykonaj ruch (otwórz komórkę)\n";
        cout << "3. Zakończ bieżącą grę\n";
        cout << "4. Wyjdź z gry\n";
        cout << "5. Zmień domyślne ustawienia\n";

        int choice;
        cin >> choice;

        if (choice == 1) {
            account.start_new_game();
            cout << "Nowa gra rozpoczęta!\n";
        } else if (choice == 2) {
            if (account.current_game.get_game_status() != Game::game_status::ONGOING) {
                cout << "Nie masz aktywnej gry. Rozpocznij nową grę, aby kontynuować.\n";
                continue;
            }
            int i, j;
            cout << "Podaj współrzędne komórki (wiersz i kolumna): ";
            cin >> i >> j;
            if (account.current_game.make_move(i, j)) {
                cout << "Dobra komórka! Kontynuuj.\n";
            } else {
                cout << "Zła komórka! Uważaj.\n";
            }
        } else if (choice == 3) {
            account.end_current_game();
            cout << "Gra zakończona. Twój aktualny balans: " << account.get_balance() << " jednostek.\n";
        } else if (choice == 4) {
            cout << "Dziękujemy za grę! Do zobaczenia.\n";
            break;
        } else if (choice == 5) {
            cout << "Zmień ustawienia gry:\n";
            cout << "1. Domyślny poziom trudności\n";
            cout << "2. Domyślna stawka\n";
            cout << "3. Domyślny rozmiar planszy\n";
            int subChoice;
            cin >> subChoice;

            if (subChoice == 1) {
                int newMode;
                cout << "Wybierz poziom trudności (0-EASY, 1-MEDIUM, 2-HARD, 3-VERY_HARD): ";
                cin >> newMode;
                account.set_default_mode(static_cast<Game::game_mode>(newMode));
                cout << "Zmieniono domyślny poziom trudności.\n";
            } else if (subChoice == 2) {
                float newBet;
                cout << "Podaj nową domyślną stawkę: ";
                cin >> newBet;
                account.set_default_bet(newBet);
                cout << "Zmieniono domyślną stawkę.\n";
            } else if (subChoice == 3) {
                int newRows, newCols;
                cout << "Podaj nowy rozmiar planszy (wiersze i kolumny): ";
                cin >> newRows >> newCols;
                account.set_default_size(newRows, newCols);
                cout << "Zmieniono domyślny rozmiar planszy.\n";
            } else {
                cout << "Nieprawidłowa opcja.\n";
            }
        } else {
            cout << "Nieprawidłowy wybór. Spróbuj ponownie.\n";
        }
    }
}
};

signed main() {
    Gameplay gameplay;
    gameplay.start();
}
