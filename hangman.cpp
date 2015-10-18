#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <fstream>
#include <algorithm>
#include <iterator>
#include "hangman.h"

using namespace std;

NoFileException::NoFileException(){}		//exception class for no input file

OpenFileException::OpenFileException(std::string arg) : arg_(arg) {}	//exception class for bad input file

string OpenFileException::arg() {
	return arg_;
}

EmptyPoolException::EmptyPoolException() {}		//exception class for empty pool of gamewords

LetterGuessedException::LetterGuessedException(char letter) {		//exception class for already guessed letter
	letter_ = letter;
}

char LetterGuessedException::letter() const {
	return letter_;
}

string transformtolower(string word) {		//helper function to lower case a string
	transform(word.begin(), word.end(), word.begin(), ::tolower);
	return word;
}

bool nonalphalt6(string word) {		//checks for invalid game words from list
	string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	return (word.find_first_not_of(alphabet) != string::npos || word.length() < 6);
}

char tolowerchar(char let) {		//lower case character
	return tolower(let);
}

char toupperchar(char let) {		//upper case character
	return toupper(let);
}

void print(vector<char> guessed, vector<char> notguessed, int lives, string gameword) {		//output game stat
	for_each(notguessed.begin(), notguessed.end(), [&gameword](char let) mutable->string {replace(gameword.begin(), gameword.end(), tolowerchar(let), '-'); return gameword; });		//replace all unguessed lower case letters with '-'
	for_each(notguessed.begin(), notguessed.end(), [&gameword](char let) mutable->string {replace(gameword.begin(), gameword.end(), toupperchar(let), '-'); return gameword; });		//replace all unguessed upper case letters with '-'
	cout << "Word: " << gameword << endl;
	cout << "Letters used: ";
	ostream_iterator<char> out_it(cout, " ");		//output vector of guessed letters
	copy(guessed.begin(), guessed.end(), out_it);
	cout << endl;
	if (lives == 1)
		cout << "You have " << lives << " life left." << endl;
	else
		cout << "You have " << lives << " lives left." << endl;
	cout << "Next guess: ";
}

char playagain() {		//helper function to determine whether player wants to play again
	cout << "Do you want to play again? [Y/N] ";
	string temp;
	cin >> temp;
	return tolower(temp[0]);	//convers input to lower case to compare easier
}

int main(int argc, const char * argv[]) {		//main
	vector<string> words;
	vector<char> guesses;		//vector of guessed letters
	vector<char> notguessed;	//vector of letters in gameword not guessed yet
	string fileName, word, guess, gameword;
	char play = 'y', letter;
	int seed = 0, randomnum, lives = 5;
	bool firstround = true;
	try{
		if (argc > 1) 
			fileName = argv[1];
		else
			throw NoFileException();		// no input file - throw exception
		ifstream file(fileName);
		if (!file)
			throw OpenFileException(fileName);		//bad input file - throw exception
		if (argc > 2) {
			istringstream ss(argv[2]);
			ss >> seed;
		}
		static std::mt19937 rng(seed);		//initialize rng
		copy(istream_iterator<string>(file), istream_iterator<string>(), back_inserter(words));		//copy input file into vector
		words.erase(remove_if(words.begin(), words.end(), nonalphalt6), words.end());		//remove all bad words not qualified
		if (words.size() == 0)		
			throw EmptyPoolException();		//no gamewords left - throw exception
		ofstream output("gamewords");		//create output file
		ostream_iterator<string> output_iterator(output, "\n");		//output vector of words to output file
		copy(words.begin(), words.end(), output_iterator);

		while (play == 'y') {			//main while loop to determine play/quit
			if (firstround) {
				lives = 5;
				guesses.clear();
				notguessed.clear();
				randomnum = (int)(rng() % (words.size()));			//determine game word
				word = words[randomnum];		
				gameword = word;
				notguessed.assign(word.begin(), word.end());		//set not guessed letters to be letters of the word
				print(guesses, notguessed, lives, gameword);		//print stats
				firstround = false;
			}
			gameword = word;
			cin >> guess;
			if (guess.length() > 1) {		//if guess an entire word
				if (guess == word) {
					cout << "You WIN!" << endl;		//win if word matches exactly
					play = playagain();
				}
				else {
					cout << "You LOSE!  The word was \"" << word << "\"." << endl;		//otherwise, lose
					play = playagain();
				}
				firstround = true;
			}
			else if (guess.length() == 1) {		//if guess a letter
				try {
					letter = guess[0];
					if (find(guesses.begin(), guesses.end(), tolower(letter)) != guesses.end())		//if guess already exists in vector of guessed letters, throw exception
						throw LetterGuessedException(letter);
					if (transformtolower(word).find(tolower(letter)) == string::npos)	//if letter cannot be found in the word (comparison all in lower case), subtract life
						lives--;
					if (lives == 0) {
						cout << "You LOSE!  The word was \"" << word << "\"." << endl;		//lose when no life left
						play = playagain();
						firstround = true;
					}
					else {
						guesses.push_back(tolower(letter));		//add valid guess letter to vector of guessed letters in lower case
						notguessed.erase(remove_if(notguessed.begin(), notguessed.end(), [letter](char guess)->bool {return tolower(letter) == tolower(guess); }), notguessed.end());	//remove the guessed letter from the not guessed vector
						if (notguessed.size() == 0) {		//if all letters of the word are guessed, player wins
							cout << "You WIN!  The word was \"" << word << "\"." << endl;
							play = playagain();
							firstround = true;
						}
						else
							print(guesses, notguessed, lives, gameword);
					}
				}
				catch (LetterGuessedException ex) {		//catches exception, prints stat and prompts for input
					cout << "You already guessed letter \"" << (char) tolower(ex.letter()) << "\"." << endl;
					print(guesses, notguessed, lives, gameword);
				}
			}
		}
	}
	catch (NoFileException ex) {		//catches exception for no input file
		cout << "Error: No input file specified." << endl;
	}
	catch (OpenFileException ex) {		//catches exception for bad input file
		cout << "Error: Could not open file \"" << ex.arg() << "\"." << endl;
	}
	catch (EmptyPoolException ex) {		//catches exception for not gamewords
		cout << "Error: Pool of game words is empty." << endl;
	}
}