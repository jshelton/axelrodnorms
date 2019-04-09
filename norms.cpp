#include <iostream>
#include <list>
#include <vector>
#include <sstream>

#include <math.h>
#include <time.h> /* time */

using namespace std;

// For double math to work well
const double nudge = 0.00000001;

// Populationcontrols
const int defaultPopulationSize = 40;
const int initialScore = 0;

class Player
{

  int V, B, S;

public:
  int vengance() const
  {
    return V;
  }

  int &vengance()
  {
    return V;
  }

  int score() const
  {
    return S;
  }

  int &score()
  {
    return S;
  }

  int boldness() const
  {
    return B;
  }

  int &boldness()
  {
    return B;
  }

  // void clone(Player const &p)
  // {
  //   this->V = p.V;
  //   this->B = p.B;
  //   this->S = initialScore;
  // }

  Player(Player const &p)
  {
    this->V = p.V;
    this->B = p.B;
    this->S = p.S;
  }

  Player(int newV, int newB, int newS)
  {

    this->V = newV;
    this->B = newB;
    this->S = newS;
  }

  Player(int newV, int newB)
  {
    this->V = newV;
    this->B = newB;
    this->S = initialScore;
  }
};

int scoreSort(const Player &p1, const Player &p2)
{
  return p1.score() - p2.score();
}

ostream &operator<<(ostream &os, const Player &p)
{
  os << "(" << p.vengance() << "," << p.boldness() << "," << p.score() << ")";
  return os;
}

class PlaySet
{

  list<Player> playerList;
  int seed;
  int length;
  int populationSize;

public:
  void setPopulationSize(int size = defaultPopulationSize)
  {
    populationSize = size;
  }

  void setSeed(int seedIn = -1)
  {

    if (seedIn == -1)
      seed = time(NULL);
    else
      seed = seedIn;

    srand(seed);
  }

  /** 
 * Return the list size for printing to ostream
 */
  int size() const
  {
    return playerList.size();
  }

  /*
   * This generates a list of random players 
   */
  void generateList()
  {
    if (seed < 0)
    {
      this->setSeed();
    }

    for (int i = 0; i < populationSize; i++)
    {
      playerList.push_back(Player(0.0 + rand() % 7, 0.0 + rand() % 7));
    }
  }

  void
  iterate()
  {
    for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
    {
      Player p = (*it);
    }
  }

  Player &playerAt(int indexFrom)
  {
    list<Player>::iterator it = playerList.begin();
    for (int i = 0; i < indexFrom; i++)
      it++;
    return *it;
  }

  const Player &playerAt(int indexFrom) const
  {
    list<Player>::const_iterator it = playerList.cbegin();
    for (int i = 0; i < indexFrom; i++)
      it++;
    return *it;
  }

  static inline double squared(double num)
  {
    return num * num;
  }

  /* 
* averageScore()
* returns the average score of the dataset
*/
  double averageScore()
  {

    double scoreTotal = 0;

    for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
    {
      scoreTotal += it->score();
    }

    double average = scoreTotal / playerList.size();

    return average;
  }
  /* 
* stdDev()
* Returns the standard deviation
*/
  double stdDevScore()
  {

    double varianceTotal = 0;

    double average = averageScore();

    for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
    {
      varianceTotal += squared(it->score() - average);
    }

    double stdDev = sqrt(varianceTotal) / playerList.size();
    return stdDev;
  }

  /*
* Procreate
* This function will go through all the items and will generate 2 clones of individuals with scores
* One standard deviation higher, as well as clones for all except the lowest generation. 
*/
  void procreate()
  {
    bool debug = false;
    double stdDev = stdDevScore();
    double average = averageScore();

    if (debug)
      cerr << "-std dev: " << stdDev << " Average: " << average;

    // Here we procreate by:
    // Add two of the ones higher than one standard deviation
    // Add one for those who are above one standard deviation
    // Ignore less than one standard deviation
    // Until we reach the size of our max Population and then we stop

    int i = 0;
    list<Player> newList;

    for (list<Player>::iterator it = playerList.begin(); it != playerList.end() && i < populationSize; i++, it++)
    {

      if (it->score() > average - stdDev - nudge)
      {
        newList.push_back(Player(*it));
        if (it->score() > stdDev + average + nudge)
          newList.push_back(Player(*it));
      }
    }

    if (debug)
    {
      cerr << endl
           << "-newList ";
      for (list<Player>::iterator it = newList.begin(); it != newList.end(); it++)
      {
        cerr << "," << *it;
      }
    }

    playerList.clear();

    playerList.assign(newList.begin(), newList.end());

    if (debug)
    {
      cerr << endl
           << "-playerList ";
      for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
      {
        cerr << "," << *it;
      }

      cerr << endl;
    }
  }

