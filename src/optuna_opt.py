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
        f.write(f"{weights_dict['QueenMoveWeight']} ")
        f.write(f"{weights_dict['numActionWeight']} ")
        
        # Write array weights
        for weight_name in ['noisyWeight', 'coveredWeight', 'numEnemyCloseWeight', 
                           'totalNumCloseWeight', 'placedWeight']:
            weights_str = ' '.join([str(weights_dict[f'{weight_name}_{i}']) for i in range(8)])
            f.write(f"{weights_str} ")

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
    weights['QueenMoveWeight'] = trial.suggest_float('QueenMoveWeight', 50, 400)
    weights['numActionWeight'] = trial.suggest_float('numActionWeight', 20, 150)
    
    # Array parameters (8 pieces each: S, B, G, Q, A, M, L, P)
    weight_categories = [
        ('noisyWeight', -200, 200),
        ('coveredWeight', -300, 300),
        ('numEnemyCloseWeight', -600, 200),
        ('totalNumCloseWeight', -600, 200),
        ('placedWeight', -600, 600)
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
        'QueenMoveWeight': 200,
        'numActionWeight': 80,
        'noisyWeight_0': 140, 'noisyWeight_1': 140, 'noisyWeight_2': 105, 'noisyWeight_3': 95,
        'noisyWeight_4': 53, 'noisyWeight_5': 34, 'noisyWeight_6': 140, 'noisyWeight_7': 100,
        'coveredWeight_0': -45, 'coveredWeight_1': -48, 'coveredWeight_2': -82, 'coveredWeight_3': -130,
        'coveredWeight_4': -51, 'coveredWeight_5': -47, 'coveredWeight_6': -45, 'coveredWeight_7': -70,
        'numEnemyCloseWeight_0': 47, 'numEnemyCloseWeight_1': 63, 'numEnemyCloseWeight_2': 24, 'numEnemyCloseWeight_3': -538,
        'numEnemyCloseWeight_4': 21, 'numEnemyCloseWeight_5': 20, 'numEnemyCloseWeight_6': 9, 'numEnemyCloseWeight_7': 30,
        'totalNumCloseWeight_0': 21, 'totalNumCloseWeight_1': 36, 'totalNumCloseWeight_2': 13, 'totalNumCloseWeight_3': -317,
        'totalNumCloseWeight_4': 14, 'totalNumCloseWeight_5': 9, 'totalNumCloseWeight_6': 4, 'totalNumCloseWeight_7': 40,
        'placedWeight_0': 25, 'placedWeight_1': 48, 'placedWeight_2': 63, 'placedWeight_3': 13,
        'placedWeight_4': 132, 'placedWeight_5': 35, 'placedWeight_6': 20, 'placedWeight_7': 50
    }
    
    try:
        study.enqueue_trial(initial_weights)
        print("Enqueued initial weights as first trial")
    except:
        print("Could not enqueue initial trial (may already exist)")
    
    # Run optimization
    print("Starting optimization...")
    try:
        study.optimize(objective, n_trials=100, timeout=3600*4)  # 4 hours timeout
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