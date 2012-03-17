import re
import sys
import collections

#
#   Author: Cameron D Hawkins
#   Date Modified: September 30, 2011
#   File: basicFunctions.py
#   File Desciption: This file contains all of the helper functions and global 
#                   initializations that the sps.py function needs to work.
#   Crutial Design Choices:
#           1) System dictionary lives on the dictionary stack
#           2) Everything on the evaluation stack is a string
#           3) If any error is found, it prints it and quits executution 
#           imediately
#           4) Run-time execution is handled entirely by evaluate()
#



# Magic function: Given a string, return the PS tokens it contains
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

# add, calls the doMath function with the +
def add_func():
    doMath('+')
    return

# sub, calls the doMath function with the -
def sub_func():
    doMath('-')
    return

# mul, calls the doMath function with the *
def mul_func():
    doMath('*')
    return

# div, calls the doMath function with the /
def div_func():
    doMath('/')
    return

# eq
def eq_func():
    operand2 = popStack(opStack, "eq")
    operand1 = popStack(opStack, "eq")
    
    # Simple, if their equal, push 'true'
    if (operand1 == operand2):
        opStack.append("true")
    else:
        opStack.append("false")
    return

# lt, calls the doMath function with the <
def lt_func():
    doMath('<')
    return

# gt, calls the doMath function with the >
def gt_func():
    doMath('>')
    return

# and, calls the doLogic function with 'and'
def and_func():
    doLogic('and')
    return

# or, calls the doLogic function with 'or'
def or_func():
    doLogic('or')
    return

# not
def not_func():
    # convert the string to a python bool, then evaluate it
    operand = toBool(popStack(opStack), "not")
    # return its oposite
    if operand:
        opStack.append("false")
    else:
        opStack.append("true")
    return

# if
def if_func():
    # pop twice
    code = popStack(opStack, "if")
    bool = popStack(opStack, "if")
    
    # if the bool is true, call evaluate on the code
    if isCode(code, "if"):
        bool = toBool(bool, "if")
        if (bool):
            evaluate(code[1:-1])
    # else return
    return

# ifelse
def ifelse_func():
    # Pop 3 times
    falseCode = popStack(opStack, "ifelse")
    trueCode = popStack(opStack, "ifelse")
    bool = popStack(opStack, "ifelse")
    
    # if the bool is true, call evaluate on the 'true' code
    if (isCode(falseCode, "ifelse") and isCode(trueCode,"ifelse")):
        bool = toBool(bool, "ifelse")
        if (bool):
            evaluate(trueCode[1:-1])
        # else call evaluate on the 'false' code
        else:
            evaluate(falseCode[1:-1])
    return

# dup
def dup_func():
    # pop once, push twice
    operand = popStack(opStack, "dup")
    opStack.append(operand)
    opStack.append(operand)
    return

# exch
def exch_func():
    # pop twice, push back in opposite order
    operand1 = popStack(opStack, "exch")
    operand2 = popStack(opStack, "exch")
    opStack.append(operand1)
    opStack.append(operand2)
    return

# pop
def pop_func():
    # just pop
    popStack(opStack, "pop")
    return

# dict
def dict_func():
    # pop the dummy var off, then push an empty dictionary to the evaluation stack
    operand1 = popStack(opStack, "dict")
    opStack.append({})
    return

# begin
def begin_func():
    # pop top element off the stack
    dictionary = popStack(opStack, "begin")
    # if the its a dictionary, push it to the dictionary stack
    if (type(dictionary) != dict):
        error("Compatibility", "begin")
    else:
        #print ("Pushing a dictionary")
        dictStack.append(dictionary)
    return

# end
def end_func():
    # make sure not to pop the system dictionary!
    if (len(dictStack) < 2):
        error("EmptyStack", "end")
    else:
        dictStack.pop()
    return

# def
def def_func():
    # pop the value, pop the identifier
    value = popStack(opStack, "def")
    identifier = popStack(opStack, "def")
    
    # if the identifier is in the proper form, define it on the top dictionary
    if (isIdentifier(identifier) == False):
        error("badIdentifier", "def")
    else:
        # On the top dicitonary, push the identifier without its first '/' character
        # as the key for the 'value'
        dictStack[-1][identifier[1:]] = value
    return

# stack
def stack_func():
    # My cool reverse printing function
    for i in range(len(opStack)):
        # Print the -ith element
        # made sure it's not off by 1
        print(opStack[-(i + 1)])
    return

# =
def eqOp_func():
    # pretty simple, pop the stack and print it 
    operand = popStack(opStack, "=")
    print(operand)
    return

# Generic pop the stack function, with error handling
def popStack (list, func):
    if (len(list) <= 0):
        error("EmptyStack", func)
        sys.exit(1)
    else:
        return list.pop()

# Function to find out if a given operand is a number
def isNumber (string):
    # I found this number validation code online
    try:
        i = int(string)
    except ValueError:
        return False
    except TypeError:
        return False
    else:
        return True

# Helper Function to find out if a operand is a bool
def isBool (bool):
    if (bool == "true") or (bool == "false"):
        return True
    else:
        return False

# Converts the lowercase bool string to python bool
def toBool (operand, func):
    if isBool(operand):
        if (operand == "false"):
            return False
        else:
            return True
    else:
        error("Compatibility", func)

# Checks to see if something looks like code
def isCode (code, func):
    if ((type(code) != list) or (code[0] != "{") or (code[-1] != "}")):
        return False
    else:
        return True
 
# Checks if the identifier matches the regualr expression             
def isIdentifier (string):
    pattern = '/[a-zA-Z][a-zA-Z0-9]*'
    if (None == re.match(pattern, string)):
        return False
    else:
        return True
    

