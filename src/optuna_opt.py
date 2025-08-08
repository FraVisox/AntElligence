import csv
import optuna
import os
import tempfile
from subprocess import Popen, PIPE
import platform
import time

# Configuration
WEIGHTS_FILE = "./cpp_minimax/weights.txt"
RESULTS_FILE = "../results/optuna_optimization_results.csv"
NUMBER_OF_MATCHES = 5
NUMBER_OF_TURNS_PER_MATCH = 200
NUMBER_MOVES_LOG = 1

# Engine configuration
game_start = "newgame Base+MLP\n"
ending_sequence = "ok"
bestmove = "bestmove time 00:00:05\n" 

if platform.system() == "Linux":
    MINIMAX_ENGINE = "./cpp_minimax/main"
    MZINGA_ENGINE = "../dist/MzingaEngine"
else:
    MINIMAX_ENGINE = "./cpp_minimax/main.exe"
    MZINGA_ENGINE = "../dist/MzingaEngine.exe"

def write_weights_file(weights_dict, filename=WEIGHTS_FILE):
    """Write weights to the structured text file."""
    with open(filename, 'w') as f:
        # Write array weights
        for weight_name in ['inPlay', 'isPinned', 'isCovered', 'noisyWeight', 'quietWeight', 'friendlyNeighbor', 
                           'enemyNeighbor']:
            weights_str = ' '.join([str(weights_dict[f'{weight_name}_{i}']) for i in range(8)])
            f.write(f"{weights_str} ")
    
    # Write single value weights
    f.write(f"{weights_dict['pillbugNearQ']} {weights_dict['mosquitoNearStrong']} {weights_dict['spiderDistance']} {weights_dict['queenGate']} {weights_dict['ringPenalty']}")

def play_single_game(pl1_path, pl2_path, matches=NUMBER_OF_MATCHES):
    """
    Play a game between two engines and return stats.
    Returns: (pl1_wins, pl2_wins, draws, avg_turns_pl1_win, avg_turns_pl2_win)
    """
    try:
        print("New game: -------------------------------------------------------")
        # Start players as subprocesses
        player1 = Popen(pl1_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)
        player2 = Popen(pl2_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)

        # Make players present
        for player in [player1, player2]:
            output = player.stdout.readline()
            while not output.startswith(ending_sequence):
                output = player.stdout.readline()

        # Initialize stats
        stats = {
            "pl1_wins": 0, "pl2_wins": 0, "draws": 0,
            "pl1_win_turns": [], "pl2_win_turns": []
        }

        for match in range(1, matches + 1):
            # Start a new game
            player1.stdin.write(game_start)
            player2.stdin.write(game_start)
            for player in [player1, player2]:
                while not player.stdout.readline().startswith(ending_sequence):
                    pass
            
            # Set moving players: for odd matches player 1 starts, for even matches player 2 starts
            moving_player = player1 if match % 2 == 1 else player2
            opponent = player2 if match % 2 == 1 else player1
            
            # Player 1 is white if match is odd
            player1_is_white = (match % 2 == 1)

            # Simulate the game
            for turn in range(1, NUMBER_OF_TURNS_PER_MATCH + 1):
                if turn % NUMBER_MOVES_LOG == 0:
                    print(turn)
                # Get move from moving player
                moving_player.stdin.write(bestmove)
                move = moving_player.stdout.readline()
                output = moving_player.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = moving_player.stdout.readline()

                # Send move to both players
                move_cmd = f"play {move.strip()}\n"
                opponent.stdin.write(move_cmd)
                moving_player.stdin.write(move_cmd)

                # Get acknowledgment from moving player
                output = moving_player.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = moving_player.stdout.readline()

                # Get response from opponent
                response_lines = []
                while True:
                    line = opponent.stdout.readline()
                    response_lines.append(line)
                    if line.startswith(ending_sequence):
                        break

                response = ''.join(response_lines)
                print(response.strip())

                # Check for game end
                if "WhiteWins" in response:
                    if player1_is_white:
                        stats["pl1_wins"] += 1
                        stats["pl1_win_turns"].append(turn)
                    else:
                        stats["pl2_wins"] += 1
                        stats["pl2_win_turns"].append(turn)
                    break
                elif "BlackWins" in response:
                    if not player1_is_white:
                        stats["pl1_wins"] += 1
                        stats["pl1_win_turns"].append(turn)
                    else:
                        stats["pl2_wins"] += 1
                        stats["pl2_win_turns"].append(turn)
                    break
                elif "Draw" in response:
                    stats["draws"] += 1
                    break

                # Swap moving player
                moving_player, opponent = opponent, moving_player

        # Close subprocesses
        try:
            player1.stdin.write("exit\n")
            player2.stdin.write("exit\n")
        except:
            pass
        player1.terminate()
        player2.terminate()

        # Calculate averages
        avg_turns_pl1 = sum(stats["pl1_win_turns"]) / len(stats["pl1_win_turns"]) if stats["pl1_win_turns"] else 0
        avg_turns_pl2 = sum(stats["pl2_win_turns"]) / len(stats["pl2_win_turns"]) if stats["pl2_win_turns"] else 0
        
        print("Stats:", stats)
        return stats["pl1_wins"], stats["pl2_wins"], stats["draws"], avg_turns_pl1, avg_turns_pl2

    except Exception as e:
        print(f"Error during game simulation: {e}")
        return 0, 0, 0, 0, 0

