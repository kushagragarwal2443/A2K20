def check_palindrome(seq):
    
    '''Returns 1 if the input sequence is a palindrome, -1 otherwise'''
    
    seq = seq.lower() # Converts to lowercase
    length = len(seq) 
    rev_seq = ""
    
    for i in range(length):

        if(seq[i] == "a"):
            letter = "t"
        elif(seq[i]== "t"):
            letter = "a"
        elif(seq[i] == "g"):
            letter = "c"
        else:
            letter = "g"

        rev_seq+=letter

    if(rev_seq[::-1] == seq): # if reverse complement == original then palindrome
        return 1
    else: # not a palindrome
        return -1

def traverse_sequence(dna, k):
    
    '''This function traverses through the lenght of the sequence and finds all palindromic sites of length k'''
     
    sites = [] # List of palindromic sites
    dna = dna.lower() # Convert to lower case
    
    for i in range(len(dna)-k+1):
        
        seq = dna[i:i+k] # k-mer formed
        val = check_palindrome(seq) # check if the k-mer is palindromic
        if(val == 1):
            sites.append(seq) # if palindromic append to the list
            
    return sites

def driver():
    
    '''This function gives an interface to interact with the program.
    Input: DNA and value of k
    Output: All recoginition sites of length k'''
    
    dna = input("Enter DNA sequence: ")
    k = int(input("Enter value of k: "))
    
    dna = dna.lower() # Convert to lower case
    answer = traverse_sequence(dna, k) # Function call
    print("\nNumber of Recognition sites of length k:", len(answer))
    for i in range(len(answer)):
        print(answer[i])
    
driver()

