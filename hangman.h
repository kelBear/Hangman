#ifndef HANGMAN_H
#define HANGMAN_H

#include <string>

class NoFileException {		//exception class for no input file
public:
	NoFileException();
};

class OpenFileException {		//exception class for bad input file
public:
	OpenFileException(std::string arg);
	std::string arg();
private:
	std::string arg_;
};

class EmptyPoolException {		//exception class for no gamewords in pool
public:
	EmptyPoolException();
};

class LetterGuessedException {		//exception class for already guessed letter
public:
	LetterGuessedException(char);
	char letter() const;
private:
	char letter_;
};

#endif