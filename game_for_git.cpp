#include <bits/stdc++.h>

using namespace std;

bool DEBUG = false;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int get_rand(int l, int r) {
    int d = r - l + 1;
    return (rng() % d) + l;
}

string rjust(string s, int k) {
    if (s.size() < k)
        s = string(k - int(s.size()), ' ') + s;
    return s;
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
        HARD
    };

    enum move_result {
        OK,
        LOSE,
        WIN
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
        int goods_cnt;
        int passed;
        vector <vector <Cell>> board;

        Board() {
            n = m = 0;
            board = {};
            goods_cnt = 0;
            passed = 0;
        }

        pair <int, int> get_size() {return pair <int, int> {n, m};}

        void init(int rows, int cols, int goods_c) {
            n = rows, m = cols;
            goods_cnt = goods_c;

            assert(0 < n and 0 < m);
            assert(0 < goods_cnt and goods_cnt < m);

            board.resize(n, vector <Cell> (m));
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++)
                    board[i][j].set_good(false);

                vector <int> unused(m);
                iota(unused.begin(), unused.end(), 0);
                for (int rep = 0; rep < goods_cnt; rep++) {
                    int ind = get_rand(0, unused.size() - 1);
                    int j = unused[ind];
                    unused.erase(unused.begin() + j, unused.begin() + j + 1);
                    board[i][j].set_good(true);
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
        bool inc_passed() {++passed; return passed == n;}
    };

    int get_chance(game_mode gamemode) {
        if (gamemode == game_mode::EASY) {
            return 7500;
        } else if (gamemode == game_mode::MEDIUM) {
            return 5000;
        } else if (gamemode == game_mode::HARD) {
            return 2500;
        } else assert(false);

        return -1;
    }

    int get_cnt(game_mode gamemode) {
        if (gamemode == game_mode::EASY) {
            return 3;
        } else if (gamemode == game_mode::MEDIUM) {
            return 2;
        } else if (gamemode == game_mode::HARD) {
            return 1;
        } else assert(false);

        return -1;
    }

    Game() {status = game_status::NOT_STARTED;};

    Board board;
    game_status status;
    game_mode mode;
    float bet;
    float income;

    Board get_board() {return board;}
    game_status get_game_status() {return status;}
    float get_bet() {return bet;}
    float get_income() {return income;}

    void start(int rows, int cols, float set_bet, game_mode set_mode) {
        mode = set_mode;
        bet = set_bet;
        int cnt = get_cnt(mode);
        board.init(rows, cols, cnt);
        income = 0;
        status = game_status::ONGOING;
    }

    bool finished() {
        int n = board.get_size().first;
        int passed = board.get_passed();
        return passed == n;
    }

    float calculate_income() {
        int passed = board.get_passed();
        float ans = bet * 0.98 / pow(get_chance(mode) * 1. / M, passed);
        // cout << get_mode_txt() << ' ' << get_chance(mode) << ' ' << bet << ' ' << passed << ' ' << ans << '\n';
        // cout.flush();
        return ans;
    }

    move_result make_move(int j) {
        assert(get_game_status() == game_status::ONGOING);

        int n = board.get_size().first;

        int passed = board.get_passed();
        assert(0 <= passed and passed < n);
        bool res = board.open_cell(passed, j);

        income = calculate_income();

        if (res) {
            bool win = board.inc_passed();
            return win ? move_result::WIN : move_result::OK;
        } else {
            income = 0;
            return move_result::LOSE;
        }
    }
    
    void end(bool win) {
        assert(get_game_status() != game_status::FINISHED);
        if (!win)
            assert(income == 0);
        status = game_status::FINISHED;
    }

    string get_mode_txt() {
        if (mode == game_mode::EASY)
            return "Easy";
        if (mode == game_mode::MEDIUM)
            return "Medium";
        if (mode == game_mode::HARD)
            return "Hard";
        // if (mode == game_mode::VERY_HARD)
        //     return "Very hard";
        assert(false);
        return "???";
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
        auto was_status = current_game.status;
        current_game.end(true);
        add_balance(current_game.get_income());
        if (was_status != Game::game_status::NOT_STARTED)
            history.push_back(current_game);
        current_game = Game();
    }

    bool start_new_game() {
        float bet = get_default_bet();
        float bal = get_balance();
        if (bal < bet)
            return false;
        end_current_game();
        auto [rows, cols] = get_default_size();
        Game::game_mode mode = get_default_mode();
        assert(bal >= bet);
        add_balance(-bet);
        current_game.start(rows, cols, bet, mode);
        return true;
    }

    Game::Board get_board() {
        assert(current_game.status != Game::game_status::NOT_STARTED);
        return current_game.get_board();
    }

    vector <string> get_board_txt() {
        auto board = get_board();

        int cur_i = board.get_passed();
        auto [n, m] = board.get_size();

        vector <string> txt(n, string(m, '?'));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (DEBUG) {
                    txt[i][j] = board.board[i][j].is_good() ? '#' : '.';
                    continue;
                }
                if (i == cur_i) {
                    if (j == 0 or j == m - 1)
                        txt[i][j] = (j == 0 ? '>' : '<');
                    else
                        txt[i][j] = '-';
                    continue;
                }
                if (!board.board[i][j].is_opened())
                    continue;
                txt[i][j] = board.board[i][j].is_good() ? '#' : '.';
            }
        }

        return txt;
    }

    Game::move_result make_move(int j) {
        assert(current_game.status == Game::game_status::ONGOING);
        Game::move_result result = current_game.make_move(j);

        if (result == Game::move_result::WIN) {
            current_game.end(true);
            end_current_game();
        } else if (result == Game::move_result::LOSE) {
            current_game.end(false);
            end_current_game();
        }

        return result;
    }

    vector <Game> get_history() {return history;}
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
        cout << "6. Wypisz historię gier\n";
        cout << '\n';

        int choice;
        cin >> choice;

        assert(account.current_game.get_game_status() != Game::game_status::FINISHED);

        if (choice == 1) {
            if (account.current_game.get_game_status() == Game::game_status::ONGOING) {
                cout << "Masz aktywną grę. Zakończ ją, aby kontynuować.\n";
                continue;
            }
            
            bool started = account.start_new_game();
            if (!started) {
                cout << "Nie możesz zacząć nową grę przez niedostatni balans konta." << '\n';
                continue;
            }

            cout << "Nowa gra rozpoczęta!\n";
            float bal = account.get_balance();
            float bet = account.get_default_bet();
            cout << "Twój balans: " << bal;
            cout << "; twoja stawka: " << bet << '\n';
        } else if (choice == 2) {
            if (account.current_game.get_game_status() != Game::game_status::ONGOING) {
                cout << "Nie masz aktywnej gry. Rozpocznij nową grę, aby kontynuować.\n";
                continue;
            }

            auto [n, m] = account.current_game.board.get_size();

            cout << "\n" << "Aktualny stan planszy (aktualny wiersz poznaczony przez >----<):" << "\n";
            auto board_txt = account.get_board_txt();
            for (int i = n - 1; i >= 0; --i) {
                cout << board_txt[i] << '\n';
            }
            cout << "\n";

            cout << "Podaj numer kolumny nowej komórki do otwórzenia (od 1 do " << m << "): ";
            
            int j;
            cin >> j;
            if (j < 1 or m < j) {
                cout << "Numer kolumny ma być w przedziale od 1 do " << m << "." << '\n';
                continue;
            }
            --j;

            auto result = account.current_game.make_move(j);

            float income = account.current_game.get_income();
            float balance = account.get_balance();

            if (result == Game::move_result::OK) {
                cout << "Dobra komórka!\n";
                cout << "Na razie możesz zabrać " << income << " lub kontuuować grę z ryzykiem przegrać." << '\n';
            } else if (result == Game::move_result::WIN) {
                cout << "Dobra komórka!\n";
                cout << "Wygrałeś i otrzymałeś " << income << "!" << '\n';
                account.end_current_game();
                cout << "Twój aktualny balans: " << balance << "." << '\n';
            } else if (result == Game::move_result::LOSE) {
                cout << "Zła komórka!\n";
                cout << "Niestety, przegrałeś." << '\n';
                account.end_current_game();
                cout << "Twój aktualny balans: " << balance << "." << '\n';
            } else assert(false);
        } else if (choice == 3) {
            if (account.current_game.get_game_status() != Game::game_status::ONGOING) {
                cout << "Nie masz aktywnej gry.\n";
                continue;
            }

            account.end_current_game();
            cout << "Gra zakończona. Twój aktualny balans: " << account.get_balance() << " jednostek.\n";
        } else if (choice == 4) {
            cout << "Dziękujemy za grę! Do zobaczenia.\n";
            break;
        } else if (choice == 5) {
            if (account.current_game.get_game_status() == Game::game_status::ONGOING) {
                cout << "Masz aktywną grę. Zakończ ją, aby kontynuować.\n";
                continue;
            }

            cout << "Zmień ustawienia gry:\n";
            cout << "1. Domyślny poziom trudności\n";
            cout << "2. Domyślna stawka\n";
            // cout << "3. Domyślny rozmiar planszy\n";
            int subChoice;
            cin >> subChoice;

            if (subChoice == 1) {
                int newMode;
                // cout << "Wybierz poziom trudności (0-EASY, 1-MEDIUM, 2-HARD, 3-VERY_HARD): ";
                cout << "Wybierz poziom trudności (0-EASY, 1-MEDIUM, 2-HARD): ";
                cin >> newMode;
                if (newMode < 0 or 2 < newMode) {
                    cout << "Poziom ma być liczbą całkowitą od 0 do 2" << '\n';
                    continue;
                }
                account.set_default_mode(static_cast<Game::game_mode>(newMode));
                cout << "Zmieniono domyślny poziom trudności.\n";
            } else if (subChoice == 2) {
                float newBet;
                cout << "Podaj nową domyślną stawkę: ";
                cin >> newBet;
                if (newBet < 0) {
                    cout << "Stawka ma być liczbą dodatnią" << '\n';
                    continue;
                }
                account.set_default_bet(newBet);
                cout << "Zmieniono domyślną stawkę.\n";
                if (newBet > account.get_balance()) {
                    cout << "Uwaga: nowa domyślna stawka jest większa od aktualnego balansu Twojego konta." << '\n';
                    cout << "Przez to na razie nie możesz zacząć nową grę." << '\n';
                }
            // } else if (subChoice == 3) {
            //     int newRows, newCols;
            //     cout << "Podaj nowy rozmiar planszy (wiersze i kolumny): ";
            //     cin >> newRows >> newCols;
            //     account.set_default_size(newRows, newCols);
            //     cout << "Zmieniono domyślny rozmiar planszy.\n";
            } else {
                cout << "Nieprawidłowa opcja.\n";
            }
        } else if (choice == 6) {
            cout << "\n" << "Historia Twoich gier:" << "\n";
            auto history = account.get_history();
            
            cout << rjust("#", 2) << ": ";
            cout << rjust("game mode", 9) << ' ';
            cout << rjust("bet", 10) << ' ';
            cout << rjust("income", 10);
            cout << '\n';

            for (int i = 0; i < history.size(); i++) {
                cout << rjust(to_string(i + 1), 2) << ": ";
                cout << rjust(history[i].get_mode_txt(), 9) << ' ';
                cout << rjust(to_string(history[i].bet), 10) << ' ';
                cout << rjust(to_string(history[i].income), 10);
                cout << '\n';
            }
            cout << '\n';
            auto current_game = account.current_game;
            if (current_game.get_game_status() == Game::game_status::NOT_STARTED) {
                cout << "Aktualnej gry nie ma";
                cout << '\n';
            } else {
                cout << "Aktualna gra:";
                cout << '\n';
                cout << rjust("#", 2) << ": ";
                cout << rjust(current_game.get_mode_txt(), 9) << ' ';
                cout << rjust(to_string(current_game.bet), 10) << ' ';
                cout << rjust(to_string(current_game.income), 10);
                cout << '\n';
            }
            cout << '\n';
        } else if (choice == 42) {
            if (!DEBUG) {
                cout << "Nieprawidłowy wybór. Spróbuj ponownie.\n";
            } else {
                if (account.current_game.get_game_status() != Game::game_status::ONGOING) {
                    cout << "Nie masz aktywnej gry.\n";
                    continue;
                }

                cout << "Debug. Showing the whole grid:\n";
                auto [n, m] = account.current_game.board.get_size();

                auto board_txt = account.get_board_txt();
                for (int i = n - 1; i >= 0; --i) {
                    cout << board_txt[i] << '\n';
                }
                cout << "\n";
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
