#include <iostream>
#include <list>
#include <vector>
#include <sstream>

#include <time.h> /* time */

using namespace std;

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

  void clone(Player const &p)
  {
    const int initialScore = 0;
    this->V = p.V;
    this->B = p.B;
    this->S = initialScore;
  }

  Player(Player const &p)
  {
    clone(p);
  }

  Player(int newV, int newB)
  {

    const int initialScore = 2;
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

public:
  void setSeed(int seedIn = -1)
  {

    if (seedIn == -1)
      seed = time(NULL);
    else
      seed = seedIn;

    srand(seed);
  }

  int size() const
  {
    return playerList.size();
  }

  /*
   * This generates a list of random players 
   */
  void generateList(int listLength)
  {

    if (seed < 0)
    {
      this->setSeed();
    }

    for (int i = 0; i < listLength; i++)
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
    //    this->playerList = new list<Player>();
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

      pl.generateList(5);

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

      pl.generateList(20);

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
    string testName = "Test cloning/progeny";
    stringstream ss;
    string expected("(0,0,0)(4,3,0)(3,4,0)(6,1,0)(6,1,0)(1,1,0)(3,5,0)(5,4,0)(0,0,0)(4,5,0)(4,2,0)(4,6,0)(2,5,0)(1,1,0)(4,5,0)(6,4,0)(3,0,0)(0,4,0)(1,1,0)(6,1,0)");

    {
      PlaySet pl;

      pl.setSeed(3);

      pl.generateList(20);

      try
      {
        pl.procreateOne(3);
      }
      catch (const char *msg)
      {
        cerr << msg << endl;
      }

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
    string testName = "Test sort by score";
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