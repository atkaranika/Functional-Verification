// athanasia karanika                                                               //
// given a file via arguments with boolean clauses in the following form            //
// x1 + x4                                                                          //
// x1 + x3bar + x8bar                                                              //
// ...                                                                              //
// where the literal with the bar is the complement of the literal without the bar  // 
// implements the conflict driven clause learning to satisfy the boolean expression //
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <utility>

// Define a struct to hold literal name and implication graph level //
struct TrailEntry {
    std::string literal;
    int level;

    TrailEntry(const std::string& lit, int lvl) : literal(lit), level(lvl) {}
};

class Literal {
  private:
    std::string name;    // Literal name (e.g., "x1", "x2", etc.)
    bool negated;        // Negation status (true if negated, false otherwise)
    int variableIndex;   // Index of the variable associated with this literal
  public:
    // Constructor
    Literal(const std::string& name, bool negated, int variableIndex)
        : name(name), negated(negated), variableIndex(variableIndex) {}

    Literal() : name(""), negated(false), variableIndex(0) {}

    // Equality comparison operator
    bool operator==(const Literal& other) const {
        return (name == other.name && negated == other.negated && variableIndex == other.variableIndex);
    }

    void setVariableIndex(int varIndex){
      variableIndex = varIndex;
    }

    int getVariableIndex(){
      return variableIndex;
    }

    std::string getName() const {
      return name;
    }
    
};

class Variable {
  private:
      std::string variable;
      std::vector<int> clauses; // clauses in which this variable exists
      bool assignment;
  public:
      // Constructor to initialize variable
      Variable(const std::string& var) : variable(var) {}

      // Method to add a clause
      void addClause(int clauseIndex) {
          clauses.push_back(clauseIndex);
      }

      std::vector<int> getClausesIndex() {
          return clauses;
      }

      std::string getVariable() {
          return variable;
      }

      bool getAssignment() {
          return assignment;
      }
};

class Clause {
  private:
      std::vector<Literal> literals; // Pair representing literal and its negation status
      // point to watched variable in each clause //
      Literal pointer1; 
      Literal pointer2;
      bool assignment; // 1 if clause is satisfied, 0 otherwise //

  public:
      Clause(std::string& clauseString) {
          size_t pos = 0;
          std::string token;
          std::string clauseStringCopy = clauseString;
          while ((pos = clauseStringCopy.find("+")) != std::string::npos) {
              token = clauseStringCopy.substr(0, pos);
              if (token.back() == 'r' && token[token.length() - 2] == 'a' && token[token.length() - 3] == 'b') {
                literals.push_back(Literal(token, false, 0));
              } else {
                literals.push_back(Literal(token, true, 0));
              }
              clauseStringCopy.erase(0, pos + 1);      
          }
          token = clauseStringCopy;

          if (token.back() == 'r' && token[token.length() - 2] == 'a' && token[token.length() - 3] == 'b') {
            literals.push_back(Literal(token, false, 0));
          } else {
            literals.push_back(Literal(token, true, 0));
          }
          pointer1 = literals[0];
          pointer2 = literals[literals.size() - 1];
      }

      void setAssignment(bool assign){
          assignment = assign;
      }

      bool getAssignment(){
          return assignment;
      }
      std::vector<Literal>& getLiterals() {
        return literals;
      }

      Literal& getPointer1() {
        return pointer1;
      }

      Literal& getPointer2() {
        return pointer2;
      }

      void setPointer1(Literal& p1) {
        pointer1 = p1;
      }

      void setPointer2(Literal& p2) {
        pointer2 = p2;
      }

      void print() {
          for (size_t j = 0; j < literals.size(); ++j) {
                std::cout << literals[j].getName();
                if (j < literals.size() - 1) {
                    std::cout << " + ";
                }
            }
      }

};

class ClauseVector {
  private:
      std::vector<Clause> clauses;
  public:
      // Method to add a clause to the vector //
      void addClause(std::string& clauseString) {
          clauses.push_back(Clause(clauseString));
      }

      // Method to read clauses from a file and add them to the vector //
      void readClausesFromFile(const std::string& filename) {
          std::ifstream file(filename);
          if (file.is_open()) {
              std::string line;
              while (std::getline(file, line)) {
                  // Remove white spaces from the line
                  line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
                  addClause(line);
              }
              file.close();
          } else {
              std::cerr << "Unable to open file: " << filename << std::endl;
          }
      }

      void print() {
        for (size_t i = 0; i < clauses.size(); ++i) {
            std::cout << "Clause " << (i + 1) << ": ";
            auto& literals = clauses[i].getLiterals();

            for (size_t j = 0; j < literals.size(); ++j) {
                std::cout << literals[j].getName();
                if (j < literals.size() - 1) {
                    std::cout << " + ";
                }
            }
            std::cout << std::endl;
        }
      }

