# Hangman Game with Binary Search Tree

Welcome to our Hangman game project! We've created a fun Hangman game using a Binary Search Tree (BST) to store the words you'll be playing with. Not only do we have a classic console interface with some cool features like sounds, timers, and scores, but we've also built a multiplayer Hangman game server using multithreading. Plus, we've got graphical user interfaces (GUI) for both desktop and web!

## Requirements

To get started, make sure you have the following installed:

- `make`
- `gcc`
- `pulseaudio-utils`
- `python3`
- Python packages:
  - `flask`
  - `flask_sqlalchemy`
  - `flask_login`
  - `flask_migrate`

You can install these dependencies on Fedora using:

sudo dnf install make gcc pulseaudio-utils python3
sudo python3 -m pip install flask flask_sqlalchemy flask_login flask_migrate

## Compilation and Execution

To compile the project, simply run:

make

To run the program, use:

./program_entry

If you encounter a memory error, try running with `valgrind`:

valgrind ./program_entry

To clean up the executables, just run:

make clean

## Project Structure

Here's a quick overview of how our project is organized:

- `src/`: This is where all the C source code lives.
- `gui/`: Here's where you'll find the Tkinter and Flask GUI code.
- `data/`: We store any necessary data files here.

## Usage

- For the console interface, just follow the prompts to play Hangman. We've added some extra features like sound effects, timers, and scores to spice things up.
- If you're up for a challenge, fire up the multiplayer Hangman game server and connect with your friends for some competitive fun!
- And don't forget to check out our graphical user interfaces:
  - Run the desktop GUI using Tkinter by running the appropriate script.
  - Start the web GUI using Flask by firing up the Flask server.

## Contributing

We'd love your contributions! If you've got ideas or want to help out, here's how you can get involved:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -am 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a pull request

## License

This project is licensed under the [MIT License](LICENSE).
