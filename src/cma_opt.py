import os
import platform
import multiprocessing as mp
import numpy as np
import cma
from subprocess import Popen, PIPE

# ===============================
# CONFIGURATION
# ===============================

WEIGHTS_FILE = "./cpp_minimax/weights.txt"
RESULTS_FILE = "./cma_optimization_results.txt"
NUMBER_OF_MATCHES = 6
NUMBER_OF_TURNS_PER_MATCH = 200
NUMBER_MOVES_LOG = 25

game_start = "newgame Base+MLP\n"
ending_sequence = "ok"
bestmove = "bestmove time 00:00:05\n"

if platform.system() == "Linux":
    MINIMAX_ENGINE = "./cpp_minimax/main"
    MZINGA_ENGINE = "../dist/MzingaEngine"
else:
    MINIMAX_ENGINE = "./cpp_minimax/main.exe"
    MZINGA_ENGINE = "./cpp_minimax/base.exe"


# ===============================
# FILE HANDLING
# ===============================

def write_weights_file(weights_dict, filename=WEIGHTS_FILE):
    """Write weights to the structured text file."""
    with open(filename, 'w') as f:
        categories = [
            'inPlay', 'isPinned', 'isCovered', 'noisyWeight',
            'quietWeight', 'friendlyNeighbor', 'enemyNeighbor',
            'isOnTop', 'offensive', 'defensive'
        ]
        for cat in categories:
            weights_str = ' '.join(str(weights_dict[f'{cat}_{i}']) for i in range(8))
            f.write(f"{weights_str} ")

        # Single values
        for key in ['pillbugNearQ', 'mosquitoNearStrong', 'spiderDistance', 'queenGate']:
            f.write(f"{weights_dict[key]} ")


# ===============================
# GAME PLAY
# ===============================

def play_single_game(pl1_path, pl2_path, matches=NUMBER_OF_MATCHES):
    """
    Play a game between two engines and return stats.
    """
    try:
        player1 = Popen(pl1_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)
        player2 = Popen(pl2_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)

        for player in [player1, player2]:
            output = player.stdout.readline()
            while not output.startswith(ending_sequence):
                output = player.stdout.readline()

        stats = {"pl1_wins": 0, "pl2_wins": 0, "draws": 0, "pl1_win_turns": [], "pl2_win_turns": []}

        for match in range(1, matches + 1):
            # Start new game
            for p in [player1, player2]:
                p.stdin.write(game_start)
                while not p.stdout.readline().startswith(ending_sequence):
                    pass

            moving_player = player1 if match % 2 == 1 else player2
            opponent = player2 if match % 2 == 1 else player1
            player1_is_white = (match % 2 == 1)

            for turn in range(1, NUMBER_OF_TURNS_PER_MATCH + 1):
                #print(turn)
                moving_player.stdin.write(bestmove)
                move = moving_player.stdout.readline()
                output = moving_player.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = moving_player.stdout.readline()

                move_cmd = f"play {move.strip()}\n"
                opponent.stdin.write(move_cmd)
                moving_player.stdin.write(move_cmd)

                output = moving_player.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = moving_player.stdout.readline()

                response_lines = []
                while True:
                    line = opponent.stdout.readline()
                    response_lines.append(line)
                    if line.startswith(ending_sequence):
                        break

                response = ''.join(response_lines)

                if "WhiteWins" in response:
                    if player1_is_white:
                        stats["pl1_wins"] += 1; stats["pl1_win_turns"].append(turn)
                    else:
                        stats["pl2_wins"] += 1; stats["pl2_win_turns"].append(turn)
                    break
                elif "BlackWins" in response:
                    if not player1_is_white:
                        stats["pl1_wins"] += 1; stats["pl1_win_turns"].append(turn)
                    else:
                        stats["pl2_wins"] += 1; stats["pl2_win_turns"].append(turn)
                    break
                elif "Draw" in response:
                    stats["draws"] += 1
                    break

                if turn == NUMBER_OF_TURNS_PER_MATCH-1:
                    stats["draws"] += 1

                moving_player, opponent = opponent, moving_player

        try:
            player1.stdin.write("exit\n")
            player2.stdin.write("exit\n")
        except:
            pass
        player1.terminate()
        player2.terminate()

        avg_turns_pl1 = sum(stats["pl1_win_turns"]) / len(stats["pl1_win_turns"]) if stats["pl1_win_turns"] else 0
        avg_turns_pl2 = sum(stats["pl2_win_turns"]) / len(stats["pl2_win_turns"]) if stats["pl2_win_turns"] else 0

        return stats["pl1_wins"], stats["pl2_wins"], stats["draws"], avg_turns_pl1, avg_turns_pl2

    except Exception as e:
        print(f"Error during game simulation: {e}")
        return 0, 0, 0, 0, 0


