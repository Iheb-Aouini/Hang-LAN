from flask import Flask, render_template, request, redirect, url_for, flash
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager, UserMixin, login_user, logout_user, login_required, current_user
from flask import request, flash, redirect, url_for
from flask_login import logout_user
from flask_migrate import Migrate
from datetime import datetime
import random

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///hangman.db'
app.config['SECRET_KEY'] = 'your_secret_key'

db = SQLAlchemy(app)
migrate = Migrate(app, db)

login_manager = LoginManager(app)
login_manager.login_view = 'login'



# User model for database
class User(UserMixin, db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(100), unique=True, nullable=False)
    password = db.Column(db.String(100), nullable=False)
    score_easy = db.Column(db.Integer, default=0)
    score_medium = db.Column(db.Integer, default=0)
    score_hard = db.Column(db.Integer, default=0)
    game_history = db.relationship('GameHistory', backref='user')

# GameHistory model for database
class GameHistory(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'))
    difficulty = db.Column(db.Integer)
    score = db.Column(db.Integer)
    guessed_word = db.Column(db.String(255))
    correct_guesses = db.Column(db.Integer)  # Add correct_guesses column
    total_attempts = db.Column(db.Integer)
    date = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)

# Initialize Flask-Login
@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))

# Hangman game logic
class HangmanGame:
    def __init__(self):
        self.difficulty = None
        self.word = None
        self.guessed_word = None
        self.attempts_left = None
        self.guessed_letters = None

    def set_difficulty(self, difficulty):
        self.difficulty = difficulty
        self.word = self.choose_random_word()
        self.guessed_word = ['*' if letter != '#' else ' ' for letter in self.word]
        self.attempts_left = self.get_attempts_left()
        self.guessed_letters = []

    def choose_random_word(self):
        with open("console_hangman/input_output_files/dictionnaire.txt", "r") as file:
            words = [word.strip() for word in file.readlines()]

        if self.difficulty == 1:
            words = [word for word in words if len(word) >= 1 and len(word) <= 4]
        elif self.difficulty == 2:
            words = [word for word in words if len(word) >= 5 and len(word) <= 6]
        elif self.difficulty == 3:
            words = [word for word in words if len(word) >= 7]

        return random.choice(words)

    def get_attempts_left(self):
        if self.difficulty == 1:
            return 10
        elif self.difficulty == 2:
            return 8
        elif self.difficulty == 3:
            return 7

    def guess_letter(self, letter):
        if letter in self.guessed_letters:
            return "You already guessed this letter!"

        self.guessed_letters.append(letter)

        if letter in self.word:
            for i, char in enumerate(self.word):
                if char == letter:
                    self.guessed_word[i] = letter
        else:
            self.attempts_left -= 1

        if self.attempts_left == 0:
            # Update score based on difficulty
            if self.difficulty == 1 and current_user.score_easy is not None:
                current_user.score_easy -= 1
            elif self.difficulty == 2 and current_user.score_medium is not None:
                current_user.score_medium -= 1
            elif self.difficulty == 3 and current_user.score_hard is not None:
                current_user.score_hard -= 1

            # Add game history
            game_history = GameHistory(
                user_id=current_user.id,
                difficulty=self.difficulty,
                score=0,
                guessed_word=''.join(self.guessed_word),
                correct_guesses=sum(1 for char in self.guessed_word if char != '*'),
                total_attempts=10 if self.difficulty == 1 else (8 if self.difficulty == 2 else 7)
            )
            db.session.add(game_history)
            db.session.commit()

            return f"Game Over! The word was '{self.word}'."
        elif '*' not in self.guessed_word:
            # Update score based on difficulty
            if self.difficulty == 1:
                if current_user.score_easy is not None:
                    current_user.score_easy += 1
            elif self.difficulty == 2:
                if current_user.score_medium is not None:
                    current_user.score_medium += 1
            elif self.difficulty == 3:
                if current_user.score_hard is not None:
                    current_user.score_hard += 1

            # Add game history
            game_history = GameHistory(
                user_id=current_user.id,
                difficulty=self.difficulty,
                score=1,
                guessed_word=''.join(self.guessed_word),
                correct_guesses=sum(1 for char in self.guessed_word if char != '*'),
                total_attempts=10 if self.difficulty == 1 else (8 if self.difficulty == 2 else 7)
            )
            db.session.add(game_history)
            db.session.commit()

            return "Congratulations! You guessed the word!"
        else:
            return ""

