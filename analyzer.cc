/*
   This is a code for Sentiment analyzer based on the tutorial 
   at http://karpathy.ca/mlsite/lecture2.php
 
   Copyright (C) 2012  gauravagrawal
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<stdexcept>
#include<sstream>
#include<map>

class SA {

    private:
	enum sentimentType { NEGATIVE = -1, POSITIVE = 1 } ;

	typedef std::pair<std::string, sentimentType> reviewSentiment;

	std::vector<reviewSentiment> m_trainingData;
	std::vector<reviewSentiment> m_testData;

	std::map<std::string, int> m_positiveWords;
	std::map<std::string, int> m_negativeWords;

    public:
	SA(std::string positiveTraining, std::string negativeTraining) {
	    this->addTrainingData(positiveTraining, POSITIVE);
	    this->addTrainingData(negativeTraining, NEGATIVE);
	}

	void analyze(std::string positiveTest, std::string negativeTest) {
	    this->addTestData(positiveTest, POSITIVE);
	    this->addTestData(negativeTest, NEGATIVE);
	    this->populateWords();
	    this->performAnalysis();
	}

    private:
	void addTrainingData(const std::string filename, sentimentType stype) {
	    this->addData(filename, m_trainingData, stype);
	}

	void addTestData(const std::string filename, sentimentType stype) {
	    this->addData(filename, m_testData, stype);
	}

	void addData(const std::string filename, std::vector<reviewSentiment>& tobeAddedIn, sentimentType stype) {
	    if (filename.length() < 1)
		throw std::invalid_argument("invalid filename");

	    std::ifstream file(filename.c_str());

	    if (file.is_open()) {
		std::string line;

		while (file.good()) {
		    std::getline(file, line);
		    tobeAddedIn.push_back(reviewSentiment(line, stype)) ;
		}
		file.close();
	    }
	    else 
		throw std::runtime_error("cannot open file");
	}

	std::vector<std::string> splitWords(const std::string line) {
	    std::vector<std::string> elems;
	    std::stringstream ss(line);
	    std::string item;

	    while (std::getline(ss, item, ' ')) {
		elems.push_back(item);
	    }
	    return elems;
	}

	void splitAndAddWord(const std::string line, std::map<std::string, int>& container) {
	    std::vector<std::string> splittedElems = splitWords(line);
	    std::vector<std::string> :: iterator iter = splittedElems.begin();
	    while (iter != splittedElems.end()) {
		container[*iter]++;
		iter++;
	    }
	}

	void populatePositiveWords(const std::string line) {
	    this->splitAndAddWord(line, m_positiveWords);
	}

	void populateNegativeWords(const std::string line) {
	    this->splitAndAddWord(line, m_negativeWords);
	}

	void populateWords() {
	    std::vector<reviewSentiment> :: iterator iter = m_trainingData.begin();

	    while (iter != m_trainingData.end()) {
		if (iter->second == POSITIVE)
		    this->populatePositiveWords(iter->first);
		else
		    this->populateNegativeWords(iter->first);
		iter++;
	    }
	}

	void performAnalysis() {

	    std::vector<reviewSentiment> :: iterator iter = m_testData.begin();
	    int wrongAnswers = 0;

	    while (iter != m_testData.end()) {
		std::vector<std::string> splittedWords = splitWords(iter->first);
		std::vector<std::string> :: iterator wordsIter = splittedWords.begin();

		double percentPositive = 0.0;
		double percentNegative = 0.0;

		while (wordsIter != splittedWords.end()) {
		    int positiveCount = m_positiveWords[*wordsIter] + 1.0;
		    int negativeCount = m_negativeWords[*wordsIter] + 1.0;

		    percentPositive += positiveCount / (positiveCount + negativeCount);
		    percentNegative += negativeCount / (positiveCount + negativeCount);

		    wordsIter++;
		}

		sentimentType sentiment = POSITIVE;
		if (percentNegative > percentPositive)
		    sentiment = NEGATIVE;

		if (iter->second != sentiment) {
		    std::cout<<iter->first<<std::endl;
		    wrongAnswers++;
		}

		iter++;
	    }

	    std::cout<<"************* TOTAL ******************* "<<wrongAnswers<<std::endl;
	    std::cout<<"Percentage is "<<wrongAnswers * 1.0/(m_testData.size())<<std::endl;
	}
} ;

int main(int argc, char **argv) {

    SA analyzer("training.pos", "training.neg");
    analyzer.analyze("test.pos", "test.neg");

    return 0;
}
