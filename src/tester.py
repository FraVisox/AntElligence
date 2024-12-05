import csv
from subprocess import Popen, PIPE

file_results = "results\\game_results.csv"

files_to_match = {
    "Random": "C:\\Users\\vison\\OneDrive\\Desktop\\Progetti\\Hive\\AntElligence\\dist\\AntElligenceEngine.exe",
    "Minimax": "C:\\Users\\vison\\OneDrive\\Desktop\\Progetti\\Hive\\AntElligence\\dist\\AntElligenceEngine.exe",
    #"Mzinga": "C:\\Users\\vison\\OneDrive\\Desktop\\Progetti\\Hive\\AntElligence\\dist\\MzingaEngine.exe"
}

options_players = [
    None,
    "options set Strategy Minimax\n",
    None
]

# Commands to pass to the engine
game_start = "newgame Base+MLP\n"
ending_sequence = "ok"
#bestmove = "bestmove time 00:00:01\n"
bestmove = "bestmove depth 10\n"


def write_results_to_csv(results, filename):
    """Write results of the matches to a CSV file."""
    headers = [
        "Player 1", "Player 2", 
        "Player 1 Wins (White)", "Turns (White)",
        "Player 1 Wins (Black)", "Turns (Black)",
        "Player 2 Wins (White)", "Turns (White)",
        "Player 2 Wins (Black)", "Turns (Black)",
        "Draws", "Turns (Draw)"
    ]

    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(headers)
        for row in results:
            writer.writerow(row)

def play_game(pl1_path, name1, pl2_path, name2, matches, turns=-1, options1 = None, options2 = None):
    """Simulate some matches between two players with a maximum number of turns and return the results as a list."""
    results = []

    try:
        # Start players as subprocesses
        player1 = Popen(pl1_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)
        player2 = Popen(pl2_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)

        # Make players present
        for player, label in [(player1, name1), (player2, name2)]:
            print(f"{label} presenting:")
            output = player.stdout.readline()
            print(output.strip())
            while not output.startswith(ending_sequence):
                output = player.stdout.readline()

        # Pass options
        if options1:
            player1.stdin.write(options1)
            output = player1.stdout.readline()
            print(output)
            while not output.startswith(ending_sequence):
                print(output)
                output = player1.stdout.readline()
        if options2:
            player2.stdin.write(options2)
            output = player2.stdout.readline()
            while not output.startswith(ending_sequence):
                print(output)
                output = player2.stdout.readline()


        # Initialize stats
        stats = {
            "pl1_wins_white": 0, "pl1_turns_white": 0,
            "pl1_wins_black": 0, "pl1_turns_black": 0,
            "pl2_wins_white": 0, "pl2_turns_white": 0,
            "pl2_wins_black": 0, "pl2_turns_black": 0,
            "draws": 0, "draw_turns": 0
        }

        for match in range(1, matches + 1):
            print(f"Starting Match {match}")
            # Start a new game
            player1.stdin.write(game_start)
            player2.stdin.write(game_start)
            for player in [player1, player2]:
                while not player.stdout.readline().startswith(ending_sequence):
                    pass
            
            # Set moving players: for odd matches player 1 is moving, for even matches player 2 is moving
            moving_player = player1 if match % 2 == 1 else player2
            opponent = player2 if match % 2 == 1 else player1

            # Simulate the game
            for turn in range(1, turns + 1 if turns > 0 else float('inf')):
                moving_player.stdin.write(bestmove)
                move = output = moving_player.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = moving_player.stdout.readline()

                move_cmd = f"play {move.strip()}\n"
                opponent.stdin.write(move_cmd)
                moving_player.stdin.write(move_cmd)

                output = moving_player.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = moving_player.stdout.readline()

                response = output = opponent.stdout.readline()
                while not output.startswith(ending_sequence):
                    output = opponent.stdout.readline()

                # Check for game end
                if "WhiteWins" in response:
                    if moving_player is player1:
                        stats["pl1_wins_white" if match % 2 == 1 else "pl1_wins_black"] += 1
                        stats["pl1_turns_white" if match % 2 == 1 else "pl1_turns_black"] += turn
                    else:
                        stats["pl2_wins_white" if match % 2 == 1 else "pl2_wins_black"] += 1
                        stats["pl2_turns_white" if match % 2 == 1 else "pl2_turns_black"] += turn
                    break
                elif "BlackWins" in response:
                    if moving_player is player1:
                        stats["pl1_wins_black" if match % 2 == 0 else "pl1_wins_white"] += 1
                        stats["pl1_turns_black" if match % 2 == 0 else "pl1_turns_white"] += turn
                    else:
                        stats["pl2_wins_black" if match % 2 == 0 else "pl2_wins_white"] += 1
                        stats["pl2_turns_black" if match % 2 == 0 else "pl2_turns_white"] += turn
                    break
                elif "Draw" in response:
                    stats["draws"] += 1
                    stats["draw_turns"] += turn
                    break

                # Swap moving player
                moving_player, opponent = opponent, moving_player

                print(turn)


        # Close subprocesses
        player1.stdin.write("exit\n")
        player2.stdin.write("exit\n")
        player1.terminate()
        player2.terminate()

        results.append([
            name1, name2,
            stats["pl1_wins_white"], stats["pl1_turns_white"]/stats["pl1_wins_white"] if stats["pl1_wins_white"] != 0 else 0,
            stats["pl1_wins_black"], stats["pl1_turns_black"]/stats["pl1_wins_black"] if stats["pl1_wins_black"] != 0 else 0,
            stats["pl2_wins_white"], stats["pl2_turns_white"]/stats["pl2_wins_white"] if stats["pl2_wins_white"] != 0 else 0,
            stats["pl2_wins_black"], stats["pl2_turns_black"]/stats["pl2_wins_black"] if stats["pl2_wins_black"] != 0 else 0,
            stats["draws"], stats["draw_turns"]/stats["draws"] if stats["draws"] != 0 else 0
        ])

    except Exception as e:
        print(f"Error during game simulation: {e}")

    return results

def test_players(files_to_match, options, file_results):
    results = []
    keys = list(files_to_match.keys())
    for i in range(len(keys)):
        for j in range(i + 1, len(keys)):
            pl1 = files_to_match[keys[i]]
            pl2 = files_to_match[keys[j]]
            results.extend(play_game(pl1, keys[i], pl2, keys[j], matches=6, turns=100, options1=options[i], options2=options[j]))
    write_results_to_csv(results, file_results)

# Run the tests
test_players(files_to_match, options_players, file_results)