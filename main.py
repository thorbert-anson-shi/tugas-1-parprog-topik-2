from typing import List

type Vector = List[float]
type Matrix = List[List[float]]


def mat_vec_mul(mat: Matrix, vec: Vector) -> Vector | None:
    # Check for compatibility of matrix and vector sizes
    if len(mat) != len(vec):
        return None

    # Check for square matrix
    for row in mat:
        if len(row) != len(mat):
            return None

    ans_dim = len(vec)

    ans = list[float]()
    for component in range(ans_dim):
        ans_component: float = 0
        for dim in range(ans_dim):
            ans_component += vec[component] * mat[dim][component]
        ans.append(ans_component)

    return ans


def mat_transpose(mat: Matrix) -> Matrix | None:
    for row in mat:
        if len(mat) != len(row):
            return None

    ans_mat = [[0.0 for _ in range(len(mat))] for _ in range(len(mat))]

    for row_idx, row in enumerate(mat):
        for ele_idx, element in enumerate(row):
            ans_mat[ele_idx][row_idx] = element

    return ans_mat


def matmul(a: Matrix, b: Matrix) -> Matrix | None:
    ans_transposed: Matrix = []

    # b_transpose is used to allow for easier indexing
    # Intuitively, we're iterating per column of B instead of
    # per row of b_transpose
    b_transpose = mat_transpose(b)

    if b_transpose is None:
        print("matrix B needs to be a square matrix")
        return None

    for row in b_transpose:
        ans_row = mat_vec_mul(a, row)

        if ans_row is None:
            print("the matrices are of incompatible dimensionality")
            return None

        ans_transposed.append(ans_row)

    return mat_transpose(ans_transposed)


def main():
    a: Matrix = [[1.0, 0.0], [0.0, 1.0]]
    b: Matrix = [[2.0, 3.0], [5.0, 4.0]]

    print(matmul(a, b))


if __name__ == "__main__":
    main()
