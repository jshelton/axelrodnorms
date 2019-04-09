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
// const int defaultbytesPerPlayer = 2;
//const int defaultbitsPerPlayerByte = 3;
const double bitsPerPlayerByte = 3;
const double probAboveStdDev = 0.1587;
const int numberOfDefectionOpportunities = 1;
const bool metaNormsDefault = true;
bool metaNormsEnabled = false;
const double bitFlipProbability = 0.01;

// AxelRod's Algorithm
int boldnessScale = 7;
int vengenceScale = 7;
const int T = 3;
const int H = -1;
const int P = -9;
const int E = -2;

const int Pp = -9;
const int Ep = -2;

double rand1()
{
  return (1.0 * rand()) / RAND_MAX;
}

class Player
{

  int V, B;

  int S;
  // static int bitsPerPlayerByte;
  //  static int bytesPerPlayer;

  /**
 * randomBitFlip(number,probability)
 * This function will flip assumes bitsPerPlayer number of bits (usually 3)
 * It will randomly flip one of the first bitsPerPlayer bits if the random 
 * generated double is below the input probability.
 */
  int randomBitFlip(int &number, double probability)
  {

    for (int i = 0; i < bitsPerPlayerByte; i++)
    {
      if (rand1() < probability)
      {
        // lucky bit
        number = number ^ (1 << i);
      }
    }
    return number;
  }

public:
  // Not used anymore
  //void setBitsAssumptions(int bytspb = defaultbytesPerPlayer, int bitspp = defaultbitsPerPlayerByte)
  // void setBitsAssumptions(int bitspp = defaultbitsPerPlayerByte)
  // {
  //   // bytesPerPlayer = bytspb;
  //   bitsPerPlayerByte = bitspp;
  // }

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

  void BitFlip(double probability)
  {
    this->V = randomBitFlip(this->V, probability);
    this->B = randomBitFlip(this->B, probability);
  }
};