      std::vector<Variable> extractVariables() {
          std::vector<Variable> variables;
          int clauseIndex = 0; 
          for (auto& clause : clauses) {
              for (auto& literal : clause.getLiterals()) {
                  std::string varName = literal.getName();
                  bool negated = (varName.back() == 'r' && varName[varName.length() - 2] == 'a' && varName[varName.length() - 3] == 'b');
                  if (negated) {
                      varName = varName.substr(0, varName.length() - 3);
                  }
                  auto it = std::find_if(variables.begin(), variables.end(), [&](Variable& var) { return var.getVariable() == varName; });
                  if (it == variables.end()) {
                      Variable newVar(varName);
                      newVar.addClause(clauseIndex);
                      variables.push_back(newVar);
                      literal.setVariableIndex(variables.size()-1); 
                  } else {
                      it->addClause(clauseIndex);
                      size_t varIndex = std::distance(variables.begin(), it);
                      literal.setVariableIndex(varIndex);
                  }
              }
              clauseIndex++;
          }
          return variables;
      }

      int getSize(){
        return clauses.size();
      }

      std::vector<Clause>& getClauses(){
        return clauses;
      }
};

//                                        *************** CDCLSolver ********************                                                   //
// 1) satisfy unit clauses and propagate to the rest of the clauses, e.g (x1)(x1bar+x2), x1=1(satisfy unit clause), x2=1(implied value)     //
// 2) assignment: pick the literal that occur in the most clauses and assign it to 1                                                        //
// 2) if no conflict, propagate choice through the CNF finding implications                                                                 //
// 3) if SAT all done, if a propagation leads to a conflict, 1st UIP is identified and conlfict clause is produced and added to the formula //
// 4) iterate all the above steps until SAT or UNSAT is proven                                                                              //
class CDCLSolver{
  private:
    ClauseVector clauseVector;
    std::vector<Variable> variables;
    std::vector<TrailEntry> decisionTrail;
  public:
    CDCLSolver(ClauseVector& ClauseVec, std::vector<Variable>& vars) : clauseVector(ClauseVec), variables(vars){
      for (auto& clause : clauseVector.getClauses()) {
        clause.setAssignment(false);
      }
    }

    bool solve(){
      return backtrack(0);
    }
  private:
    bool backtrack(int decisionLevel){
      Literal unassignedLiteral; // next literal to decide //

      // step 1: satisfy unit clauses and propagate to the rest of the clauses //
      auto propagateUnitClausesResult = propagateUnitClauses();
      if(propagateUnitClausesResult.second){
        return false;
      }
      if (propagateUnitClausesResult.first) {
        printDecisionTrail(decisionTrail);
        return true; // If all unit clauses are satisfied by propagation, return true //
      }
      
      // step 2: assignment: pick the literal that occur in the most clauses and assign it to 1 //
      auto result = selectLiteral(clauseVector.getClauses(), decisionTrail); 
      unassignedLiteral = result.first;
      bool noconflict = result.second;
      if (!noconflict){
        std::cout << "conflict detected" << std::endl;
        std::pair<std::string, bool> result = findUniqueLevelFromEnd(decisionTrail);
        if (result.second) {
            std::cout << "Unique Implication Point (UIP) found: " << result.first << std::endl;
            std::string literalToAdd;
            // complement of uip is added to the clauses //
            if (result.first.back() == 'r' && result.first[result.first.length() - 2] == 'a' && result.first[result.first.length() - 3] == 'b') {
                literalToAdd = result.first.substr(0, result.first.size() - 3);
            } else {
                literalToAdd = result.first + "bar"; 
            }
            std::cout << "info: " << literalToAdd << " conflict clause is added to the clauses " << std::endl;
            clauseVector.addClause(literalToAdd);
            // declare all clauses as unsatisfied, Clear the decision trail and start from decision level 0 again //
            for (auto& clause : clauseVector.getClauses()) {
              clause.setAssignment(false);
            }
            decisionTrail.clear();
            decisionLevel = 0;
            if (backtrack(0)){
              return true;
            }
        } else {
            std::cout << "No Unique Implication Point (UIP) found." << std::endl;
        }
      }
      else {
        decisionTrail.emplace_back(unassignedLiteral.getName(), 0);
        if (!propagate(unassignedLiteral)){
          printDecisionTrail(decisionTrail);
          std::cout << "conflict detected" << std::endl;
          std::pair<std::string, bool> result = findUniqueLevelFromEnd(decisionTrail);
          if (result.second) {
              std::cout << "Unique Implication Point (UIP) found: " << result.first << std::endl;
              std::string literalToAdd;
              // complement of uip is added to the clauses //
              if (result.first.back() == 'r' && result.first[result.first.length() - 2] == 'a' && result.first[result.first.length() - 3] == 'b') {
                  literalToAdd = result.first.substr(0, result.first.size() - 3);
              } else {
                  literalToAdd = result.first + "bar"; 
              }
              std::cout << "info: " << literalToAdd << " conflict clause is added to the clauses " << std::endl;
              clauseVector.addClause(literalToAdd);
              // declare all clauses as unsatisfied, Clear the decision trail and start from decision level 0 again //
              for (auto& clause : clauseVector.getClauses()) {
                clause.setAssignment(false);
              }
              decisionTrail.clear();
              decisionLevel = 0;
              if (backtrack(0)){
                return true;
              }
              else return false;
          } else {
              std::cout << "No Unique Implication Point (UIP) found." << std::endl;
          }
        } 
        printDecisionTrail(decisionTrail);
         if (backtrack(decisionLevel+1)){
          return true;
        }
      }
      std::cout << "conflict detected " << std::endl;
      return false;
    }

