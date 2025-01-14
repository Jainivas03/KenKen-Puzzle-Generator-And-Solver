#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip> // For grid formatting
#include <map>     // For color mapping
using namespace std;

// Struct for storing Cage info
struct Cage {
    vector<pair<int, int>> cells; // (row, column) positions
    char operation;               // '+', '-', '*', '/'
    int target;                   // target value for this cage
};

// Function declarations
bool is_safe_to_assign(vector<vector<int>>& grid, int row, int col, int value, const vector<Cage>& cages, int n);
bool check_row_constraint(const vector<vector<int>>& grid, int row, int value, int n);
bool check_column_constraint(const vector<vector<int>>& grid, int col, int value, int n);
bool check_cage_constraint(const vector<vector<int>>& grid, int row, int col, int value, const vector<Cage>& cages);
bool kenken_solver(vector<vector<int>>& grid, const vector<Cage>& cages, int n);
pair<int, int> find_empty_cell(const vector<vector<int>>& grid, int n);
void print_puzzle(const vector<vector<int>>& grid, const vector<Cage>& cages, int n, bool is_solution = false);
string get_colored_output(int color, string text);

// Color codes
map<int, string> colors = {
    {1, "\033[31m"}, // Red
    {2, "\033[32m"}, // Green
    {3, "\033[33m"}, // Yellow
    {4, "\033[34m"}, // Blue
    {5, "\033[35m"}, // Magenta
    {6, "\033[36m"}, // Cyan
    {7, "\033[37m"}, // White
    {0, "\033[0m"}   // Reset
};

int main() {
    int n;
    cout << "Enter grid size (n x n): ";
    cin >> n;

    vector<vector<int>> grid(n, vector<int>(n, 0)); // Initialize grid with 0s (unassigned cells)

    int numCages;
    cout << "Enter number of cages: ";
    cin >> numCages;

    vector<Cage> cages;
    for (int i = 0; i < numCages; ++i) {
        int numCells;
        cout << "Enter number of cells in cage " << i + 1 << ": ";
        cin >> numCells;

        vector<pair<int, int>> cells;
        for (int j = 0; j < numCells; ++j) {
            int row, col;
            cout << "Enter row and column for cell " << j + 1 << " (0-indexed): ";
            cin >> row >> col;
            cells.push_back({row, col});
        }

        char operation;
        int target;
        cout << "Enter operation (+, -, *, /) and target for this cage: ";
        cin >> operation >> target;

        cages.push_back({cells, operation, target});
    }

    // Print the unsolved puzzle with operations
    cout << "Initial Puzzle (with operations):\n";
    print_puzzle(grid, cages, n);

    if (kenken_solver(grid, cages, n)) {
        cout << "\nSolution found:\n";
        print_puzzle(grid, cages, n, true);
    } else {
        cout << "No solution exists.\n";
    }

    return 0;
}

// KenKen Solver function
bool kenken_solver(vector<vector<int>>& grid, const vector<Cage>& cages, int n) {
    auto empty_cell = find_empty_cell(grid, n);
    int row = empty_cell.first;
    int col = empty_cell.second;

    if (row == -1 && col == -1) {
        return true; // Puzzle solved
    }

    for (int value = 1; value <= n; ++value) {
        if (is_safe_to_assign(grid, row, col, value, cages, n)) {
            grid[row][col] = value; // Assign value
            if (kenken_solver(grid, cages, n)) {
                return true;
            }
            grid[row][col] = 0; // Backtrack
        }
    }
    return false;
}

// Helper functions
pair<int, int> find_empty_cell(const vector<vector<int>>& grid, int n) {
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            if (grid[row][col] == 0) {
                return {row, col};
            }
        }
    }
    return {-1, -1}; // No empty cells left
}

bool is_safe_to_assign(vector<vector<int>>& grid, int row, int col, int value, const vector<Cage>& cages, int n) {
    return check_row_constraint(grid, row, value, n) &&
           check_column_constraint(grid, col, value, n) &&
           check_cage_constraint(grid, row, col, value, cages);
}

bool check_row_constraint(const vector<vector<int>>& grid, int row, int value, int n) {
    for (int col = 0; col < n; ++col) {
        if (grid[row][col] == value) {
            return false;
        }
    }
    return true;
}

bool check_column_constraint(const vector<vector<int>>& grid, int col, int value, int n) {
    for (int row = 0; row < n; ++row) {
        if (grid[row][col] == value) {
            return false;
        }
    }
    return true;
}

bool check_cage_constraint(const vector<vector<int>>& grid, int row, int col, int value, const vector<Cage>& cages) {
    for (const auto& cage : cages) {
        bool is_in_cage = false;
        for (const auto& cell : cage.cells) {
            if (cell.first == row && cell.second == col) {
                is_in_cage = true;
                break;
            }
        }
        if (!is_in_cage) continue;

        vector<int> cage_values;
        for (const auto& cell : cage.cells) {
            int r = cell.first, c = cell.second;
            if (r == row && c == col) {
                cage_values.push_back(value);
            } else if (grid[r][c] != 0) {
                cage_values.push_back(grid[r][c]);
            } else {
                return true;
            }
        }

        if (cage.operation == '+') {
            int sum = 0;
            for (int num : cage_values) sum += num;
            if (sum != cage.target) return false;
        } else if (cage.operation == '*') {
            int product = 1;
            for (int num : cage_values) product *= num;
            if (product != cage.target) return false;
        } else if (cage.operation == '-') {
            if (cage_values.size() != 2) return false;
            if (abs(cage_values[0] - cage_values[1]) != cage.target) return false;
        } else if (cage.operation == '/') {
            if (cage_values.size() != 2) return false;
            if ((cage_values[0] / cage_values[1] != cage.target && cage_values[1] / cage_values[0] != cage.target)) {
                return false;
            }
        }
    }
    return true;
}

// Function to print the puzzle or solution
void print_puzzle(const vector<vector<int>>& grid, const vector<Cage>& cages, int n, bool is_solution) {
    vector<vector<int>> cage_map(n, vector<int>(n, -1));

    // Map each cell to a cage index
    for (int i = 0; i < cages.size(); ++i) {
        for (const auto& cell : cages[i].cells) {
            cage_map[cell.first][cell.second] = i;
        }
    }

    // Print the grid
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            int cage_idx = cage_map[row][col];
            string output;
            if (!is_solution) {
                if (cage_idx != -1) {
                    output = cages[cage_idx].operation + to_string(cages[cage_idx].target);
                } else {
                    output = "  ";
                }
            } else {
                output = to_string(grid[row][col]);
            }
            cout << get_colored_output(cage_idx % 7 + 1, output) << " ";
        }
        cout << colors[0] << endl; // Reset color after each row
    }
}

// Helper to get colored output
string get_colored_output(int color, string text) {
    return colors[color] + text + colors[0];
}