def evaluate_weights(weights_dict):
    """
    Evaluate weights by playing against Mzinga.
    Returns a fitness score (higher is better).
    """
    # Write weights to file
    write_weights_file(weights_dict)

    # Play games
    minimax_wins, mzinga_wins, draws, avg_turns_minimax, avg_turns_mzinga = play_single_game(
        MINIMAX_ENGINE, MZINGA_ENGINE, NUMBER_OF_MATCHES
    )

    total_games = minimax_wins + mzinga_wins + draws
    if total_games == 0:
        return -1000  # Penalty for failure

    # --- Fitness components ---
    # 1. Win ratio
    win_ratio = minimax_wins / total_games

    # 2. Average turns to win (faster wins = better)
    quick_win_score = 0
    if minimax_wins > 0:
        quick_win_score = (1 - (avg_turns_minimax / NUMBER_OF_TURNS_PER_MATCH)) * (minimax_wins / total_games)

    # 3. Resistance to losing (longer games before losing = better)
    loss_resistance_score = 0
    if mzinga_wins > 0:
        loss_resistance_score = (avg_turns_mzinga / NUMBER_OF_TURNS_PER_MATCH) * (mzinga_wins / total_games)

    # 4. Draw score: scaled between win and loss
    draw_score = (draws / total_games) * 0.5

    # Combine components (weights can be tuned)
    fitness = (
        1.5 * win_ratio +
        0.5 * quick_win_score +
        0.3 * loss_resistance_score +
        0.4 * draw_score
    )

    print(f"Eval: Wins={minimax_wins}, Losses={mzinga_wins}, Draws={draws}, "
          f"AvgWinTurns={avg_turns_minimax:.1f}, AvgLossTurns={avg_turns_mzinga:.1f}, "
          f"Fitness={fitness:.4f}")

    return fitness


def objective(trial):
    """Optuna objective function."""
    
    # Define parameter ranges based on your current weights
    weights = {}
    
    # Single value parameters
    weights['pillbugNearQ'] = trial.suggest_float('pillbugNearQ', 1000, 600000)
    weights['mosquitoNearStrong'] = trial.suggest_float('mosquitoNearStrong', 1000, 600000)
    weights['spiderDistance'] = trial.suggest_float('spiderDistance', 1000, 600000)
    weights['queenGate'] = trial.suggest_float('queenGate', 1000, 600000)
    weights['ringPenalty'] = trial.suggest_float('ringPenalty', 1000, 600000)
    
    # Array parameters (8 pieces each: S, B, G, Q, A, M, L, P)
    weight_categories = [
        ('inPlay', -600000, 600000),
        ('isPinned', -600000, 600000),
        ('isCovered', -600000, 600000),
        ('noisyWeight', -600000, 600000),
        ('quietWeight', -600000, 600000),
        ('friendlyNeighbor', -600000, 600000), 
        ('enemyNeighbor', -600000, 600000)
    ]
    
    for category, min_val, max_val in weight_categories:
        for i in range(8):
            weights[f'{category}_{i}'] = trial.suggest_float(f'{category}_{i}', min_val, max_val)
    
    # Evaluate these weights
    fitness = evaluate_weights(weights)
    
    # Store additional info in trial
    trial.set_user_attr('weights', weights)
    
    return fitness

def save_optimization_results(study, filename=RESULTS_FILE):
    """Save optimization results to CSV."""
    trials_data = []
    for trial in study.trials:
        if trial.state == optuna.trial.TrialState.COMPLETE:
            row = [
                trial.number,
                trial.value,
                trial.datetime_start,
                trial.datetime_complete,
            ]
            # Add all parameters
            for param_name, param_value in trial.params.items():
                row.extend([param_name, param_value])
            trials_data.append(row)
    
    # Write to CSV
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Trial', 'Fitness', 'Start_Time', 'End_Time', 'Param_Name', 'Param_Value'])
        for row in trials_data:
            writer.writerow(row)

