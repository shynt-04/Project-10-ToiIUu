# interger programming
import ortools.linear_solver.pywraplp as pywraplp

def read_input_from_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        N, M = map(int, lines[0].split())
    
    Q = []
    for i in range(1, N + 1):
        row = list(map(int, lines[i].split()))
        Q.append(row)
    
    d = []
    for i in range(N + 1, N + M + 2):
        row = list(map(int, lines[i].split()))
        d.append(row)
    
    q = list(map(int, lines[N + M + 2].split()))
    
    return N, M, Q, d, q

def read_input_from_stdin():
    N, M = map(int, input().split())
    
    Q = []
    for i in range(N):
        row = list(map(int, input().split()))
        Q.append(row)
    
    d = []
    for i in range(M + 1):
        row = list(map(int, input().split()))
        d.append(row)
    
    q = list(map(int, input().split()))
    
    return N, M, Q, d, q

ans_route = list()

def check_stock(cur_q, q):
    for i in range(len(q)):
        if cur_q[i] < q[i]:
            return False
    return True

def greedy_initial_solution(N, M, Q, d, q):
    global ans_route
    used = [False] * (M + 1)
    cur_q = [0] * N
    route = []
    curr_node = 0
    total_dist = 0

    while check_stock(cur_q, q) == False:
        next_node = -1
        min_dist = float('inf')
        for j in range(1, M + 1):
            if not used[j]:
                if d[curr_node][j] < min_dist:
                    min_dist = d[curr_node][j]
                    next_node = j
        if next_node == -1:
            break
        total_dist += min_dist
        route.append(next_node)
        used[next_node] = True
        for i in range(N):
            cur_q[i] += Q[i][next_node - 1]
        curr_node = next_node

    total_dist += d[curr_node][0]
    route.append(0)
    # print("route length:", len(route) - 1)
    # print("route:", ' '.join(map(str, route[:-1])))
    # print("total distance:", total_dist)
    return route

def MIP_Solver(N, M, Q, d, q):
    global ans_route
    try:
        solver = pywraplp.Solver.CreateSolver('SCIP')
    except:
        print("judge suck")
    # solver = pywraplp.Solver.CreateSolver('CBC')

    solver.SetTimeLimit(10000)

    # desicion variables
    X = list()
    for i in range(M + 1):
        tmp = list()
        for j in range(M + 1):
            tmp.append(solver.IntVar(0, 1, "x[{},{}]".format(i, j)))
        X.append(tmp)

    Y = list()
    for i in range(M + 1):
        Y.append(solver.IntVar(0, 1, "y[{}]".format(i)))
    
    U = list()
    for i in range(M + 1):
        U.append(solver.IntVar(1, M, "u[{}]".format(i)))

    # greedy_ans = greedy_initial_solution(N, M, Q, d, q)
    # for i in range(M + 1):
    #     if i in greedy_ans:
    #         solver.Add(Y[i] == 1) 

    # constraints
    for i in range(N):
        solver.Add(solver.Sum(Q[i][j - 1] * Y[j] for j in range(1, M + 1)) >= q[i])

    for i in range(1, M + 1):
        solver.Add(solver.Sum(X[i][j] for j in range(M + 1) if j != i) == Y[i])
        solver.Add(solver.Sum(X[j][i] for j in range(M + 1) if j != i) == Y[i])

    solver.Add(solver.Sum(X[0][j] for j in range(1, M + 1)) == 1)
    solver.Add(solver.Sum(X[j][0] for j in range(1, M + 1)) == 1)

    for i in range(0, M + 1):
        solver.Add(X[i][i] == 0)

    for i in range(1, M + 1):
        for j in range(1, M + 1):
            if i != j:
                solver.Add(U[i] - U[j] + M * X[i][j] <= M - 1)

    # objective function
    obj = 0
    for i in range(M + 1):
        for j in range(M + 1):
            obj += d[i][j] * X[i][j]

    solver.Minimize(obj)
    status = solver.Solve()

    if status == pywraplp.Solver.OPTIMAL or status == pywraplp.Solver.FEASIBLE:
        next_node = [-1] * (M + 1)
        for i in range(0, M + 1):
            for j in range(0, M + 1):
                if X[i][j].solution_value() > 0:
                    next_node[i] = j
                    break
        route = []
        curr = 0
        while True:
            curr = next_node[curr]
            route.append(curr)
            if curr == 0:
                break

        ans_route = route
        # print("answer =", int(solver.Objective().Value()))
    else:
        ans_route = greedy_initial_solution(N, M, Q, d, q)

    print(len(ans_route) - 1)
    for i in range(0, len(ans_route) - 1):
        print(ans_route[i], end=' ')
        


if __name__ == "__main__":
    # N, M, Q, d, q = read_input_from_file('annotshy.inp')
    N, M, Q, d, q = read_input_from_stdin()
    # print(N, M)
    # print(Q)
    # print(d)
    # print(q)
    MIP_Solver(N, M, Q, d, q)