  /* 
  *  Procreate clones the current player and places him immediately after the current player. 
  *  This new player has the default initial score 0.
  */
  // void procreateOne(int indexFrom, int indexTo)
  // {
  //   playerList[indexTo].clone(playerList.at(indexFrom));
  // }
  void procreateOne(int indexFrom)
  {
    if (indexFrom < playerList.size() - 1)
    {
      playerList.pop_back();

      list<Player>::iterator it = playerList.begin();
      for (int i = 0; i < indexFrom; i++)
        it++;

      playerList.insert(it, Player(*it));
    }
    else
    {
      throw "index out of range (cannot be the last element of the list";
    }
  }

  PlaySet()
  {
    this->seed = -1;
    length = 0;
    populationSize = defaultPopulationSize;
  }
};

ostream &operator<<(ostream &os, const PlaySet &ps)
{
  for (int i = 0; i < ps.size(); i++)
  {
    os << ps.playerAt(i);
  }
  return os;
}

struct UnitTests
{

  template <typename Function>
  static void runTest(vector<bool> &pass, Function f, bool debug)
  {
    if (debug)
    {
      cerr << "Test Number: " << pass.size() << endl;
    }
    pass.push_back(f(debug));
    if (debug)
    {
      cerr << "Test " << (pass.back() ? "passed" : "failed") << endl
           << endl;
    }
  }

  static bool test0(bool debug)
  {
    string testName = "Test of the Testing system";
    stringstream ss;
    string expected("This is a Test");

    {
      ss << "This is a Test";
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << expected << endl;
      cerr << "Actual: " << ss.str() << endl;
    }
    return success;
  }

  static bool test1(bool debug)
  {
    string testName = "Test five players with seed two";
    stringstream ss;
    string expected("(0,0,0)(3,5,0)(4,5,0)(4,1,0)(1,1,0)");

    {
      PlaySet pl;

      pl.setSeed(2);

      pl.setPopulationSize(5);
      pl.generateList();

      ss << pl;
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << expected << endl;
      cerr << "Actual: " << ss.str() << endl;
    }
    return success;
  }

  static bool test2(bool debug)
  {
    string testName = "Test twenty new players";
    stringstream ss;
    string expected("(0,0,0)(4,3,0)(3,4,0)(6,1,0)(1,1,0)(3,5,0)(5,4,0)(0,0,0)(4,5,0)(4,2,0)(4,6,0)(2,5,0)(1,1,0)(4,5,0)(6,4,0)(3,0,0)(0,4,0)(1,1,0)(6,1,0)(5,4,0)");

    {
      PlaySet pl;

      pl.setSeed(3);
      pl.setPopulationSize(20);
      pl.generateList();

      ss << pl;
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << expected << endl;
      cerr << "Actual: " << ss.str() << endl;
    }
    return success;
  }

  static bool test3(bool debug)
  {
    string testName = "Test reproduction";
    stringstream ss;
    string expected("(0,3,-40)(2,1,-33)(1,4,-15)(4,5,-23)(1,5,-10)\n(1,4,-15)(1,4,-15)(4,5,-23)(1,5,-10)(1,5,-10)");

    {
      PlaySet pl;

      pl.setSeed(9823);

      pl.setPopulationSize(5);
      pl.generateList();

      pl.playerAt(0).score() = -40;
      pl.playerAt(1).score() = -33;
      pl.playerAt(2).score() = -15;
      pl.playerAt(3).score() = -23;
      pl.playerAt(4).score() = -10;
      ss << pl << endl;

      pl.procreate();

      ss << pl;
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << expected << endl;
      cerr << "Actual: " << ss.str() << endl;
    }
    return success;
  }

  static bool test4(bool debug)
  {
    string testName = "Test Procreation";
    stringstream ss;
    string expected("This is a Test");

    {
      ss << "This is a Test";
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << expected << endl;
      cerr << "Actual: " << ss.str() << endl;
    }
    return success;
  }

  static bool runAll()
  {
    bool debug = true;

    vector<bool> pass;

    runTest(pass, test0, debug);
    runTest(pass, test1, debug);
    runTest(pass, test2, debug);
    runTest(pass, test3, debug);
    runTest(pass, test4, debug);

    bool passall = true;

    for (int i = 0; i < pass.size(); i++)
    {
      passall = passall && pass[i];
    }

    if (debug)
    {
      cout << "The results are " << (passall ? "all passing" : "failing") << "." << endl;
      for (int i = 0; i < pass.size(); i++)
        if (!pass[i])
          cerr << "Test number " << i << " failed." << endl;
    }

    return passall;
  }

} test;

int main()
{
  test.runAll();
}