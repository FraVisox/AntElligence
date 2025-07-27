import time
import platform
from subprocess import Popen, PIPE

# Select engine path based on OS
if platform.system() == "Linux":
    engine_path = "./cpp_minimax/main"
else:
    engine_path = "./cpp_minimax/main.exe"

# Engine commands
start_game_command = "newgame Base+MLP\n"
bestmove_command = "bestmove time 00:00:05\n"
end_sequence = "ok"

def wait_for_ok(process):
    """Wait until the engine outputs a line starting with 'ok'."""
    line = process.stdout.readline()
    while line and not line.startswith(end_sequence):
        print(line)
        line = process.stdout.readline()
    return line

def get_bestmove(process):
    """Get the move returned by bestmove command."""
    line = process.stdout.readline()
    while line and not line.strip() and not line.startswith("ok"):
        line = process.stdout.readline()
    move = line.strip()
    wait_for_ok(process)
    return move

def measure_bestmove_response_time():
    # Start the engine
    engine = Popen(engine_path, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF-8", bufsize=1)

    # Wait for greeting
    wait_for_ok(engine)

    # Start game
    engine.stdin.write(start_game_command)
    engine.stdin.flush()
    wait_for_ok(engine)

    timings = []

    for i in range(200):
        start = time.time()

        # Send bestmove
        engine.stdin.write(bestmove_command)
        engine.stdin.flush()

        # Capture move and wait for 'ok'
        move = get_bestmove(engine)

        end = time.time()
        duration = end - start
        timings.append(duration)

        # Send play <move>
        engine.stdin.write(f"play {move}\n")
        engine.stdin.flush()
        wait_for_ok(engine)

        print(f"[{i+1}] Move: {move} | Time: {duration:.4f} seconds")

    # Exit engine
    engine.stdin.write("exit\n")
    engine.stdin.flush()
    engine.terminate()

    max_time = max(timings)
    print(f"\nMax bestmove time over 100 iterations: {max_time:.4f} seconds")
    return max_time

if __name__ == "__main__":
    measure_bestmove_response_time()
