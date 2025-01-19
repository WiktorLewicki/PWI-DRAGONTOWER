#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <chrono>
#include <numeric>
#include <cmath>

using namespace std;
extern mt19937 rng;
int get_rand(int l, int r);

const int M = 10000;

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

            Cell();
            Cell(bool is_good);

            bool is_good();
            bool is_opened();

            void open();
            void set_good(bool is_good);
        };

        int n, m;
        int goods_cnt;
        int passed;
        vector<vector<Cell>> board;

        Board();

        pair<int, int> get_size();
        void init(int rows, int cols, int goods_c);

        bool open_cell(int i, int j);
        void open_row(int i);

        bool is_won();
        int get_passed();
        bool inc_passed();
    };

    Game();

    Board board;
    game_status status;
    game_mode mode;
    long double bet;
    long double income;

    Board get_board();
    game_status get_game_status();
    long double get_bet();
    long double get_income();

    bool is_won();

    void start(int rows, int cols, long double set_bet, game_mode set_mode);
    bool finished();
    long double calculate_income();

    move_result make_move(int j);
    vector<vector<bool>> get_bool_board();
    void end();
    string get_mode_txt();

private:
    int get_chance(game_mode gamemode);
    int get_cnt(game_mode gamemode);
};

struct Account {
    long double balance;
    vector<Game> history;
    Game current_game;
    long double default_bet;
    int default_rows, default_cols;
    Game::game_mode default_mode;

    Account();

    void set_default_mode(Game::game_mode new_mode);
    void set_default_size(int new_rows, int new_cols);
    void set_default_bet(long double new_bet);
    void set_balance(long double new_bal);
    void add_balance(long double added);

    void end_current_game();
    void start_new_game();

    Game::Board get_board();
    Game::move_result make_move(int j);

    vector<Game> get_history();
    Game::game_mode get_default_mode();
    pair<int, int> get_default_size();
    long double get_default_bet();
    long double get_balance();
};

struct Gameplay {
    bool ok;
    int who;
    vector<vector<bool>> tab;
    Account account;

    Gameplay();

    long double get_balance();
    long double get_potential();
    long double get_multiplier();

    void start_money(int money);
    void new_game();
    int select(int j);
    void cashout();
    void change_settings(int newBet, int newMode);
};

#endif // GAMEPLAY_H
