import re
import sys
import collections

# add
def add_func():
    doMath('+')
    return

# sub
def sub_func():
    doMath('-')
    return

# mul
def mul_func():
    doMath('*')
    return

# div
def div_func():
    doMath('/')
    return

# eq
def eq_func():
    operand2 = popStack(opStack, "eq")
    operand1 = popStack(opStack, "eq")
    
    if (operand1 == operand2):
        opStack.append("true")
    else:
        opStack.append("false")
    return

# lt
def lt_func():
    doMath('<')
    return

# gt
def gt_func():
    doMath('>')
    return

# and
def and_func():
    doLogic('and')
    return

# or
def or_func():
    doLogic('or')
    return

# not
def not_func():
    operand = toBool(popStack(opStack), "not")
    if operand:
        opStack.append("false")
    else:
        opStack.append("true")
    return

# if
def if_func():
    code = popStack(opStack, "if")
    bool = popStack(opStack, "if")
    
    if isCode(code, "if"):
        bool = toBool(bool, "if")
        if (bool):
            evaluate(code[1:-1])
    return

# ifelse
def ifelse_func():
    falseCode = popStack(opStack, "ifelse")
    trueCode = popStack(opStack, "ifelse")
    bool = popStack(opStack, "ifelse")
    
    if (isCode(falseCode, "ifelse") and isCode(trueCode,"ifelse")):
        bool = toBool(bool, "ifelse")
        if (bool):
            evaluate(trueCode[1:-1])
        else:
            evaluate(falseCode[1:-1])
    return

# dup
def dup_func():
    operand = popStack(opStack, "dup")
    opStack.append(operand)
    opStack.append(operand)
    return

# exch
def exch_func():
    operand1 = popStack(opStack, "exch")
    operand2 = popStack(opStack, "exch")
    opStack.append(operand1)
    opStack.append(operand2)
    return

# pop
def pop_func():
    popStack(opStack, "pop")
    return

# dict
def dict_func():
    operand1 = popStack(opStack, "dict")
    opStack.append({})
    return

# begin
def begin_func():
    dictionary = popStack(opStack, "begin")
    if (type(dictionary) != dict):
        error("Compatibility", "begin")
    else:
        #print ("Pushing a dictionary")
        dictStack.append(dictionary)
    return

# end
def end_func():
    if (len(dictStack) < 2):
        error("EmptyStack", "end")
    else:
        dictStack.pop()
    return

# def
def def_func():
    value = popStack(opStack, "def")
    identifier = popStack(opStack, "def")
    
    if (isIdentifier(identifier) == False):
        error("badIdentifier", "def")
    else:
        # FILTHY
        dictStack[-1][identifier[1:]] = value
    return

# stack
def stack_func():
    # My cool reverse printing function
    for i in range(len(opStack)):
        print(opStack[-(i + 1)])
    return

# =
def eqOp_func():
    operand = popStack(opStack, "=")
    print(operand)
    return

# Pop the stack, with error handling
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
        i = float(string)
    except:
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
        operand1 = float(operand1)
        operand2 = float(operand2)
        if op == "+":
            result = operand1 + operand2
        elif op == "-":
            result = operand1 - operand2
        elif op == "*":
            result = operand1 * operand2
        elif op == "/":
            if operand2 != 0:
                result = operannd1 / operand2
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
    operand2 = toBool(popStack(opStack), op)
    operand1 = toBool(popStack(opStack), op)    
    if op == "and":
        return operand1 and operand2
    elif op == "or":
        return operand1 or operand2
            

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
    for i in range(len(dictStack)):
        tmp = dictStack[-(i + 1)]
        if (identifier[1:] in tmp):
            return tmp.get(identifier[1:])
    error("notDef", identifier)

# Function that prints the dictionary stack
def printDicionaryStack():
    print ("=================================================")
    for i in range(len(dictStack)):
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
    


dictStack = [{'add': add_func, 'sub': sub_func, 'mul': mul_func, 'div': div_func, 'eq': eq_func, 'lt': lt_func, 'gt': gt_func, 'and': and_func, 'or': or_func, 'not': not_func, 'if': if_func, 'ifelse': ifelse_func, 'dup': dup_func, 'exch': exch_func, 'pop': pop_func, 'dict': dict_func, 'begin': begin_func, 'end': end_func, 'def': def_func, 'stack': stack_func, '=': eqOp_func }]

opStack = []


# This function is the recursive heart, nay the soul of this program
def evaluate (tokens):
    i = 0
    while (i < len(tokens)):
        token = tokens[i]
        #print("Read Token: ",token)
        if isIdentifier(token):
            #print("Pushing Token: ",token)
            opStack.append(token)
        elif isIdentifier('/' + token):
            #print("Dereferencing Token: ",token)
            # got to find its value
            definition = searchDictionaryStack('/' + token)
            if (isinstance(definition, collections.Callable)):
                #print("Calling Token: ",token)
                definition()
            elif (isCode(definition, definition)):
                evaluate(definition[1:-1])
            else:
                opStack.append(definition)
        elif (token == "{"):
            closingBraceIndex = findMatchingCurlyBrace(tokens, i)
            opStack.append(tokens[i:closingBraceIndex + 1])
            i = closingBraceIndex
        else:
            opStack.append(token)
        i += 1
    return
