import re
import sys

from basicFunctions import *

#
#   Author: Cameron D Hawkins
#   Date Modified: September 30, 2011
#   File: sps.py
#   File Desciption: This file is has a python executable main function 
#   Crutial Design Choices:
#           1) This function doesnt run unless its caled from terminal
#

def main():
    # Run the reader Funtions
    tokens = readerIntoTokens()
    
    # Start the execution
    evaluate(tokens)
    
    # Exit Output
    print ("\n============== Evaluation Complete ==============")
    print ("Here is the content of the stack, top to bottom:")
    stack_func()
    print ("And Here is the library of dictionaries:")
    printDicionaryStack()
    print ("Have a nice day :)")
    return



# Run main, only if this function was called from terminal.
if __name__ == "__main__":
    main()
else:
    print("Please run this program from terminal.")
    quit()