dicti = dict() # Dictionary to convert codons to corr amino acid

dicti["aaa"] = "Lys"
dicti["aag"] = "Lys"
dicti["aac"] = "Asn"
dicti["aau"] = "Asn"
dicti["uuu"] = "Phe"
dicti["uuc"] = "Phe"
dicti["uua"] = "Leu"
dicti["uug"] = "Leu"
dicti["cuu"] = "Leu"
dicti["cuc"] = "Leu"
dicti["cua"] = "Leu"
dicti["cug"] = "Leu"
dicti["auu"] = "Ile"
dicti["auc"] = "Ile"
dicti["aua"] = "Ile"
dicti["aug"] = "Met"
dicti["guu"] = "Val"
dicti["guc"] = "Val"
dicti["gua"] = "Val"
dicti["gug"] = "Val"
dicti["ucu"] = "Ser"
dicti["ucc"] = "Ser"
dicti["uca"] = "Ser"
dicti["ucg"] = "Ser"
dicti["agu"] = "Ser"
dicti["agc"] = "Ser"
dicti["ccu"] = "Pro"
dicti["ccc"] = "Pro"
dicti["cca"] = "Pro"
dicti["ccg"] = "Pro"
dicti["acu"] = "Thr"
dicti["acc"] = "Thr"
dicti["aca"] = "Thr"
dicti["acg"] = "Thr"
dicti["gcu"] = "Ala"
dicti["gcc"] = "Ala"
dicti["gca"] = "Ala"
dicti["gcg"] = "Ala"
dicti["uau"] = "Tyr"
dicti["uac"] = "Tyr"
dicti["cau"] = "His"
dicti["cac"] = "His"
dicti["caa"] = "Gln"
dicti["cag"] = "Gln"
dicti["gau"] = "Asp"
dicti["gac"] = "Asp"
dicti["gaa"] = "Glu"
dicti["gag"] = "Glu"
dicti["ugu"] = "Cys"
dicti["ugc"] = "Cys"
dicti["ugg"] = "Trp"
dicti["cgu"] = "Arg"
dicti["cgc"] = "Arg"
dicti["cga"] = "Arg"
dicti["cgg"] = "Arg"
dicti["aga"] = "Arg"
dicti["agg"] = "Arg"
dicti["ggu"] = "Gly"
dicti["ggc"] = "Gly"
dicti["gga"] = "Gly"
dicti["ggg"] = "Gly"

def reverse_strand(dna):
    
    '''This function finds the corresponding reverse strand for a given forward DNA strand.
    Reverse strand read from 5' to 3' end is reverse of the complementary of forward strand of DNA'''
    
    dna = dna.lower() # Converting all bases to lower case 
    rev_dna = ""
    for i in range(len(dna)):
        
        # Complementary bases for all nucleotides
        if(dna[i] == "a"):
            letter = "t"
        elif(dna[i]== "t"):
            letter = "a"
        elif(dna[i] == "g"):
            letter = "c"
        elif(dna[i] == "c"):
            letter = "g"
        else: # if a base except for A,G,T,C is present, a flag is raised indicating incorrect dna sequence
            return -1
        
        rev_dna += letter # augmenting the reverse strand from 3' to 5' end
        
    rev_dna = rev_dna[::-1] # reversing the strand to read from 5' to 3' end
    
    return rev_dna

def rna(dna):
    
    '''This function finds the corresponding RNA for a given DNA sequence.
    RNA is complement of complement of forward strand of DNA. Therfore we can directly get the RNA by replacing T with U'''
    
    dna = dna.lower() # Converting all bases to lower case 
    
    for i in range(len(dna)): # Checking if DNA sequence is valid
        if(dna[i]!="a" and dna[i]!= "g" and dna[i]!="c" and dna[i]!="t"):
            return -1
        
    rna = dna.replace("t", "u") # RNA has Uracil(u) instead of Thymine(t)
    
    return rna

def start_codon(rna):
    
    '''This function searches for start codons in the rna sequence. 
    aug signifies a start codon, and if none is found then suitable flag is raised'''
    
    rna = rna.lower() # Converting all bases to lower case 
    start_codon_pos = -1 # if codon is not found then -1 is returned
    
    for i in range(len(rna)-2):
        
        if( rna[i] == "a" and rna[i+1] == "u" and rna[i+2] == "g"):
            start_codon_pos = i # update start_codon_pos
            return start_codon_pos
        
    return start_codon_pos # if start codon not found -1 will be returned 

def stop_codon(rna):
    
    '''This function finds the first postion of stop codon after the position of start_codon.
    UAA, UGA and UAG are stop codons'''
    
    rna = rna.lower() # Converting all bases to lower case 
    stop_codons = ["uaa", "uga", "uag"]
    stop_codon_pos = -1    # if no stop codon found, then -1 is returned
    start_codon_pos = start_codon(rna) # start from the position of the start_codon
    
    if(start_codon_pos == -1): # if no start codon present, then a stop codon does not make sense
        return -1
    
    for i in range(start_codon_pos, len(rna)-2, 3): # we need to check in the frame of the start_codon only, hence jumps of 3
        
        three_mer = rna[i] + rna[i+1] + rna[i+2]
        
        if(three_mer in stop_codons):
            stop_codon_pos = i # update stop_codon_pos to i
            return stop_codon_pos
        
    return stop_codon_pos # if no stop codon found then -1 is returned

def translate(rna):
    
    '''This function translates a valid RNA sequence with both start and stop codons into the amino acid sequence.
    The 3 letter sequence for each amino acid is used. Appropriate error message is shown if sequence is invalid'''
    
    rna = rna.lower() # Converting all bases to lower case 
    start_codon_pos = start_codon(rna)
    stop_codon_pos = stop_codon(rna)
    
    if(start_codon_pos == -1): # Start codon absent
        return "Start codon AUG not present in the sequence"
    elif(stop_codon_pos == -1): # Stop codon absent
        return "Stop codon not present in the reading frame starting from the start codon"
    else:      
        sequence = rna[start_codon_pos:stop_codon_pos] # seqeunce between start and stop codons
        aa_sequence = ""
        
        for i in range(0,len(sequence)-2,3): # jumps of 3 and goes till length but 2
            three_mer = sequence[i] + sequence[i+1] + sequence[i+2]
            amino_acid = dicti[three_mer] # lookup dictionary for corr amino acid
            aa_sequence+=amino_acid # append to aa sequence
        
        return aa_sequence

def driver_program():
    
    '''This program drives the rest of the code and gives an easy terminal based interface'''

    dna = input("Enter your DNA sequence: ")
    dna = dna.lower()  # Converting all bases to lower case 
    
    print("\nEnter 1 to get sequence of reverse strand")
    print("Enter 2 to get RNA sequence synthesized")
    print("Enter 3 to get Amino acid synthesized")
    
    opcode = int(input("Enter your choice: "))
    
    if(opcode == 1): # Subpart 1
        
        answer = reverse_strand(dna)
        if(answer == -1):
            print("\nDNA sequence not valid")
        else:
            print("\n"+answer)
    
    elif(opcode == 2): # Subpart 2
        
        answer = rna(dna)
        if(answer == -1):
            print("\nDNA sequence not valid")
        else:
            print("\n"+answer)
        
    elif(opcode == 3): # Subpart 3
        
        answer = rna(dna)
        if(answer == -1):
            print("\nDNA sequence not valid")
        else:
            print("\n"+translate(answer))
            
    else: # Invalid choice
        
        print("\nEnter a valid choice")
        
driver_program()

