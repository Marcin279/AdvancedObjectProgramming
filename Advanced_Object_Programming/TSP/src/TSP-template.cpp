//
// Created by Dominika on 01.11.2020.
//
// Grupa 24: Agata Semerjak (402426), Kamil Maćków (302875), Marcin Biela (302959)
//

#include "TSP.hpp"

#include <algorithm>
#include <stack>
#include <optional>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const CostMatrix& cm) {
    for (std::size_t r = 0; r < cm.size(); ++r) {
        os << "[";
        for (std::size_t c = 0; c < cm.size(); ++c) {
            const auto& elem = cm[r][c];
            os << "   " <<  (is_inf(elem) ? "INF" : std::to_string(elem)) <<  "";
        }
        os << " ]" << "\n";
    }
    os << std::endl;

    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<cost_t>& v) {
    os << "[";
    for (auto it = v.cbegin(); it != v.cend(); ++it){
        os << " " << *it << ((it != v.cend() - 1) ? "," : " ");
    }
    os << "]";
    return os;
}


/* PART 1 */

/**
 * Create path from unsorted path and last 2x2 cost matrix.
 * @return The vector of consecutive vertex.
 */
path_t StageState::get_path() {
    path_t sorted_path;
    const CostMatrix& matrix2x2 = StageState::get_matrix();
    unsorted_path_t unsorted_path = StageState::get_unsorted_path();
    for (std::size_t row = 0; row < matrix2x2.size(); row++) {
        for (std::size_t col = 0; col < matrix2x2.size(); col++) {
            if (matrix2x2[row][col] != INF) {
                vertex_t v(row, col);
                unsorted_path.push_back(v);
            }
        }
    }
    vertex_t v_to_be_added = unsorted_path[0];
    sorted_path.push_back(v_to_be_added.row);
    while (sorted_path.size() != unsorted_path.size()) {
        for (auto v : unsorted_path) {
            if (v_to_be_added.col == v.row) {
                v_to_be_added = v;
                sorted_path.push_back(v_to_be_added.row);
                break;
            }
        }
    }
    return sorted_path;  // Done
}

/**
 * Get minimum values from each row and returns them.
 * @return Vector of minimum values in row.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_rows() const {
    std::vector<cost_t > min_values;
    auto& matrix = const_cast<cost_matrix_t&>(CostMatrix::get_matrix());
    for (auto row : matrix){
        cost_t min_value_in_row = row[0];
        for (std::size_t col = 1; col < matrix.size(); ++col) {
            if (row[col] == INF){
                continue;
            }
            else if (row[col] < min_value_in_row){
                min_value_in_row = row[col];
            }
        }
        min_values.push_back(min_value_in_row);
    }

    for (auto & min_value : min_values){
        if(min_value == INF){
            min_value = 0;
        }
    }
    return min_values;  // DONE
}

/**
 * Reduce rows so that in each row at least one zero value is present.
 * @return Sum of values reduced in rows.
 */

cost_t CostMatrix::reduce_rows() {

    std::vector<cost_t> min_values = CostMatrix::get_min_values_in_rows();

    auto& matrix = const_cast<cost_matrix_t&>(CostMatrix::get_matrix());

    for (std::size_t row = 0; row < matrix.size(); ++row) {
        for (std::size_t col = 0; col < matrix.size(); ++col) {
            if (is_inf(matrix[row][col])){
                continue;
            }
            else {
                matrix[row][col] -= min_values[row];
            }
        }
    }
    cost_t result = std::accumulate(std::cbegin(min_values), std::cend(min_values), 0);

    return result;  // DONE

}

/**
 * Get minimum values from each column and returns them.
 * @return Vector of minimum values in columns.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_cols() const {
    std::vector<cost_t> min_values;
    auto& matrix = const_cast<cost_matrix_t&>(CostMatrix::get_matrix());
    for (std::size_t col = 0; col < matrix.size(); ++col) {
        cost_t  min_value_in_col = matrix[0][col];
        for (std::size_t row = 1; row < matrix.size(); ++row) {
            if (matrix[row][col] < min_value_in_col) {
                min_value_in_col = matrix[row][col];
            }
        }
        min_values.push_back(min_value_in_col);
    }
    for (auto & min_value : min_values){
        if(min_value == INF){
            min_value = 0;
        }
    }
    return min_values;  // Done
}

/**
 * Reduces rows so that in each column at least one zero value is present.
 * @return Sum of values reduced in columns.
 */
