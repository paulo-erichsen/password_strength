/******************************************************************************
* Program:
*    Password Strength Analyzer
* Author:
*    Paulo Fagundes
* Summary:
*    This program is designed to compute the strength of a password.
*    First it prompts the user for some password. (could be a line of input)
*    Then it computes the number of combinations and the minimum number of bits
*       to represent such number of combinations
*    Finally it displays to the user these computed values.
*
*    This program assumes that the user won't input an ASCII character of 128+
******************************************************************************/
#include <iostream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <termios.h>
#include <unistd.h>
using namespace std;

// Constants for the number of possiblities of each type of character
enum
{
   NUMBERS = 10,
   LOWERCASE = 26,
   UPPERCASE = 26,
   SYMBOLS = 32,
   SPACE = 1,
   TAB = 1,
   OTHER = 1 // this is an arbitrary value, other values could be much higher
};                                         //  think ASCII values of 128+

/******************************************************************************
* PasswordAnalyzer - a class that prompts for a password and displays
*                    the strength of the given password
******************************************************************************/
class PasswordAnalyzer
{
   private:
      string password;
      long double numCombinations;
      int numBits;

      void promptPassword();
      void computeNumCombinations();
      void computeNumBits();

   public:
      PasswordAnalyzer();
      void run();
      friend ostream& operator << (ostream& os, const PasswordAnalyzer &pa);
};

/******************************************************************************
* main()
*    main creates a PasswordAnalyzer object and executes it's run function
******************************************************************************/
int main()
{
   PasswordAnalyzer password;

   password.run();

   return 0;
}

/******************************************************************************
* public constructor
******************************************************************************/
PasswordAnalyzer::PasswordAnalyzer()
: password(""), numCombinations(0), numBits(0)
{

}

/******************************************************************************
* run()
* This function
*  1 - prompts the user for a password
*  2 - computes the number of password combinations that password has
*  3 - computes the number of bits required to represent it
*  4 - displays the computed information to the user
******************************************************************************/
void PasswordAnalyzer::run()
{
   promptPassword();
   computeNumCombinations();
   computeNumBits();
   cout << *this;
}

/******************************************************************************
* promptPassword()
* Prompts the user for any password
* The input could contain spaces
******************************************************************************/
void PasswordAnalyzer::promptPassword()
{
   bool display_password = true;

   // depending on the above bool, display or hide the given password:
   // http://stackoverflow.com/questions/1413445/read-a-password-from-stdcin
   struct termios tty;
   tcgetattr(STDIN_FILENO, &tty);
   if( !display_password )
      tty.c_lflag &= ~ECHO;
   else
      tty.c_lflag |= ECHO;

   (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);

   // prompt for the password
   string prompt = "Please enter the password: ";
   string errMsg = "Invalid entry! Please try again!\n";

   cout << prompt;
   getline(cin, password, '\n');

   while (cin.fail())
   {
      cin. clear();
      cin.ignore(80,'\n');
      cout << errMsg;
      cout << prompt;
      getline(cin, password);
   };

   // reset the option to enable the tty to display the inputs
   tty.c_lflag |= ECHO;
   tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

/******************************************************************************
* computeNumCombinations()
* This function iterates through the string checking which character sets are
*    found.
* When it finishes analyzing the character sets, it computes the number of
*    combinations based on the size of the password.
* the formula is n^m,
*       where n is the number of different characters
*       and m is the number of characters the password has (it's length)
******************************************************************************/
void PasswordAnalyzer::computeNumCombinations()
{
   // 0-NUMBERS, 1-LOWERCASE, 2-UPPERCASE, 3-SYMBOLS, 4-SPACE, 5-TAB, 6-OTHER
   bool charTypes[7] = {0}; // bool charTypes[] = {0, 0, 0, 0, 0, 0, 0};

   for (int i = 0; i < password.length(); i++)
   {
      if (isdigit(password[i]))
         charTypes[0] = 1; // NUMBERS
      else if (islower(password[i]))
         charTypes[1] = 1; // LOWERCASE
      else if (isupper(password[i]))
         charTypes[2] = 1; // UPPERCASE
      else if (ispunct(password[i]))
         charTypes[3] = 1; // SYMBOLS
      else if (password[i] == ' ')  // was gonna use isspace(), but it doesn't
         charTypes[4] = 1; // SPACE      // differentiate between tab vs space
      else if (password[i] == '\t') // not many passwords contain tabs, but
         charTypes[5] = 1; // TAB        // why not check for it anyways?
      else
         charTypes[6] = 1; // OTHER
   }

   numCombinations = (charTypes[0] * NUMBERS)   + (charTypes[1] * LOWERCASE)
                   + (charTypes[2] * UPPERCASE) + (charTypes[3] * SYMBOLS)
                   + (charTypes[4] * SPACE)     + (charTypes[5] * TAB)
                                                + (charTypes[6] * OTHER);

   numCombinations = pow(numCombinations, password.length());
}

/******************************************************************************
* computeNumBits()
* computes the number of bits required to represent the number of combinations
* the formula is : log base 2 of the number of combinations
******************************************************************************/
void PasswordAnalyzer::computeNumBits()
{
   numBits = floor(log(numCombinations) / log(2));
}

/******************************************************************************
* Overloaded Insertion operator
* displays the computed data of the PasswordAnalyzer object.
******************************************************************************/
ostream& operator << (ostream& os, const PasswordAnalyzer &pa)
{
   os.setf(ios::fixed);
   os.precision(0);

   os << "There are " << pa.numCombinations << " combinations" << endl;
   os << "That is equivalent to a key of " << pa.numBits << " bits" << endl;

   return os;
}
