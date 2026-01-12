import os
import subprocess
import time
from pathlib import Path


def run_command(args, *, timeout_s=None):
    t0 = time.perf_counter()
    completed = subprocess.run(
        args,
        capture_output=True,
        text=True,
        timeout=timeout_s,
    )
    t1 = time.perf_counter()

    if completed.returncode != 0:
        stderr = (completed.stderr or "").strip()
        stdout = (completed.stdout or "").strip()
        msg = "Command failed: " + " ".join(map(str, args))
        if stdout:
            msg += "\n--- stdout ---\n" + stdout
        if stderr:
            msg += "\n--- stderr ---\n" + stderr
        raise RuntimeError(msg)

    return t1 - t0

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

def read_output_from_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        route_size = int(lines[0].strip())
        route = list(map(int, lines[1].split()))
    return route_size, route

def check_valid_route(N, M, Q, d, q, route_size, route):
    cur_q = [0] * N
    total_dist = 0
    visited = [False] * (M + 1)

    if route_size == 0:
        return False, "Route size is zero."

    for i in range(route_size):
        u = 0
        if i > 0:
            u = route[i - 1]
        v = route[i]

        if v < 1 or v > M:
            return False, f"trong route co diem khong hop le: {v}"

        if visited[v]:
            return False, f"Di qua {v} > 1 lan"

        visited[v] = True
        for j in range(N):
            cur_q[j] += Q[j][v - 1]

        total_dist += d[u][v]
    total_dist += d[route[-1]][0]

    for i in range(N):
        if cur_q[i] < q[i]:
            return False, f"nhu cau cho mat hang {i} khong du."

    return True, total_dist

def check_algo(algo, N, M, Q, d, q):
    print(f"Checking {algo}.cpp")
    if out_path.exists():
        out_path.unlink()

    compile_time_s = run_command([
        'g++',
        '-O2',
        '-std=c++17',
        f'{algo}.cpp',
        '-o',
        f'{algo}.exe',
    ])
    exec_time_s = run_command([f'./{algo}.exe'], timeout_s=60)
        
    route_size, route = read_output_from_file('annotshy.out')
    valid, result = check_valid_route(N, M, Q, d, q, route_size, route)
    # print("Route: ", route)
    if valid:
        print("Valid route with total distance:", result)
    else:
        print("Invalid route:", result)
    print("Compile time (s):", compile_time_s)
    print("Execution time (s):", exec_time_s)

def check_ortools(N, M, Q, d, q):
    # doan nay phai sua ortools.py de in ra file thay vì in ra stdout moi chay duoc
    print("Checking IP-ortools.py")
    if out_path.exists():
        out_path.unlink()
    
    exec_time_s = run_command([
        'python',
        'IP-ortools.py',
        '>', 'annotshy.out'
    ], timeout_s=60)
    route_size, route = read_output_from_file('annotshy.out')
    valid, result = check_valid_route(N, M, Q, d, q, route_size, route)
    # print("Route: ", route)
    if valid:
        print("Valid route with total distance:", result)
    else:
        print("Invalid route:", result)
    print("Execution time (s):", exec_time_s)

if __name__ == "__main__":
    N, M, Q, d, q = read_input_from_file('annotshy.inp')

    out_path = Path('annotshy.out')

    # try:
    #     check_algo("backtracking", N, M, Q, d, q)
    # except:
    #     print("TLE")

    # check_algo("greedy", N, M, Q, d, q)
    
    # try:
    #     check_algo("branch-and-bound", N, M, Q, d, q)
    # except:
    #     print("TLE")
    
    # try:
    #     check_algo("branch-and-cut", N, M, Q, d, q)
    # except:
    #     print("TLE")
    
    check_algo("local-search-hill-climbing", N, M, Q, d, q)
    
    check_algo("local-search-simulated-annealing", N, M, Q, d, q)
    
    # check_algo("local-search-tabu-search", N, M, Q, d, q)

    # try: 
    #     check_ortools(N, M, Q, d, q)
    # except:
    #     print("TLE")