def main():
    """Main optimization loop."""
    print("Starting Optuna weight optimization...")
    print(f"Engine paths: Minimax={MINIMAX_ENGINE}, Mzinga={MZINGA_ENGINE}")
    
    # Check if engines exist
    if not os.path.exists(MINIMAX_ENGINE):
        print(f"Error: Minimax engine not found at {MINIMAX_ENGINE}")
        return
    if not os.path.exists(MZINGA_ENGINE):
        print(f"Error: Mzinga engine not found at {MZINGA_ENGINE}")
        return
    
    # Create study
    study = optuna.create_study(
        direction='maximize',
        study_name='weight_optimization',
        storage='sqlite:///optuna_weight_optimization.db',
        load_if_exists=True
    )
    
    # Add initial trial with current best weights (optional)
    initial_weights = {
        # InPlayWeight
        'inPlay_0': 25436, 'inPlay_1': -47942, 'inPlay_2': 62698, 'inPlay_3': 13433,
        'inPlay_4': 132380, 'inPlay_5': 35702, 'inPlay_6': 428, 'inPlay_7': -3,

        # IsPinnedWeight
        'isPinned_0': -46539, 'isPinned_1': -48021, 'isPinned_2': -82598, 'isPinned_3': -129954,
        'isPinned_4': -51036, 'isPinned_5': -47853, 'isPinned_6': 428, 'isPinned_7': 36,

        # IsCoveredWeight
        'isCovered_0': 7318, 'isCovered_1': 9325, 'isCovered_2': 24024, 'isCovered_3': -20359,
        'isCovered_4': -1515, 'isCovered_5': -5471, 'isCovered_6': 1, 'isCovered_7': 1,

        # NoisyMoveWeight
        'noisyWeight_0': 139756, 'noisyWeight_1': 142117, 'noisyWeight_2': 105511, 'noisyWeight_3': 72291,
        'noisyWeight_4': 53867, 'noisyWeight_5': 34855, 'noisyWeight_6': -327, 'noisyWeight_7': -327,

        # QuietMoveWeight
        'quietWeight_0': -20696, 'quietWeight_1': 11689, 'quietWeight_2': -12497, 'quietWeight_3': 95775,
        'quietWeight_4': 4129, 'quietWeight_5': 3658, 'quietWeight_6': -2037, 'quietWeight_7': -5,

        # FriendlyNeighborWeight
        'friendlyNeighbor_0': 21040, 'friendlyNeighbor_1': 36463, 'friendlyNeighbor_2': 13656, 'friendlyNeighbor_3': -317467,
        'friendlyNeighbor_4': 14307, 'friendlyNeighbor_5': 9199, 'friendlyNeighbor_6': 426, 'friendlyNeighbor_7': 1,

        # EnemyNeighborWeight
        'enemyNeighbor_0': 47143, 'enemyNeighbor_1': 63937, 'enemyNeighbor_2': 24316, 'enemyNeighbor_3': -538149,
        'enemyNeighbor_4': 21240, 'enemyNeighbor_5': 20205, 'enemyNeighbor_6': 0, 'enemyNeighbor_7': 25,

        # Single value weights
        'pillbugNearQ': 15000,
        'mosquitoNearStrong': 10000,
        'spiderDistance': 10000,
        'queenGate': 40000,
        'ringPenalty': 30000
    }

    
    try:
        study.enqueue_trial(initial_weights)
        print("Enqueued initial weights as first trial")
    except:
        print("Could not enqueue initial trial (may already exist)")
    
    # Run optimization
    print("Starting optimization...")
    try:
        study.optimize(objective, n_trials=1000, timeout=3600*12)  # 12 hours timeout
    except KeyboardInterrupt:
        print("Optimization interrupted by user")
    
    # Print results
    print("\nOptimization completed!")
    print(f"Number of finished trials: {len(study.trials)}")
    print(f"Best trial: {study.best_trial.number}")
    print(f"Best fitness: {study.best_value:.4f}")
    
    print("\nBest parameters:")
    for key, value in study.best_params.items():
        print(f"  {key}: {value:.4f}")
    
    # Save best weights to file
    best_weights_file = "best_weights.txt"
    write_weights_file(study.best_params, best_weights_file)
    print(f"\nBest weights saved to {best_weights_file}")
    
    # Save optimization history
    save_optimization_results(study)
    print(f"Optimization results saved to {RESULTS_FILE}")

if __name__ == "__main__":
    main()