import tkinter as tk
from tkinter import messagebox
import random

class DifficultySelection:
    def __init__(self, root):
        # Initialize the DifficultySelection window
        self.root = root
        self.root.title("Select Difficulty Level")
        self.root.geometry("400x300")
        self.root.configure(bg="#1c1c1c")

        # Label for selecting difficulty level
        self.label = tk.Label(root, text="Select Difficulty Level:", font=("Arial", 18), bg="#1c1c1c", fg="white")
        self.label.pack(pady=10)

        # Default difficulty level
        self.difficulty = tk.IntVar(root, value=1)
        options = [("Easy", 1), ("Medium", 2), ("Hard", 3)]
        for text, value in options:
            button = tk.Button(root, text=text, font=("Arial", 14), command=lambda v=value: self.set_difficulty(v), bg="#37474f", fg="white", activebackground="#546e7a", activeforeground="white")
            button.pack(pady=5, ipadx=20, ipady=10)
            button.bind("<Enter>", lambda event, b=button: self.on_enter(event, b))
            button.bind("<Leave>", lambda event, b=button: self.on_leave(event, b))

    def on_enter(self, event, button):
        # Change button color on mouse hover
        button.config(bg="#78909c")

    def on_leave(self, event, button):
        # Restore button color on mouse leave
        button.config(bg="#37474f")

    def set_difficulty(self, value):
        # Set selected difficulty level
        self.difficulty.set(value)
        self.confirm_difficulty()

    def confirm_difficulty(self):
        # Close the DifficultySelection window and start the HangmanGame with selected difficulty
        self.root.destroy()
        difficulty_level = self.difficulty.get()
        root = tk.Tk()
        root.configure(bg='#f0f0f0')
        root.geometry("800x600")
        root.title("Hangman Game")
        game = HangmanGame(root, difficulty_level)
        root.mainloop()