cost_t CostMatrix::reduce_cols() {

    std::vector<cost_t> min_values = CostMatrix::get_min_values_in_cols();

    auto& matrix = const_cast<cost_matrix_t&>(CostMatrix::get_matrix());

    for (std::size_t row = 0; row < matrix.size(); ++row) {
        for (std::size_t col = 0; col < matrix.size(); ++col) {
            if (is_inf(matrix[row][col])) {
                continue;
            }
            else {
                matrix[row][col] -= min_values[col];
            }
        }
    }
    cost_t result = std::accumulate(std::cbegin(min_values), std::cend(min_values), 0);

    return result;  // DONE
}

/**
 * Get the cost of not visiting the vertex_t (@see: get_new_vertex())
 * @param row
 * @param col
 * @return The sum of minimal values in row and col, excluding the intersection value.
 */
cost_t CostMatrix::get_vertex_cost(std::size_t row, std::size_t col) const {
    cost_t min_cost = 0;
    const CostMatrix& matrix = CostMatrix::get_matrix();
    cost_t min_value_in_col = (row != 0)? matrix[0][col] : matrix[1][col];
    for (std::size_t r = 0; r < matrix.size(); r++) {
        if (r != row && matrix[r][col] < min_value_in_col) {
            min_value_in_col = matrix[r][col];
        }
    }
    cost_t min_value_in_row = 0;
    if (col != 0) {
        min_value_in_row = matrix[row][0];
    } else {
        min_value_in_row = matrix[row][1];
    }
    for (std::size_t c = 0; c < matrix.size(); c++) {
        if (c != col && matrix[row][c] < min_value_in_row) {
            min_value_in_row = matrix[row][c];
        }
    }
    min_cost = min_value_in_col + min_value_in_row;
    return min_cost;  // Done
}

/* PART 2 */

/**
 * Choose next vertex to visit:
 * - Look for vertex_t (pair row and column) with value 0 in the current cost matrix.
 * - Get the vertex_t cost (calls get_vertex_cost()).
 * - Choose the vertex_t with maximum cost and returns it.
 * @return The coordinates of the next vertex.
 */
NewVertex StageState::choose_new_vertex() {
    const CostMatrix & matrix = StageState::get_matrix();

    std::vector<std::size_t> rows_with_zeros;
    std::vector<std::size_t> columns_with_zeros;

    for (std::size_t r = 0; r < matrix.size(); r++){
        for (std::size_t c = 0; c < matrix.size(); c++){
            if (matrix[r][c] == 0){
                rows_with_zeros.push_back(r);
                columns_with_zeros.push_back(c);
            }
        }
    }
    std::vector<cost_t> costs_of_zeros;
    for (std::size_t i = 0; i < rows_with_zeros.size(); i++){
        cost_t cost = matrix.get_vertex_cost(rows_with_zeros[i], columns_with_zeros[i]);
        costs_of_zeros.push_back(cost);
    }
    cost_t max_cost = costs_of_zeros[0];
    for (std::size_t i = 0; i < costs_of_zeros.size(); i++){
        if (costs_of_zeros[i] > max_cost){
            max_cost = costs_of_zeros[i];
        }
    }
    std::size_t max_cost_index = 0;
    for (std::size_t index = 0; index < costs_of_zeros.size(); index++){
        if (costs_of_zeros[index] == max_cost){
            max_cost_index = index;
            break;
        }
    }
    std::size_t max_val_row = rows_with_zeros[max_cost_index];
    std::size_t max_val_col = columns_with_zeros[max_cost_index];


    return NewVertex(vertex_t(max_val_row, max_val_col), max_cost); // Done
}

/**
 * Update the cost matrix with the new vertex.
 * @param new_vertex
 */

void StageState::update_cost_matrix(vertex_t new_vertex) {

    //auto& matrix = const_cast<cost_matrix_t&>(matrix_.get_matrix());
    auto& unsor_path = const_cast<unsorted_path_t&>(StageState::get_unsorted_path());

    std::size_t forbidden_row = new_vertex.row;
    std::size_t forbidden_col = new_vertex.col;

    for (std::size_t i = 0; i < matrix_.size(); ++i){
        matrix_[i][forbidden_col] = INF;
        matrix_[forbidden_row][i] = INF;
    }

    auto r = forbidden_row;
    bool does_match = true;
    while (does_match) {
        for (auto v : unsor_path) {
            if (v.col == r) {
                r = v.row;
                does_match = true;
                break;
            }
            else {
                does_match = false;
            }
        }
    }

    auto c = forbidden_col;
    does_match = true;
    while (does_match) {
        for (auto v : unsor_path) {
            if (v.row == c) {
                c = v.col;
                does_match = true;
                break;
            }
            else {
                does_match = false;
            }
        }
    }
    matrix_[c][r] = INF;
}

