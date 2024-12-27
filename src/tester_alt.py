import csv
from subprocess import Popen, PIPE
import platform
from agents.strategy import Strategy
from agents.minimax import Minimax
from agents.random_strat import Random as RandStrat
from game_logic.board import *

file_results = "results\\game_results.csv"



player=[
    {
    "name":"Minmax",
    "strategy":RandStrat,
    "options": ["depth",3]
    },

    {
    "name":"random",
    "strategy":Minimax,
    "options": ["depth",3]
    }

]

def write_results_to_csv(results, filename):
    """
    Writes the results of a game to a CSV file.

    The results are passed as a list of lists, where each sublist contains the results of a single game.
    The first two elements of each sublist are the names of the two players, in the order they played.
    The next 8 elements are the results of the game, in the following order:

    - Player 1 wins as white
    - Average turns for Player 1 to win as white
    - Player 1 wins as black
    - Average turns for Player 1 to win as black
    - Player 2 wins as white
    - Average turns for Player 2 to win as white
    - Player 2 wins as black
    - Average turns for Player 2 to win as black
    - Draws
    - Average turns for draws

    The results will be written to the specified file, with the headers written first.

    :param results: The results of the games to be written.
    :type results: list of lists
    :param filename: The file to write the results to.
    :type filename: str
    """

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


    
def play_game_fast(strat1,  strat2,  matches, turns=-1, options1=None, options2=None):
    """
    As before, but using the python code directly instead of going throw a compiled code

    """
    result=[0]*10

    player1:Strategy=strat1()
    player2:Strategy=strat2()
    if options1:
        if options1[0]=="depth":
            player1.set_depth_limit(options1[1])
        if options1[0]=="time":
            player1.set_time_limit(options1[1])

    if options2:
        if options2[0]=="depth":
            player2.set_depth_limit(options2[1])
        if options2[0]=="time":
            player2.set_time_limit(options2[1])


    

    for numGame in range(1,matches+1):
        b=Board()
        print(f"start game {numGame}")
        if(numGame%2==0):
            gamer1=player1
            gamer2=player2
        else:
            gamer1=player2
            gamer2=player1
        turn=1
        while(turn <= turns):
            nextMove=""
            if turn%2==1:
                nextMove=gamer1.calculate_best_move(b)
            else:
                nextMove=gamer2.calculate_best_move(b)
            b.play(nextMove)
            
            print(f"{turn}. {nextMove}")
            if not b.state.IN_PROGRESS:
                if b.state.WHITE_WINS:
                    if(numGame%2==0):
                        result[0]+=1
                        result[1]+=turn
                    else:
                        result[4]+=1
                        result[5]+=turn
                if b.state.BLACK_WINS:
                    if(numGame%2==1):
                        result[2]+=1
                        result[3]+=turn
                    else:
                        result[6]+=1
                        result[7]+=turn
                if b.state.DRAW:
                    result[8]+=1
                    result[9]+=1
                break
            turn+=1
    for i in range(5):
        if result[i*2]!=0:
            result[i*2+1]/=result[i*2]
    return result
    
        

def test_players(infoMatch, file_results):
    
    results=[i['name'] for i in infoMatch]
    p1=infoMatch[0]
    p2=infoMatch[1]
    results.extend(play_game_fast(p1['strategy'], p2['strategy'], matches=6, turns=40, options1=p1['options'], options2=p2['options']))
    write_results_to_csv([results], file_results)


# Run the tests
if __name__ == "__main__":
    test_players(player, file_results)