    std::pair<bool, bool> propagateUnitClauses() {
      bool allSatisfied = true;
      bool conflict = false;

      for (auto& clause : clauseVector.getClauses()) {
          if (clause.getLiterals().size() == 1 && !clause.getAssignment()) {
              Literal unitLiteral = clause.getLiterals()[0];
              clause.setAssignment(true); // Mark the unit clause as satisfied //
              decisionTrail.emplace_back(unitLiteral.getName(), 0); // Add TrailEntry //
              // Check if the literal or its negation is already in the decision trail //
              if (std::find_if(decisionTrail.begin(), decisionTrail.end(), [&](const auto& entry) {
                  return (entry.literal == unitLiteral.getName() + "bar" || entry.literal + "bar" == unitLiteral.getName());
              }) != decisionTrail.end()) {
                  conflict = true; // Conflict detected //
                  return std::make_pair(allSatisfied, conflict);
              }
              printDecisionTrail(decisionTrail);
              propagate(unitLiteral); // Propagate the satisfied literal //
          }
      }
      allSatisfied = allClausesSatisfied(); // Check if all unit clauses are satisfied //

      return std::make_pair(allSatisfied, conflict);
  }

    // Function to find the entry with a unique level and different literals before and after it //
    std::pair<std::string, bool> findUniqueLevelFromEnd(const std::vector<TrailEntry>& decisionTrail) {
        std::unordered_map<int, int> levelCounts;
        std::pair<std::string, bool> result("", false); // Initialize with empty literal and false for not found //

        // Iterate from the end of the decision trail //
        for (int i = decisionTrail.size() - 1; i >= 0; --i) {
            const TrailEntry& entry = decisionTrail[i];
            
            // Count occurrences of each level //
            levelCounts[entry.level]++;

            // Check if the current entry has a unique level //
            if (levelCounts[entry.level] == 1) {
                // Check if the literals before and after it are different //
                if (i == 0 || (i < decisionTrail.size() - 1 && decisionTrail[i - 1].literal != decisionTrail[i + 1].literal)) {
                    result.first = entry.literal;
                    result.second = true; // Set found to true for UIP found
                    break;
                }
            }
        }
        return result;
    }