/**
 * Reduce the cost matrix.
 * @return The sum of reduced values.
 */
cost_t StageState::reduce_cost_matrix() {

    cost_t reduced_val_row = matrix_.reduce_rows();
    cost_t reduced_val_col = matrix_.reduce_cols();

    return reduced_val_row + reduced_val_col;  // Done
}


/**
 * Given the optimal path, return the optimal cost.
 * @param optimal_path
 * @param m
 * @return Cost of the path.
 */
cost_t get_optimal_cost(const path_t& optimal_path, const cost_matrix_t& m) {
    cost_t cost = 0;

    for (std::size_t idx = 1; idx < optimal_path.size(); ++idx) {
        cost += m[optimal_path[idx - 1]][optimal_path[idx]];
    }

    // Add the cost of returning from the last city to the initial one.
    cost += m[optimal_path[optimal_path.size() - 1]][optimal_path[0]];

    return cost;
}

/**
 * Create the right branch matrix with the chosen vertex forbidden and the new lower bound.
 * @param m
 * @param v
 * @param lb
 * @return New branch.
 */
StageState create_right_branch_matrix(cost_matrix_t m, vertex_t v, cost_t lb) {
    CostMatrix cm(m);
    cm[v.row][v.col] = INF;
    return StageState(cm, {}, lb);
}

/**
 * Retain only optimal ones (from all possible ones).
 * @param solutions
 * @return Vector of optimal solutions.
 */
tsp_solutions_t filter_solutions(tsp_solutions_t solutions) {
    cost_t optimal_cost = INF;
    for (const auto& s : solutions) {
        optimal_cost = (s.lower_bound < optimal_cost) ? s.lower_bound : optimal_cost;
    }

    tsp_solutions_t optimal_solutions;
    std::copy_if(solutions.begin(), solutions.end(),
                 std::back_inserter(optimal_solutions),
                 [&optimal_cost](const tsp_solution_t& s) { return s.lower_bound == optimal_cost; }
    );

    return optimal_solutions;
}

/**
 * Solve the TSP.
 * @param cm The cost matrix.
 * @return A list of optimal solutions.
 */
tsp_solutions_t solve_tsp(const cost_matrix_t& cm) {

    StageState left_branch(cm);

    // The branch & bound tree.
    std::stack<StageState> tree_lifo;

    // The number of levels determines the number of steps before obtaining
    // a 2x2 matrix.
    std::size_t n_levels = cm.size() - 2;

    tree_lifo.push(left_branch);   // Use the first cost matrix as the root.

    cost_t best_lb = INF;
    tsp_solutions_t solutions;

    while (!tree_lifo.empty()) {

        left_branch = tree_lifo.top();
        tree_lifo.pop();

        while (left_branch.get_level() != n_levels && left_branch.get_lower_bound() <= best_lb) {
            // Repeat until a 2x2 matrix is obtained or the lower bound is too high...

            if (left_branch.get_level() == 0) {
                left_branch.reset_lower_bound();
            }

            // 1. Reduce the matrix in rows and columns.
            cost_t new_cost = left_branch.reduce_cost_matrix(); // @TODO (KROK 1)

            // 2. Update the lower bound and check the break condition.
            left_branch.update_lower_bound(new_cost);
            if (left_branch.get_lower_bound() > best_lb) {
                break;
            }

            // 3. Get new vertex and the cost of not choosing it.
            NewVertex new_vertex = left_branch.choose_new_vertex(); // @TODO (KROK 2)

            // 4. @TODO Update the path - use append_to_path method.
            left_branch.append_to_path(new_vertex.coordinates);

            // 5. @TODO (KROK 3) Update the cost matrix of the left branch.
            left_branch.update_cost_matrix(new_vertex.coordinates);

            // 6. Update the right branch and push it to the LIFO.
            cost_t new_lower_bound = left_branch.get_lower_bound() + new_vertex.cost;
            tree_lifo.push(create_right_branch_matrix(cm, new_vertex.coordinates,
                                                      new_lower_bound));
        }

        if (left_branch.get_lower_bound() <= best_lb) {
            // If the new solution is at least as good as the previous one,
            // save its lower bound and its path.
            best_lb = left_branch.get_lower_bound();
            path_t new_path = left_branch.get_path();
            cost_t optimal_cost = get_optimal_cost(new_path, cm);
            for (std::size_t i = 0; i <=new_path.size(); i++) {
                new_path[i] = new_path[i] + 1;
            }
            solutions.push_back({optimal_cost, new_path});
        }
    }

    return filter_solutions(solutions); // Filter solutions to find only optimal ones.
}