# Put all of the math together
def doMath (op):
    operand2 = popStack(opStack, op)
    operand1 = popStack(opStack, op)
    # Do the correct math based on the string input
    if (isNumber(operand1) and isNumber(operand2)):
        operand1 = int(operand1)
        operand2 = int(operand2)
        if op == "+":
            result = str(operand1 + operand2)
        elif op == "-":
            result = str(operand1 - operand2)
        elif op == "*":
            result = str(operand1 * operand2)
        elif op == "/":
            if operand2 != 0:
                result = str(operannd1 / operand2)
            else: 
                error("divByZero", op)
        elif op == "<":
            if (operand1 < operand2):
                result = "true"
            else:
                result = "false"
        elif op == ">":
            if (operand1 > operand2):
                result = "true"
            else:
                result = "false"

        opStack.append(result)
    else:
        error("Compatibility", op)
    return

def doLogic (op):
    # Makes operands bools if they can
    operand2 = toBool(popStack(opStack, op), op)
    operand1 = toBool(popStack(opStack, op), op)    
    if op == "and":
        if (operand1 and operand2):
            opStack.append("true")
            return
    elif op == "or":
        if (operand1 or operand2):
            opStack.append("true")
            return
    opStack.append("false")
    return
            
# Generic error printing function, lets you know what function failed and why it failed
def error (errorType, function):
    print("Execution Error in:", function)
    if errorType == "EmptyStack":
        print("Attempt to preform operation on an empty stack.")
    elif errorType == "Compatibility":
        print("Operation attempted with incompatible data types.")
    elif errorType == "divByZero":
        print("Program attempted to divide by 0.")
    elif errorType == "notCode":
        print("Program attempted to execute something that wasn't code.")
    elif errorType == "notDef":
        print("Could not find identifier's definition.")
    sys.exit(1)

# Search a dictionary from top to bottom for identifier
def searchDictionaryStack (identifier):
    # iterate through the dictionary stack from right to left
    for i in range(len(dictStack)):
        # get the top dictionary
        tmp = dictStack[-(i + 1)]
        # look through it for the identifier
        if (identifier[1:] in tmp):
            # found it!
            return tmp.get(identifier[1:])
    error("notDef", identifier)

# Function that prints the dictionary stack
def printDicionaryStack():
    print ("=================================================")
    for i in range(len(dictStack)):
        # print the dicitonary stack right to left
        print ("Dictionary ", i, " :")
        print (dictStack[-(i + 1)])
        print ("=================================================")

# Goes through the list of tokens and returns the index of the matched curly brace
def findMatchingCurlyBrace (tokens, start):
    numOpenBraces = 1
    for i in range(start + 1, len(tokens)):
        #print("Surf Through Token: ", tokens[i] )
        if (tokens[i] == "{"):
            numOpenBraces += 1
        elif (tokens[i] == "}"):
            numOpenBraces -= 1
            if (numOpenBraces == 0):
                return i
    print ("No matching closing bracket Found")
    sys.exit(1)
    

# initialize the global dictionary stack with all of the built in functions
dictStack = [{'add': add_func, 'sub': sub_func, 'mul': mul_func, 'div': div_func, 'eq': eq_func, 'lt': lt_func, 'gt': gt_func, 'and': and_func, 'or': or_func, 'not': not_func, 'if': if_func, 'ifelse': ifelse_func, 'dup': dup_func, 'exch': exch_func, 'pop': pop_func, 'dict': dict_func, 'begin': begin_func, 'end': end_func, 'def': def_func, 'stack': stack_func, '=': eqOp_func }]

# initialize the global operation stack as an empty list
opStack = []


# This function is the recursive heart, nay the soul of this program
def evaluate (tokens):
    # Takes in a list of tokens, executable PostScript 
    i = 0
    # I used a while loop. Breaking the rules, I know...
    # But I could not find a more elegant way to be able to push large segments of code
    # without using i < len(tokens) because I need to be able to jump forward, see
    # 'elif (token == "{")' for details
    while (i < len(tokens)):
        # Run-time Execution:
        token = tokens[i]
        # if its an identifier with a '/' at the beginning, just push it
        if isIdentifier(token):
            #print("Pushing Token: ",token)
            opStack.append(token)
        # elif its a PS bool, push it 
        elif isBool (token):
            opStack.append(token)
        # elif its a number, push it as a string -> everything on the stack is a string.
        elif isNumber(token):
            opStack.append(token)
        # elif its an identifier that needs to be dereferenced: 
        elif isIdentifier('/' + token):
            # got to dictionary to find its value
            definition = searchDictionaryStack('/' + token)
            # if its a system function, means that its a python "callable instance"
            if (isinstance(definition, collections.Callable)):
                # call the function associated with the definition
                definition()
            # else if it is defined as PS code
            elif (isCode(definition, definition)):
                # Recursively call evaluate on it without its {}
                evaluate(definition[1:-1])
            else:
                # else, its a value, push it to the stack
                opStack.append(definition)
        # elif executable code is in the tokens
        elif (token == "{"):
            # fast-forward through the input tokens till you find the matchin bracket
            closingBraceIndex = findMatchingCurlyBrace(tokens, i)
            # push that list of code onto the stack
            opStack.append(tokens[i:closingBraceIndex + 1])
            # jump the evaluation 'i' till the end of the code on the stack
            i = closingBraceIndex
        else:
            error("notDef", "Evaluation")
        i += 1
        # go to the next token!
    return


# Exit gracefully
def quit ():
    sys.exit(1)
