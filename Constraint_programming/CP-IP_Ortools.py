from ortools.sat.python import cp_model
import sys

def read_input_from_stdin():
    input_data = sys.stdin.read().split()
    if not input_data:
        return None
    
    ptr = 0
    N = int(input_data[ptr]); ptr += 1
    M = int(input_data[ptr]); ptr += 1
    
    Q = []
    for i in range(N):
        row = []
        for j in range(M):
            row.append(int(input_data[ptr]))
            ptr += 1
        Q.append(row)
    
    d = []
    for i in range(M + 1):
        row = []
        for j in range(M + 1):
            row.append(int(input_data[ptr]))
            ptr += 1
        d.append(row)
    
    q = []
    for i in range(N):
        q.append(int(input_data[ptr]))
        ptr += 1
        
    return N, M, Q, d, q

def greedy_initial_solution(N, M, Q, d, q):
    used = [False] * (M + 1)
    cur_q = [0] * N
    route = []
    curr_node = 0
    
    def is_satisfied(current_stock):
        for i in range(N):
            if current_stock[i] < q[i]: return False
        return True

    while not is_satisfied(cur_q):
        next_node = -1
        min_dist = float('inf')
        for j in range(1, M + 1):
            if not used[j]:
                if d[curr_node][j] < min_dist:
                    min_dist = d[curr_node][j]
                    next_node = j
        if next_node == -1: break
        route.append(next_node)
        used[next_node] = True
        for i in range(N):
            cur_q[i] += Q[i][next_node - 1]
        curr_node = next_node
    return route

def solve_cp(N, M, Q, d, q):
    model = cp_model.CpModel()
    
    Y = [model.NewBoolVar(f'y_{i}') for i in range(M + 1)]
    model.Add(Y[0] == 1)

    for i in range(N):
        model.Add(sum(Q[i][j-1] * Y[j] for j in range(1, M + 1)) >= q[i])

    arcs = []
    for i in range(M + 1):
        for j in range(M + 1):
            if i == j: continue
            lit = model.NewBoolVar(f'x_{i}_{j}')
            model.AddImplication(lit, Y[i])
            model.AddImplication(lit, Y[j])
            arcs.append((i, j, lit))
            
    for i in range(M + 1):
        loop = model.NewBoolVar(f'loop_{i}')
        model.Add(Y[i] == loop.Not())
        arcs.append((i, i, loop))

    model.AddCircuit(arcs)

    total_distance = sum(d[i][j] * lit for i, j, lit in arcs if i != j)
    model.Minimize(total_distance)

    greedy_route = greedy_initial_solution(N, M, Q, d, q)
    for i in range(1, M + 1):
        model.AddHint(Y[i], 1 if i in greedy_route else 0)
    
    solver = cp_model.CpSolver()
    solver.parameters.max_time_in_seconds = 49.0
    solver.parameters.num_search_workers = 8
    
    status = solver.Solve(model)

    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        next_node_map = {}
        for i, j, lit in arcs:
            if i != j and solver.Value(lit):
                next_node_map[i] = j
        
        final_route = []
        curr = next_node_map[0]
        while curr != 0:
            final_route.append(curr)
            curr = next_node_map[curr]
        
        print(len(final_route))
        print(*(final_route))
    else:
        print(len(greedy_route))
        print(*(greedy_route))

if __name__ == "__main__":
    data = read_input_from_stdin()
    if data:
        solve_cp(*data)