#include "gameplay.h"

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int get_rand(int l, int r) {
    int d = r - l + 1;
    return (rng() % d) + l;
}

Game::Board::Cell::Cell() : good(true), opened(false) {}
Game::Board::Cell::Cell(bool is_good) : good(is_good), opened(false) {}

bool Game::Board::Cell::is_good() { return good; }
bool Game::Board::Cell::is_opened() { return opened; }

void Game::Board::Cell::open() { opened = true; }
void Game::Board::Cell::set_good(bool is_good) { good = is_good; }

Game::Board::Board() : n(0), m(0), goods_cnt(0), passed(0) {}

pair<int, int> Game::Board::get_size() { return {n, m}; }

void Game::Board::init(int rows, int cols, int goods_c) {
    n = rows;
    m = cols;
    goods_cnt = goods_c;

    board.resize(n, vector<Cell>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            board[i][j].set_good(false);
        }

        vector<int> unused(m);
        iota(unused.begin(), unused.end(), 0);
        for (int rep = 0; rep < goods_cnt; rep++) {
            int ind = get_rand(0, unused.size() - 1);
            int j = unused[ind];
            unused.erase(unused.begin() + ind);
            board[i][j].set_good(true);
        }
    }
}

bool Game::Board::open_cell(int i, int j) {
    bool res = board[i][j].is_good();
    open_row(i);
    return res;
}

void Game::Board::open_row(int i) {
    for (int j = 0; j < m; j++) {
        board[i][j].open();
    }
}

bool Game::Board::is_won() { return passed == n; }
int Game::Board::get_passed() { return passed; }
bool Game::Board::inc_passed() {
    ++passed;
    return is_won();
}

Game::Game() : status(NOT_STARTED), bet(0), income(0) {}

int Game::get_chance(game_mode gamemode) {
    switch (gamemode) {
    case EASY: return 7500;
    case MEDIUM: return 5000;
    case HARD: return 2500;
    }
    return -1;
}

int Game::get_cnt(game_mode gamemode) {
    switch (gamemode) {
    case EASY: return 3;
    case MEDIUM: return 2;
    case HARD: return 1;
    }
    return -1;
}

Game::Board Game::get_board() { return board; }
Game::game_status Game::get_game_status() { return status; }
long double Game::get_bet() { return bet; }
long double Game::get_income() { return income; }

bool Game::is_won() { return board.is_won(); }

void Game::start(int rows, int cols, long double set_bet, game_mode set_mode) {
    mode = set_mode;
    bet = set_bet;
    int cnt = get_cnt(mode);
    board.init(rows, cols, cnt);
    income = calculate_income();
    status = ONGOING;
}

bool Game::finished() {
    return board.get_passed() == board.get_size().first;
}

long double Game::calculate_income() {
    int passed = board.get_passed();
    return bet * 0.98 / pow(get_chance(mode) * 1.0 / M, passed);
}

Game::move_result Game::make_move(int j) {
    int passed = board.get_passed();
    bool res = board.open_cell(passed, j);

    if (!res) {
        income = 0;
        return LOSE;
    }

    bool win = board.inc_passed();
    income = calculate_income();

    return win ? WIN : OK;
}

vector<vector<bool>> Game::get_bool_board() {
    vector<vector<bool>> bool_board(board.n, vector<bool>(board.m));
    for (int i = 0; i < board.n; i++) {
        for (int j = 0; j < board.m; j++) {
            bool_board[i][j] = board.board[i][j].is_good();
        }
    }
    return bool_board;
}

void Game::end() { status = FINISHED; }

string Game::get_mode_txt() {
    switch (mode) {
    case EASY: return "Easy";
    case MEDIUM: return "Medium";
    case HARD: return "Hard";
    }
    return "Unknown";
}

Account::Account() : balance(0), default_bet(0), default_rows(6), default_cols(4), default_mode(Game::MEDIUM) {}

void Account::set_default_mode(Game::game_mode new_mode) { default_mode = new_mode; }
void Account::set_default_size(int new_rows, int new_cols) {
    if (new_rows > 0) default_rows = new_rows;
    if (new_cols > 0) default_cols = new_cols;
}
void Account::set_default_bet(long double new_bet) { default_bet = new_bet; }
void Account::set_balance(long double new_bal) { balance = new_bal; }
void Account::add_balance(long double added) { balance += added; }

void Account::end_current_game() {
    Game::game_status was_status = current_game.get_game_status();
    current_game.end();
    add_balance(current_game.get_income());
    if (was_status != Game::NOT_STARTED) history.push_back(current_game);
    current_game = Game();
}

void Account::start_new_game() {
    current_game.start(default_rows, default_cols, default_bet, default_mode);
    add_balance(-default_bet);
}

Game::Board Account::get_board() { return current_game.get_board(); }

Game::move_result Account::make_move(int j) {
    Game::move_result result = current_game.make_move(j);

    if (result == Game::WIN || result == Game::LOSE) {
        current_game.end();
        end_current_game();
    }

    return result;
}

vector<Game> Account::get_history() { return history; }
Game::game_mode Account::get_default_mode() { return default_mode; }
pair<int, int> Account::get_default_size() { return {default_rows, default_cols}; }
long double Account::get_default_bet() { return default_bet; }
long double Account::get_balance() { return balance; }

Gameplay::Gameplay() {
    account.set_balance(100.0);
    account.set_default_bet(10.0);
    account.set_default_size(6, 4);
    account.set_default_mode(Game::MEDIUM);
    ok = false;
    who = 0;
}

long double Gameplay::get_balance() { return account.get_balance(); }

long double Gameplay::get_potential() {
    if (account.current_game.get_game_status() == Game::NOT_STARTED) {
        if (!account.history.empty() && account.history.back().is_won()) {
            return account.history.back().calculate_income();
        }
        return 0.0;
    }
    return account.current_game.calculate_income();
}

long double Gameplay::get_multiplier() {
    if (account.current_game.get_game_status() == Game::NOT_STARTED) {
        if (!account.history.empty() && account.history.back().is_won()) {
            return account.history.back().calculate_income() / account.history.back().get_bet();
        }
        return 0.0;
    }
    return account.current_game.calculate_income() / account.current_game.get_bet();
}

void Gameplay::start_money(int money) { account.set_balance(money); }

void Gameplay::new_game() {
    account.start_new_game();
    ok = true;
    who = 0;
    tab = account.current_game.get_bool_board();
}

int Gameplay::select(int j) {
    Game::move_result result = account.make_move(j);
    return (result == Game::WIN) ? 1 : (result == Game::LOSE) ? 2 : 0;
}

void Gameplay::cashout() { account.end_current_game(); }

void Gameplay::change_settings(int newBet, int newMode) {
    account.set_default_bet(static_cast<long double>(newBet));
    account.set_default_mode(static_cast<Game::game_mode>(3 - newMode));
}
