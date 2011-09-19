import re
import sys

from basicFunctions import *

def main():
    # Run the reader Funtions
    tokens = readerIntoTokens()
    # Start the execution
    evaluate(tokens)
    
    
    print ("\n============== Evaluation Complete ==============")
    print ("Here is the content of the stack:")
    stack_func()
    print ("And Here is the library of dictionaries:")
    printDicionaryStack()
    print ("Have a nice day :)")
    return


# Given a string, return the tokens it contains
def parse(string):
    pattern = '/?[a-zA-Z][a-zA-Z0-9_]*|[-]?[0-9]+|[}{]+|%.*|[^\t\n ]'
    tokens = re.findall(pattern, string)
    return tokens

# Reads form the input file into tokens
def readerIntoTokens ():
    # If the user did not enter in a file name, return
    if (len(sys.argv) <= 1):
        print ("No input file name specified.")
        quit()
    
    fn = sys.argv[1]
    
    try:
        filehandle = open(fn)
    except:
        print("Input File not found.")
        quit()
    
    tokens = parse(''.join(filehandle.readlines()))
    return tokens
    

# Exit gracefully
def quit ():
    sys.exit(1)


# Run main, only if this function was called from terminal.
if __name__ == "__main__":
    main()
else:
    print("Please run this program from terminal.")
    quit()