# ===============================
# EVALUATION
# ===============================

def evaluate_weights(weights_dict):
    """Evaluate weights by playing against Mzinga. Returns fitness score."""
    write_weights_file(weights_dict)

    minimax_wins, mzinga_wins, draws, avg_turns_minimax, avg_turns_mzinga = play_single_game(
        MINIMAX_ENGINE, MZINGA_ENGINE, NUMBER_OF_MATCHES
    )

    total_games = minimax_wins + mzinga_wins + draws
    if total_games == 0:
        return -1000

    win_ratio = minimax_wins / total_games
    quick_win_score = (1 - (avg_turns_minimax / NUMBER_OF_TURNS_PER_MATCH)) * (minimax_wins / total_games) if minimax_wins > 0 else 0
    loss_resistance_score = (avg_turns_mzinga / NUMBER_OF_TURNS_PER_MATCH) * (mzinga_wins / total_games) if mzinga_wins > 0 else 0
    draw_score = (draws / total_games) * 0.5

    fitness = 1.5 * win_ratio + 0.5 * quick_win_score + 0.3 * loss_resistance_score + 0.4 * draw_score

    return fitness


# ===============================
# CMA-ES OBJECTIVE
# ===============================

def cma_objective(x):
    """CMA-ES objective wrapper."""
    weights_dict = {}
    categories = [
        'inPlay', 'isPinned', 'isCovered', 'noisyWeight', 'quietWeight',
        'friendlyNeighbor', 'enemyNeighbor', 'isOnTop', 'offensive', 'defensive'
    ]
    idx = 0
    for cat in categories:
        for i in range(8):
            weights_dict[f"{cat}_{i}"] = int(x[idx]); idx += 1

    for name in ['pillbugNearQ', 'mosquitoNearStrong', 'spiderDistance', 'queenGate']:
        weights_dict[name] = int(x[idx]); idx += 1

    fitness = evaluate_weights(weights_dict)
    return -fitness  # CMA minimizes


# ===============================
# CMA-ES OPTIMIZATION
# ===============================

def run_cma_es_parallel():
    # Default weights (hv[84])
    x0 = np.array([
        25436, -47942, 62698, 13433, 132380, 35702, 428, -3,
        -46539, -48021, -82598, -129954, -51036, -47853, 428, 36,
        7318, 9325, 24024, -20359, -1515, -5471, 1, 1,
        139756, 142117, 105511, 72291, 53867, 34855, -327, -327,
        -20696, 11689, -12497, 95775, 4129, 3658, -2037, -5,
        21040, 36463, 13656, -317467, 14307, 9199, 426, 1,
        47143, 63937, 24316, -538149, 21240, 20205, 0, 25,
        0, 50000, 0, 90000, 120000, 45000, 0, 25,
        50000, 100000, 30000, -45, 3000, 45000, 70000, 25,
        5000, 50000, 43000, 0, 3400, 100000, 56000, 250000,
        1500, 1000, 1000, 40000
    ])

    sigma0 = 20000
    options = {'popsize': 16, 'maxiter': 200, 'verb_disp': 1, 'bounds': [-600000, 600000]}

    es = cma.CMAEvolutionStrategy(x0, sigma0, options)

    with mp.Pool(processes=mp.cpu_count()) as pool:
        while not es.stop():
            solutions = es.ask()
            print(solutions)
            fitnesses = pool.map(cma_objective, solutions)
            es.tell(solutions, fitnesses)
            es.disp()

    result = es.result
    best_x = result.xbest
    best_fitness = -result.fbest

    print("\n=== CMA-ES Finished ===")
    print("Best fitness:", best_fitness)

    weights_dict = {f"w{i}": int(best_x[i]) for i in range(len(best_x))}
    write_weights_file(weights_dict, "best_weights_cma.txt")
    print("Saved best weights to best_weights_cma.txt")


# ===============================
# MAIN
# ===============================

if __name__ == "__main__":
    if not os.path.exists(MINIMAX_ENGINE):
        print(f"Error: Minimax engine not found at {MINIMAX_ENGINE}")
        exit(1)
    if not os.path.exists(MZINGA_ENGINE):
        print(f"Error: Mzinga engine not found at {MZINGA_ENGINE}")
        exit(1)

    run_cma_es_parallel()
