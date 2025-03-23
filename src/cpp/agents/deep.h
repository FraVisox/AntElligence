/*

TODO: per verificare se facciamo qualche mossa illegale semplicemente vai su cpp corretto e prova a dirgli "Inizializza questa partita Base;..."
e se ce lo fa fare non abbiamo fatto nessuna mossa sbagliata. Ancora meglio è farlo fare a Mzinga



ALPHA GO: 
https://jonathan-hui.medium.com/alphago-how-it-works-technically-26ddcc085319

Train 2 models:
- first one gives the policy = which moves is it better to do in a given state, based on various games already played and recorded
- second one gives the value = how good is it to be in a given state (to be in a board)

Then we do a MCTS using the policy to understand which moves are better and the value to understand which board positions are good

ALPHA GO ZERO: beats easily alphago
https://deepmind.google/discover/blog/alphago-zero-starting-from-scratch/

Previous versions of AlphaGo initially trained on thousands of human amateur and professional games to learn how to play Go. 
AlphaGo Zero skips this step and learns to play simply by playing games against itself, starting from completely random play.

The system starts off with a neural network that knows nothing about the game of Go. It then plays games against itself, 
by combining this neural network with a powerful search algorithm. As it plays, the neural network is tuned and updated to 
predict moves, as well as the eventual winner of the games.

This updated neural network is then recombined with the search algorithm to create a new, stronger version of AlphaGo Zero, 
and the process begins again. In each iteration, the performance of the system improves by a small amount, and the quality 
of the self-play games increases, leading to more and more accurate neural networks and ever stronger versions of AlphaGo Zero.

- AlphaGo Zero only uses the black and white stones from the Go board as its input, whereas previous versions of AlphaGo 
included a small number of hand-engineered features.
- It uses one neural network rather than two.
- AlphaGo Zero does not use “rollouts”

See the full article in res






*/