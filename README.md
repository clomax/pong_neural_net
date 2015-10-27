#PUNG

##Neural network controlled Pong clone

I was asked if it was possible to train a neural network to play a game
in a similar style to how a human would play; this is my attempt to answer that
question.

It works by collecting data from a human player, training a neural network on that data,
and importing it back into the game.


##Compilation

You'll need SFML-dev, Box2D, and the UBLAS stuff from Boost. If you have that
you should be able to just run `make`.

If you're going to try to compile this on Windows: best of luck to you, I haven't even tried.


##How does it work?

If you just want to see it play with an already trained network, run:

  `pung -p 1 -f goalkeeper-100.csv -n 100`

You should see the paddle stay in the middle and *"dive"* for the ball when
it goes to his side of the court, hence *goalkeeper*.

If you want to train the network with your own data -
first, you will need data. Run:

  `pung -p 0 -f <filename>`

to start PUNG in data-collection mode and collect ~3MB of data, which should take
around half an hour. Only give it the name of the file; it writes to
`data/human/<filename>`. It appends to the file so you don't have to collect
all of the data in one run; you can stop and start it as you wish with the same
file name.

Secondly, you'll need R to train the neural network. The script is in `src/learning/`:

  `nnet.R [-i <in_file> -o <out_file> -t <training_set_size> -h <hidden_nodes> -a <learning rate> -e <number_of_epochs>]`

The training set size is any number up to the number of lines in the `<in_file>`. You should experiment with the other values
but here's the starting point:

- 50 hidden nodes
- 0.2 learning rate
- 1000 epochs

Once training is done, look at the image saved in `img/` and confirm that the error with respect to epochs trends downwards.
The result of training is saved to `data/ai/<out_file>`

Thirdly, run PUNG again with mode 1, that filename in `data/ai/`, and the number of hidden nodes used for training:

  `pung -p 1 -f <filename> -n 100`

If everything went well, you should see the red paddle play in a similar style to how you played when recording that data.