    bool propagate(Literal assignedLiteral) {
        bool conflict = false;
        for (auto& clause : clauseVector.getClauses()){
          bool clauseSatisfied = false;
          bool unitClause = false;
          Literal unitLiteral;
          std::vector<Literal>& literals = clause.getLiterals();
          int maxLevel = 0; // Initialize max level to 0 //

          if (clause.getAssignment()){
            continue;
          }
          if (clause.getLiterals().size() == 1) {
                // Skip unit clauses//
                continue;
            }

          // Find the maximum level of literals in the reason clause //
          for (auto& literal : literals) {
            for (const auto& entry : decisionTrail) {
                if (entry.literal == literal.getName() || entry.literal == literal.getName() + "bar" || entry.literal + "bar" == literal.getName()) {
                    maxLevel = std::max(maxLevel, entry.level + 1);
                    break;
                }
            }
          }

          for (auto& literal : literals){
            if (literal.getName() == assignedLiteral.getName()){
              clauseSatisfied = true;
              clause.setAssignment(clauseSatisfied);
              break;
            }
            if ((literal.getName() == assignedLiteral.getName() + "bar") || (literal.getName() + "bar" == assignedLiteral.getName())){
              // advance pointers //
              if(clause.getPointer1().getName() == literal.getName()){
                  size_t index1 = std::find(literals.begin(), literals.end(), clause.getPointer1()) - literals.begin();
                  clause.setPointer1(literals[(index1 + 1) % literals.size()]);
              }

              if (clause.getPointer2().getName() == literal.getName()) {
                  size_t index2 = std::find(literals.begin(), literals.end(), clause.getPointer2()) - literals.begin();
                  size_t newIndex = (index2 == 0) ? literals.size() - 1 : index2 - 2;
                  clause.setPointer2(literals[newIndex]);
              }

              if (clause.getPointer1().getName() == clause.getPointer2().getName()) {
                    // Check for conflict //
                    std::cout << std::endl;
                    if (std::find_if(decisionTrail.begin(), decisionTrail.end(), [&](const auto& entry) {
                        return (entry.literal == clause.getPointer1().getName() + "bar" || entry.literal + "bar" == clause.getPointer1().getName());
                    }) != decisionTrail.end()) {
                        conflict = true;
                        return !conflict;
                    } else {
                        decisionTrail.emplace_back(clause.getPointer1().getName(), maxLevel); // Add TrailEntry
                        clause.setAssignment(true);
                        propagate(clause.getPointer1());
                    }
                }
            }
          }
        }
        return !conflict;
      }

    bool allClausesSatisfied() {
        for ( auto& clause : clauseVector.getClauses()) {
              bool clauseSatisfied = false;
              if (clause.getAssignment() == true) {
                  clauseSatisfied = true;
              }
            
            if (!clauseSatisfied) {
                return false;
            }
        }
        return true;
    }

   std::pair<Literal, bool>  selectLiteral(std::vector<Clause> formula, std::vector<TrailEntry> decisionTrail) {
      std::unordered_map<std::string, int> literalCount;
      bool literalInTrail = true;
      bool conflict = true;
      // Count occurrences of each literal only in unsatisfied clauses //
      for (auto& clause : formula) {
          if (clause.getAssignment()){
            continue;
          }
          for (auto& literal : clause.getLiterals()) {
            // Check if the literal or its negation is already in the decision trail //
            literalInTrail = false;
            for (auto& entry : decisionTrail) {
                if (entry.literal == literal.getName() || entry.literal == literal.getName() + "bar" || literal.getName() == entry.literal + "bar") {
                    literalInTrail = true;
                    break;
                }
            }
            // If the literal is not in the decision trail, count its occurrences //
            if (!literalInTrail) {
                conflict = false;
                std::string literalName = literal.getName();
                literalCount[literalName]++;
            }
        }
      }

      // Find the literal with the maximum occurrences //
      std::string selectedLiteralName;
      int maxOccurrences = 0;
      for (auto& pair : literalCount) {
          if (pair.second > maxOccurrences) {
              selectedLiteralName = pair.first;
              maxOccurrences = pair.second;
          }
      }
    
      // Find the literal object with the selected name //
      for (auto& clause : formula) {
          for (auto& literal : clause.getLiterals()) {
              if (literal.getName() == selectedLiteralName) {
                  return std::make_pair(literal, !conflict);
              }
          }
      }
      // If no literal found, return a default literal and report conflict //
      return std::make_pair(formula[0].getLiterals()[0], !conflict);
    }

    void printAssignement() {
      std::cout << "Satisfying assignment:" << std::endl;
      for (auto i : variables) {
        std::cout << i.getVariable() << ": " << (i.getAssignment() ? "true" : "false") << std::endl;
      }
    }

    void printVariables() {
      std::cout << "Variables<name, index>:" << std::endl;
      int index = 0;
      for (auto it = variables.begin(); it != variables.end(); ++it) {
          std::cout << "<" << it->getVariable() << ", " << index << ">" << std::endl;
          index++;
      }
    }

    void printDecisionTrail(const std::vector<TrailEntry>& decisionTrail) {
        std::cout << "Decision Trail<literal, graph level>:" ;
        for (const auto& entry : decisionTrail) {
            std::cout << "<" << entry.literal << "," << entry.level << ">" <<" ";
        }
        std::cout << std::endl ;
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::string inputFileName = argv[1];
    ClauseVector clauseVector; // vector that contains all clauses //
    clauseVector.readClausesFromFile(inputFileName);
    std::vector<Variable> vars = clauseVector.extractVariables();

    CDCLSolver solver(clauseVector, vars);
    std::cout << "Given clauses: "<< std::endl; 
    clauseVector.print();
    if (solver.solve()){

    }
    else {
        std::cout << "Formula is unsatisfiable." << std::endl;
        return false; // Unsatisfiable
    }

    return 0;
}