game = HangmanGame()



# Routes
@app.route('/')
def index():
    if not current_user.is_authenticated:
        return redirect(url_for('login'))
    return render_template('index.html')

@app.route('/scores')
@login_required
def scores():
    if current_user.is_authenticated:
        # Querying the best scores for each difficulty level
        best_scores = {
            'Easy': GameHistory.query.filter_by(user_id=current_user.id, difficulty=1).order_by(GameHistory.score.desc()).first(),
            'Medium': GameHistory.query.filter_by(user_id=current_user.id, difficulty=2).order_by(GameHistory.score.desc()).first(),
            'Hard': GameHistory.query.filter_by(user_id=current_user.id, difficulty=3).order_by(GameHistory.score.desc()).first()
        }

        # Calculating the score for each difficulty level
        scores = {}
        for difficulty, history in best_scores.items():
            if history:
                correct_guesses = history.correct_guesses
                incorrect_guesses = history.total_attempts - correct_guesses
                remaining_time = (datetime.utcnow() - history.date).total_seconds()  # Assuming date is the start time
                remaining_time = max(0, remaining_time)  # Ensure remaining_time is not negative
                if difficulty == 'Easy':
                    scores[difficulty] = correct_guesses * 10 - incorrect_guesses * 5 + remaining_time * 2
                elif difficulty == 'Medium':
                    scores[difficulty] = correct_guesses * 15 - incorrect_guesses * 8 + remaining_time * 3
                elif difficulty == 'Hard':
                    scores[difficulty] = correct_guesses * 20 - incorrect_guesses * 10 + remaining_time * 4
            else:
                scores[difficulty] = None

        return render_template('scores.html', user=current_user, best_scores=best_scores, scores=scores)
    else:
        flash('No score available for this user!')
        return redirect(url_for('index'))


@app.route('/history')
@login_required
def history():
    games = GameHistory.query.filter_by(user_id=current_user.id).all()
    return render_template('history.html', games=games)

@app.route('/game', methods=['GET', 'POST'])
@login_required
def hangman_game():
    if request.method == 'POST':
        letter = request.form['letter']
        message = game.guess_letter(letter)
        if "*" not in game.guessed_word or game.attempts_left == 0:
            # Game won or lost
            return render_template('game.html', guessed_word=game.guessed_word, attempts_left=game.attempts_left, message=message, popup=True)
        else:
            return render_template('game.html', guessed_word=game.guessed_word, attempts_left=game.attempts_left, message=message, popup=False)
    else:
        return render_template('game.html', guessed_word=game.guessed_word, attempts_left=game.attempts_left, message="", popup=False)

@app.route('/difficulty_selection', methods=['GET', 'POST'])
@login_required
def difficulty_selection():
    if request.method == 'POST':
        difficulty = int(request.form['difficulty'])
        game.set_difficulty(difficulty)
        return render_template('game.html', guessed_word=' '.join(game.guessed_word), attempts_left=game.attempts_left, message="")
    else:
        return render_template('difficulty_selection.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        existing_user = User.query.filter_by(username=username).first()
        if existing_user:
            flash('Username already exists! Please choose a different one.')
            return redirect(url_for('register'))
        new_user = User(username=username, password=password, score_easy=0, score_medium=0, score_hard=0)  # Initialize scores
        db.session.add(new_user)
        db.session.commit()
        flash('Registration successful! Please login.')
        return redirect(url_for('login'))
    return render_template('register.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        user = User.query.filter_by(username=username).first()
        if user and user.password == password:
            # Initialize scores if not set
            if user.score_easy is None:
                user.score_easy = 0
            if user.score_medium is None:
                user.score_medium = 0
            if user.score_hard is None:
                user.score_hard = 0
            login_user(user)
            return redirect(url_for('index'))
        else:
            flash('Invalid username or password')
    return render_template('login.html')

@app.route('/logout', methods=['GET', 'POST'])
@login_required
def logout():
    try:
        logout_user()
        flash('You have been logged out successfully.', 'success')
        return redirect(url_for('login'))
    except Exception as e:
        flash('An error occurred while logging out. Please try again.', 'error')
        return redirect(url_for('index')) 
if __name__ == '__main__':
    with app.app_context():
        db.create_all()
        app.run(debug=True, port=62053)