class HangmanGame:
    def __init__(self, root, difficulty):
        # Initialize the HangmanGame window
        self.root = root
        self.root.title("Hangman Game")
        self.root.geometry("800x600")
        self.root.configure(bg="#f0f0f0")  # Changed background color

        # Canvas for displaying the word
        self.canvas = tk.Canvas(root, width=600, height=400, bg='#ffffff')  # Changed canvas background color
        self.canvas.pack(pady=20)

        # Initialize game parameters
        self.difficulty = difficulty
        self.word = self.choose_random_word()
        self.guessed_word = ['*' if letter != '#' else ' ' for letter in self.word]
        self.attempts_left = self.get_attempts_left()
        self.guessed_letters = []

        # Label for displaying attempts left and guessed letters
        self.label_attempts = tk.Label(root, text=f"Attempts left: {self.attempts_left}\nGuessed letters: ", font=("Arial", 16), bg="#f0f0f0", fg="black")  # Changed label background color
        self.label_attempts.pack(pady=10)

        # Buttons for guessing letters
        self.create_letter_buttons()

        # Restart button
        self.button_restart = tk.Button(root, text="Restart", font=("Arial", 14), command=self.restart_game, bg="#37474f", fg="white", activebackground="#546e7a", activeforeground="white")
        self.button_restart.pack(pady=10)

        # Display the initial word
        self.display_word()

        # Dark mode flag
        self.dark_mode = False

        # Create menu for toggling dark mode
        self.create_menu()

    def create_menu(self):
        # Create a menu for toggling dark mode
        menu = tk.Menu(self.root)
        self.root.config(menu=menu)
        theme_menu = tk.Menu(menu, tearoff=False)
        theme_menu.add_command(label="Toggle Dark Mode", command=self.toggle_dark_mode)
        menu.add_cascade(label="Theme", menu=theme_menu)

    def toggle_dark_mode(self):
        # Toggle dark mode
        self.dark_mode = not self.dark_mode
        if self.dark_mode:
            # Set dark mode colors
            self.root.config(bg="#263238")
            self.canvas.config(bg="#37474f")
            self.label_attempts.config(bg="#263238", fg="white")
            self.button_restart.config(bg="#78909c", fg="white", activebackground="#546e7a", activeforeground="white")
        else:
            # Set light mode colors
            self.root.config(bg="#f0f0f0")
            self.canvas.config(bg="white")
            self.label_attempts.config(bg="#f0f0f0", fg="black")
            self.button_restart.config(bg="#b3d9ff", fg="white", activebackground="#80bfff", activeforeground="white")

    def get_attempts_left(self):
        # Get the number of attempts based on difficulty level
        if self.difficulty == 1:
            return 10
        elif self.difficulty == 2:
            return 8
        elif self.difficulty == 3:
            return 7

    def choose_random_word(self):
        # Choose a random word from the dictionary based on difficulty level
        with open("dictionnaire.txt", "r") as file:
            words = [word.strip() for word in file.readlines()]

        if self.difficulty == 1:
            words = [word for word in words if len(word) >= 1 and len(word) <= 4]
        elif self.difficulty == 2:
            words = [word for word in words if len(word) >= 5 and len(word) <= 6]
        elif self.difficulty == 3:
            words = [word for word in words if len(word) >= 7]

        return random.choice(words)

    def display_word(self):
        # Display the word on the canvas
        self.canvas.delete("word")
        word_display = " ".join(self.guessed_word)
        self.canvas.create_text(300, 200, text=word_display, font=("Arial", 36), tags="word", fill="black")  # Changed text color

    def guess_letter(self, letter):
        # Guess a letter
        if letter in self.guessed_letters:
            messagebox.showwarning("Warning", "You already guessed this letter!")
            return

        self.guessed_letters.append(letter)

        if letter in self.word:
            for i, char in enumerate(self.word):
                if char == letter:
                    self.guessed_word[i] = letter
        else:
            self.attempts_left -= 1

        self.display_word()
        self.label_attempts.config(text=f"Attempts left: {self.attempts_left}\nGuessed letters: {' '.join(self.guessed_letters)}")

        if self.attempts_left == 0:
            messagebox.showinfo("Game Over", f"Sorry, you ran out of attempts. The word was '{self.word}'.")
            self.restart_game()

        if '*' not in self.guessed_word:
            messagebox.showinfo("Congratulations!", "You guessed the word!")
            self.restart_game()

    def create_letter_buttons(self):
        # Create buttons for guessing letters
        alphabet = 'abcdefghijklmnopqrstuvwxyz'
        frame = tk.Frame(self.root, bg='#f0f0f0')  # Changed frame background color
        frame.pack()
        for i, letter in enumerate(alphabet):
            row_index = i // 7  # Change the number of buttons per row by adjusting the denominator
            column_index = i % 7  # Change the number of buttons per row by adjusting the denominator
            button = tk.Button(frame, text=letter, font=("Arial", 14), command=lambda l=letter: self.guess_letter(l), bg="#37474f", fg="white", activebackground="#546e7a", activeforeground="white")
            button.grid(row=row_index, column=column_index, padx=5, pady=5, ipadx=10, ipady=5)
            button.bind("<Enter>", lambda event, b=button: self.on_enter(event, b))
            button.bind("<Leave>", lambda event, b=button: self.on_leave(event, b))

    def on_enter(self, event, button):
        # Change button color on mouse hover
        button.config(bg="#78909c")

    def on_leave(self, event, button):
        # Restore button color on mouse leave
        button.config(bg="#37474f")

    def restart_game(self):
        # Restart the game with a new word
        self.word = self.choose_random_word()
        self.guessed_word = ['*' if letter != '#' else ' ' for letter in self.word]
        self.attempts_left = self.get_attempts_left()
        self.guessed_letters = []

        self.canvas.delete("all")
        self.display_word()
        self.label_attempts.config(text=f"Attempts left: {self.attempts_left}\nGuessed letters: ")

if __name__ == "__main__":
    root = tk.Tk()
    root.configure(bg="#f0f0f0")
    difficulty_selection = DifficultySelection(root)
    root.mainloop()