bool scoreCompare(const Player &p1, const Player &p2)
{
  return p1.score() > p2.score();
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
      playerList.push_back(Player(rand() % boldnessScale, rand() % vengenceScale));
    }
  }

  void calculateScores()
  {
    bool debug = false;
    for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
    {

      if (debug)
      {
        cerr << endl
             << "-Now Player: " << (*it) << " ";
      }

      for (int defectionOpportunity = 0; defectionOpportunity < numberOfDefectionOpportunities; defectionOpportunity++)
      {
        // probability of being seen
        double S = rand1();

        if (debug)
        {
          cerr << "Defection opportunity: " << defectionOpportunity << ", S = " << S << ". Brave enough (" << S * boldnessScale << "/" << it->boldness() << ")? " << endl;
        }

        // If he is brave enough
        if (S * boldnessScale < it->boldness())
        {
          if (debug)
          {
            cerr << "Yes Brave enough. ";
          }
          // Increase the player's payoff
          it->score() += T;

          // Now to see if anyone caught this person
          for (list<Player>::iterator jt = playerList.begin(); jt != playerList.end(); jt++)
          {
            // skip over the subject
            if (it == jt)
              continue;

            // Every other agent was hurt because of this defection
            jt->score() += H;

            double probabilityWasSeen = rand1();

            if (debug)
              cerr << endl
                   << "Did " << (*jt) << " see him (" << probabilityWasSeen << "/" << S << ")? ";

            // if this defection falls in the probability of being seen
            if (probabilityWasSeen < S)
            {
              if (debug)
                cerr << "Yes, he was seen by " << (*jt) << ". ";

              // Agent j saw agent i
              // Now let's see what he does about it

              // is he going to be vengeful
              double probabilityWasVengeful = rand1() * vengenceScale;
              if (debug)
                cerr << "Did " << (*jt) << " punish him (" << probabilityWasVengeful << "/" << jt->vengance() << ")?";

              if (probabilityWasVengeful < jt->vengance())
              {

                jt->score() += E;
                it->score() += P;

                if (debug)
                  cerr << "Yes " << (*jt) << " punished " << (*it) << ".";
              }
              else
              {
                if (debug)
                  cerr << " He was not vengeful.";

                // Someone has seen the culprit but not punished
                if (metaNormsEnabled)
                {
                  // kt is the punisher of jt when he does not punish it
                  for (list<Player>::iterator kt = playerList.begin(); kt != playerList.end(); kt++)
                  {
                    // skip over the subject and the observer who did nothing
                    if (kt == jt || kt == it)
                      continue;

                    // No other agent was hurt by this
                    // jt->score() += H;

                    double probabilityWasSeen = rand1();

                    if (debug)
                      cerr << endl
                           << " Did " << (*kt) << " see " << (*jt) << " walk away (" << probabilityWasSeen << "/" << S << ")? ";

                    // if this defection falls in the probability of being seen
                    if (probabilityWasSeen < S)
                    {
                      if (debug)
                        cerr << " Yes, he was seen.";

                      // Agent j saw agent i
                      // Now let's see what he does about it

                      // is he going to be vengeful
                      double probabilityWasVengeful = rand1() * vengenceScale;
                      if (debug)
                        cerr << "Did " << (*kt) << " punish him for walking away (" << probabilityWasVengeful << "/" << kt->vengance() << ")?";

                      if (probabilityWasVengeful < kt->vengance())
                      {

                        kt->score() += Ep;
                        jt->score() += Pp;

                        if (debug)
                          cerr << " Yes " << (*kt) << " punished " << (*jt) << ".";
                      } // if was vengeful
                      else if (debug)
                        cerr << " No not vengeful.";
                    } // if was seen
                    else if (debug)
                      cerr << " No not seen not punishing.";
                  } // for all other metapunishers
                }   // if Metanorms working
                else if (debug)
                  cerr << " No metanorms.";

              } // not punished first time

            } // Seen defecting
            else if (debug)
              cerr << " No not seen defecting.";
          } // For each other agent possibly observing
        }   // If Boldness is high enough
        else if (debug)
          cerr << " No too brave.";
      } // For each defecting opportunity
    }   // For each player
  }     // Calculate Scores

  Player &
  playerAt(int indexFrom)
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
  // double averageScore()
  // {

  //   double scoreTotal = 0;

  //   for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
  //   {
  //     scoreTotal += it->score();
  //   }

  //   double average = scoreTotal / playerList.size();

  //   return average;
  // }
  /* 
* stdDev()
* Returns the standard deviation
*/
  // double stdDevScore()
  // {

  //   double varianceTotal = 0;

  //   double average = averageScore();

  //   for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
  //   {
  //     varianceTotal += squared(it->score() - average);
  //   }

  //   double stdDev = sqrt(varianceTotal) / playerList.size();
  //   return stdDev;
  // }

  /*
* Procreate
* This function will go through all the items and will generate 2 clones of individuals with scores
* One standard deviation higher, as well as clones for all except the lowest generation. 
*/
  void procreate()
  {
    bool debug = false;

    // In theory Here we procreate by:
    // Add two of the ones higher than one standard deviation
    // Add one for those who are above one standard deviation
    // Ignore less than one standard deviation
    // Until we reach the size of our max Population and then we stop

    // In practice it is easier to remove a low one and add two high ones
    // until we get to a standard deviation. Essentially the top 15.87%
    // replaces the bottom 15.87%.

    // Just make sure our assumptions are correct
    if (populationSize != playerList.size())
      cerr << "ERROR: Population sizes do not match up" << endl;

    int numberOfTopEschelon = populationSize * probAboveStdDev;
    int countProgeny = 0;
    list<Player> newList;

    playerList.sort(scoreCompare);
    if (debug)
    {
      cerr << "-playerList ";
      for (list<Player>::iterator it = playerList.begin(); it != playerList.end(); it++)
      {
        cerr << "," << *it;
      }

      cerr << endl;
    }

    // First Start with top eschelon
    for (int i = 0; i < numberOfTopEschelon; i++)
    {

      // Just checking Assumptions
      if (playerList.begin() == playerList.end())
      {
        cerr << "ERROR: top eschelon miscalculated" << endl;
        break;
      }

      // Top Eschelon gets kids
      newList.push_back(Player(*playerList.begin()));
      newList.push_back(Player(*playerList.begin()));

      // This will replace the spot of one lower eschelon and one upper eschelon
      playerList.pop_front();
      playerList.pop_back();
    }

    // Checking Assumptions
    if (playerList.size() != populationSize - 2 * numberOfTopEschelon)
      cerr << "ERROR: The math behind normal population size is mistaken" << endl;

    // Now the remaining Should be
    newList.insert(newList.end(), playerList.begin(), playerList.end());

    // // Not going to do it this way anymore
    // if (false)
    // {

    //   double stdDev = stdDevScore();
    //   double average = averageScore();
    //   if (debug)
    //     cerr << "-std dev: " << stdDev << " Average: " << average;

    //   for (list<Player>::iterator it = playerList.begin(); it != playerList.end() && countProgeny < populationSize; it++)
    //   {

    //     // Any below one standard deviation from mean
    //     if (it->score() > average - stdDev - nudge)
    //     {
    //       if (++countProgeny > populationSize)
    //         break;

    //       newList.push_back(Player(*it));

    //       // if we are talking about above one standard deviation from mean
    //       if (it->score() > stdDev + average + nudge)
    //       {
    //         if (++countProgeny > populationSize)
    //           break;

    //         newList.push_back(Player(*it));
    //       }
    //     }
    //   }
    // }

    if (debug)
    {
      cerr << "-newList ";
      for (list<Player>::iterator it = newList.begin(); it != newList.end(); it++)
      {
        cerr << "," << *it;
      }
      cerr << endl;
    }

    playerList.clear();

    playerList.assign(newList.begin(), newList.end());

    for (list<Player>::iterator it = playerList.begin(); it != playerList.end() && countProgeny < populationSize; it++)
    {
      it->BitFlip(bitFlipProbability);
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
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
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
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
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
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
    }
    return success;
  }

  static bool test3(bool debug)
  {
    string testName = "Test reproduction";
    stringstream ss;
    string expected("(0,3,-40)(2,1,-33)(1,4,-15)(4,5,-23)(1,5,-10)\n(1,5,-10)(1,4,-15)(4,5,-23)(2,1,-33)(0,3,-40)");

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
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
    }
    return success;
  }

  static bool test4(bool debug)
  {
    string testName = "Test RandomBitSwitch";
    stringstream ss;
    string expected("(6,4,-12)(0,2,-32)(3,3,-25)(1,1,-79)(6,4,-77)(3,5,-92)(3,0,-81)(5,4,-76)(6,3,-53)(6,6,-87)(1,4,-58)(3,1,-71)(4,1,-60)(1,5,-73)(4,6,-75)(4,6,-6)(4,0,-7)(1,0,-47)(4,3,-81)(6,6,-22)\n(4,6,-6)(4,6,-6)(4,0,-7)(4,1,-7)(6,4,-12)(6,4,-12)(6,6,-22)(3,3,-25)(0,2,-32)(1,0,-47)(6,3,-53)(1,4,-58)(4,1,-60)(3,1,-71)(1,5,-73)(4,6,-75)(5,4,-76)(6,4,-77)(1,1,-79)(3,0,-81)");

    {
      PlaySet pl;

      pl.setSeed(232321);

      int popSize = 20;
      pl.setPopulationSize(popSize);
      pl.generateList();

      for (int i = 0; i < popSize; i++)
        pl.playerAt(i).score() = (rand() % 100) - 95;

      ss << pl << endl;

      pl.procreate();

      ss << pl;
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
    }
    return success;
  }

  static bool test5(bool debug)
  {
    string testName = "Test Score Calculation";
    stringstream ss;
    string expected("(6,6,0)(0,3,0)(4,0,0)(1,6,0)(5,2,0)(6,1,0)(2,2,0)(0,6,0)(0,1,0)(0,3,0)(3,0,0)(1,6,0)(5,6,0)(5,2,0)(2,5,0)(0,3,0)(0,0,0)(0,1,0)(1,3,0)(5,6,0)\n(6,6,-62)(0,3,-10)(4,0,-16)(1,6,-10)(5,2,-16)(6,1,-12)(2,2,-26)(0,6,-6)(0,1,-6)(0,3,-10)(3,0,-14)(1,6,-26)(5,6,-19)(5,2,-14)(2,5,-44)(0,3,-10)(0,0,-10)(0,1,-10)(1,3,-24)(5,6,-24)");

    {
      PlaySet pl;

      pl.setSeed(195105);

      int popSize = 20;
      pl.setPopulationSize(popSize);
      pl.generateList();

      ss << pl << endl;

      metaNormsEnabled = false;
      pl.calculateScores();
      metaNormsEnabled = metaNormsDefault;

      ss << pl;
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
    }
    return success;
  }

  static bool test6(bool debug)
  {
    string testName = "Test Score Calculation";
    stringstream ss;
    string expected("(6,6,0)(0,3,0)(4,0,0)(1,6,0)(5,2,0)(6,1,0)(2,2,0)(0,6,0)(0,1,0)(0,3,0)(3,0,0)(1,6,0)(5,6,0)(5,2,0)(2,5,0)(0,3,0)(0,0,0)(0,1,0)(1,3,0)(5,6,0)\n(6,6,-57)(0,3,-112)(4,0,-57)(1,6,-54)(5,2,-22)(6,1,-45)(2,2,-93)(0,6,-58)(0,1,-49)(0,3,-17)(3,0,-42)(1,6,-71)(5,6,-70)(5,2,-135)(2,5,-84)(0,3,-76)(0,0,-26)(0,1,-35)(1,3,-102)(5,6,-34)");

    {
      PlaySet pl;

      pl.setSeed(195105);

      int popSize = 20;
      pl.setPopulationSize(popSize);
      pl.generateList();

      ss << pl << endl;
      metaNormsEnabled = true;
      pl.calculateScores();
      metaNormsEnabled = metaNormsDefault;
      ss << pl;
    }

    // See if the expectation value is accurate
    bool success = (expected.compare(ss.str()) == 0);
    if (debug)
    {
      cerr << "Test Title: " << testName << "" << endl;
      cerr << "Expected: " << endl
           << expected << endl;
      cerr << "Actual: " << endl
           << ss.str() << endl;
    }
    return success;
  }

  static bool runAll()
  {
    bool debug = true;

    vector<bool> pass;

    runTest(pass, test0, debug); //0
    runTest(pass, test1, debug); //1
    runTest(pass, test2, debug); //2
    runTest(pass, test3, debug); //3
    runTest(pass, test4, debug); //4
    runTest(pass, test5, debug); //5
    runTest(pass, test6, debug); //6

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