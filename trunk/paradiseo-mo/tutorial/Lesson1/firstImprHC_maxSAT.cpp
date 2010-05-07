//-----------------------------------------------------------------------------
/** firstImprHC_maxSAT.cpp
 *
 * SV - 05/05/10 
 *
 */
//-----------------------------------------------------------------------------

// standard includes
#define HAVE_SSTREAM

#include <stdexcept>  // runtime_error 
#include <iostream>   // cout
#include <sstream>    // ostrstream, istrstream
#include <fstream>
#include <string.h>

// the general include for eo
#include <eo>

// declaration of the namespace
using namespace std;

//-----------------------------------------------------------------------------
// representation of solutions, and neighbors
#include <ga/eoBit.h>                         // bit string : see also EO tutorial lesson 1: FirstBitGA.cpp
#include <problems/bitString/moBitNeighbor.h> // neighbor of bit string

//-----------------------------------------------------------------------------
// fitness function, and evaluation of neighbors
#include <eval/maxSATeval.h>
#include <problems/eval/moMaxSATincrEval.h>
#include <eval/moFullEvalByModif.h>

//-----------------------------------------------------------------------------
// neighborhood description
#include <neighborhood/moRndWithoutReplNeighborhood.h> // visit all neighbors in random order without repeating any neighbor

//-----------------------------------------------------------------------------
// the first improvement Hill-Climbing local search
#include <algo/moFirstImprHC.h>

// Declaration of types
//-----------------------------------------------------------------------------
// Indi is the typedef of the solution type like in paradisEO-eo
typedef eoBit<unsigned int> Indi;                      // bit string with unsigned fitness type
// Neighbor is the typedef of the neighbor type,
// Neighbor = How to compute the neighbor from the solution + information on it (i.e. fitness)  
// all classes from paradisEO-mo use this template type
typedef moBitNeighbor<unsigned int> Neighbor ;         // bit string neighbor with unsigned fitness type


// Main function
//-----------------------------------------------------------------------------
void main_function(int argc, char **argv)
{
  /* =========================================================
   *
   * Parameters from parser
   *
   * ========================================================= */
  // more information on the input parameters: see EO tutorial lesson 3
  // but don't care at first it just read the parameters of the bit string size and the random seed.

  // First define a parser from the command-line arguments
  eoParser parser(argc, argv);

  // For each parameter, define Parameter, read it through the parser,
  // and assign the value to the variable

  // random seed parameter
  eoValueParam<uint32_t> seedParam(time(0), "seed", "Random number seed", 'S');
  parser.processParam( seedParam );
  unsigned seed = seedParam.value();

  // length of the bit string
  eoValueParam<unsigned int> vecSizeParam(20, "vecSize", "Genotype size", 'V');
  parser.processParam( vecSizeParam, "Representation" );
  unsigned vecSize = vecSizeParam.value();

  // Number of clauses of the max SAT problem
  eoValueParam<unsigned int> ncParam(10, "nbClauses", "Number of clauses", 'm');
  parser.processParam( ncParam, "Representation" );
  unsigned nbClause = ncParam.value();

  // Number of litteral by clauses
  eoValueParam<unsigned int> kParam(3, "nbLitt", "Number of litteral by clauses", 'k');
  parser.processParam( kParam, "Representation" );
  unsigned nbLitteral = kParam.value();

  // the name of the instance file
  string str_in = "" ; // default value
  eoValueParam<string> inParam(str_in.c_str(), "in", "Input file of the file in ncf format", 'f');
  parser.processParam(inParam, "Persistence" );
  str_in = inParam.value();

  // the name of the "status" file where all actual parameter values will be saved
  string str_status = parser.ProgramName() + ".status"; // default value
  eoValueParam<string> statusParam(str_status.c_str(), "status", "Status file");
  parser.processParam( statusParam, "Persistence" );
  
  // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
  // i.e. in case you need parameters somewhere else, postpone these
  if (parser.userNeedsHelp()) {
    parser.printHelp(cout);
    exit(1);
  }
  if (statusParam.value() != "") {
    ofstream os(statusParam.value().c_str());
    os << parser;// and you can use that file as parameter file
  }

  /* =========================================================
   *
   * Random seed
   *
   * ========================================================= */

  // reproducible random seed: if you don't change SEED above,
  // you'll aways get the same result, NOT a random run
  // more information: see EO tutorial lesson 1 (FirstBitGA.cpp)
  rng.reseed(seed);

  /* =========================================================
   *
   * Eval fitness function (full evaluation)
   *
   * ========================================================= */

  // the max SAT evaluation
  MaxSATeval<Indi> * fullEval;

  if (str_in.compare("") == 0)
    fullEval = new MaxSATeval<Indi>(vecSize, nbClause, nbLitteral);
  else {
    fullEval = new MaxSATeval<Indi>(str_in);
    vecSize = fullEval->nbVar ;
  }

  //  string out = "cnf.dat";
  //  fullEval->save(out);

  /* =========================================================
   *
   * evaluation of a neighbor solution
   *
   * ========================================================= */

  // Use it if there is no incremental evaluation: a neighbor is evaluated by the full evaluation of a solution
  //  moFullEvalByModif<Neighbor> neighborEval(*fullEval);

  // Incremental evaluation of the neighbor:
  moMaxSATincrEval<Neighbor> neighborEval(*fullEval);

  /* =========================================================
   *
   * Initialization of the solution
   *
   * ========================================================= */

  // a Indi random initializer: each bit is random
  // more information: see EO tutorial lesson 1 (FirstBitGA.cpp)
  eoUniformGenerator<bool> uGen;
  eoInitFixedLength<Indi> random(vecSize, uGen);

  /* =========================================================
   *
   * the neighborhood of a solution
   *
   * ========================================================= */

  // Exploration of the neighborhood in random order of the neigbor's index:
  // each neighbor is visited only once
  moRndWithoutReplNeighborhood<Neighbor> neighborhood(vecSize);

  /* =========================================================
   *
   * the local search algorithm
   *
   * ========================================================= */

  moFirstImprHC<Neighbor> hc(neighborhood, *fullEval, neighborEval);

  /* =========================================================
   *
   * executes the local search from a random solution
   *
   * ========================================================= */

  // The current solution
  Indi solution;

  // Apply random initialization
  random(solution);

  // Evaluation of the initial solution:
  // can be evaluated here, or else it will be done at the beginning of the local search
  (*fullEval)(solution);

  // Output: the intial solution 
  std::cout << "initial: " << solution << std::endl ;

  // Apply the local search on the solution !
  hc(solution);

  // Output: the final solution 
  std::cout << "final:   " << solution << std::endl ;

}

// A main that catches the exceptions

int main(int argc, char **argv)
{
  try {
    main_function(argc, argv);
  }
  catch (exception& e) {
    cout << "Exception: " << e.what() << '\n';
  }
  return 1;
}
