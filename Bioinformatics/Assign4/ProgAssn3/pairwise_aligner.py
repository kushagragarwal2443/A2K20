from pandas import *
import numpy as np
from prettytable import PrettyTable
import json

############################################################# USEFUL FUNCTIONS ##############################################################

def getsequences(a,b):
    first = open(a, "r")
    second = open(b, "r")

    sequence1 = ""
    for i in first:
        if(i[0]!=">"):
            i = i.rstrip("\n")
            sequence1 = sequence1 + str(i)
    
    sequence2 = ""
    for i in second:
        if(i[0]!=">"):
            i = i.rstrip("\n")
            sequence2 = sequence2 + str(i)

    first.close()
    second.close()

    return (sequence1, sequence2)

def get_dict(file):

    with open(file) as f:
        score = json.load(f)

    return score

############################################################# GLOBAL ALIGNMENT ############################################################################

def getGlobalAlignment(s1, s2, scoring_strategy, gap_score, type):

    if(type == "dna"):
        result = open("./outputs/GlobalAlignmentDNA.txt", "w")
    else:
        result = open("./outputs/GlobalAlignmentProtein.txt", "w")

    rows = len(s1)+1 # +1 to accomodate for the starting gap
    columns = len(s2)+1

    matrix = np.zeros([rows,columns], dtype=int)
    direction_matrix = np.zeros([rows,columns], dtype=int)

    score = get_dict(scoring_strategy)

    result.write("************************  GLOBAL ALIGNMENT   ***************************\n")
    result.write("\nInput Sequences:\n")
    result.write("\nSequence1: {}".format(s1))
    result.write("\nSequence2: {}\n\n".format(s2))

    result.write("Scoring strategy:\n\n")
    result.write(str(score))
    result.write("\n")

    result.write("\nRows: {}     Columns: {}\n".format(rows, columns))

    num_matches =0
    num_mismatches = 0
    num_gaps =0
    num_score =0

    matrix[0][0] = 0 

    # filling the first column
    for i in range(1,rows):
        matrix[i][0] = matrix[i-1][0] + gap_score
        direction_matrix[i][0] = 2

    # filling the first row   
    for i in range(1, columns):
        matrix[0][i] = matrix[0][i-1] + gap_score
        direction_matrix[0][i] = 1

    #filling the rest
    for i in range(1, rows):
        for j in range(1, columns):

            from_left = matrix[i][j-1] + gap_score
            from_top = matrix[i-1][j] + gap_score
            from_top_left = matrix[i-1][j-1] + score[s1[i-1]][s2[j-1]]

            if (from_top_left >= from_left) and (from_top_left >= from_top): 
                maximum = from_top_left
                direction = 0

            elif (from_left >= from_top_left) and (from_left >= from_top): 
                maximum = from_left 
                direction = 1

            else: 
                maximum = from_top 
                direction = 2

            matrix[i][j] = maximum
            direction_matrix[i][j] = direction

        # if(i%100 == 0):
        #         print("DONE FOR {}".format(i))

    result.write("\nSCORING MATRIX \n\n")
    result.write(str(DataFrame(matrix)))
    result.write("\n\nDIRECTION MATRIX \n\n")
    result.write(str(DataFrame(direction_matrix)))
    result.write("\n\nKey ::   0: From top-left, 1: From left, 2: From top")
    
    alignedSequence1 = ""
    alignedSequence2 = ""

    mismatch_list = []
    
    current_cell_x = rows-1
    current_cell_y = columns-1 

    total_number =0

    while(True):

        value = direction_matrix[current_cell_x][current_cell_y]
        total_number+=1

        if(value == 0):

            alignedSequence1 = alignedSequence1 + s1[current_cell_x-1]
            alignedSequence2 = alignedSequence2 + s2[current_cell_y-1]
            if(s1[current_cell_x-1] == s2[current_cell_y-1]):
                num_matches+=1
            else:

                dictionary= dict()
                dictionary["pos"] = total_number
                dictionary["1"] = s1[current_cell_x-1]
                dictionary["2"] = s2[current_cell_y-1]
                mismatch_list.append(dictionary)

                num_mismatches+=1
            current_cell_x = current_cell_x -1
            current_cell_y = current_cell_y -1
            

        elif(value == 1):

            alignedSequence1 = alignedSequence1 + "_"
            alignedSequence2 = alignedSequence2 + s2[current_cell_y-1]
            current_cell_y = current_cell_y -1
            num_gaps+=1

        else:

            alignedSequence1 = alignedSequence1 + s1[current_cell_x-1]
            alignedSequence2 = alignedSequence2 + "_"
            current_cell_x = current_cell_x -1
            num_gaps+=1

        if(current_cell_x == 0 and current_cell_y == 0):
            break     
    
    result.write("\n\nALIGNED SEQUENCES:\n\n")

    result.write(alignedSequence1[::-1]+"\n")
    result.write(alignedSequence2[::-1])

    result.write("\n\nNumber of Matches: {}\n".format(num_matches))
    result.write("Number of Mismatches: {}\n".format(num_mismatches))
    result.write("Number of Gaps: {}\n".format(num_gaps))
    result.write("\nFINAL SCORE: {}".format(matrix[rows-1][columns-1]))   

    result.write("\n\nMISMATCH LIST:\n")
    for i in mismatch_list[::-1] :
        position = total_number +1 - i["pos"]
        result.write("\nPosition: {}; Sequence1: {}; Sequence2: {}".format(position, i["1"], i["2"]))
    result.write("\n")


############################################################# LOCAL ALIGNMENT ############################################################################

def getLocalAlignment(s1, s2, scoring_strategy, gap_score, type):

    if(type == "dna"):
        result = open("./outputs/LocalAlignmentDNA.txt", "w")
    else:
        result = open("./outputs/LocalAlignmentProtein.txt", "w")

    rows = len(s1)+1 # +1 to accomodate for the starting gap
    columns = len(s2)+1

    matrix = np.zeros([rows,columns], dtype=int)
    direction_matrix = np.zeros([rows,columns], dtype=int)

    score = get_dict(scoring_strategy)

    result.write("*************************  LOCAL ALIGNMENT   ***************************\n")
    result.write("\nInput Sequences:\n")
    result.write("\nSequence1: {}".format(s1))
    result.write("\nSequence2: {}\n\n".format(s2))

    result.write("Scoring strategy:\n\n")
    result.write(str(score))
    result.write("\n")

    result.write("\nRows: {}     Columns: {}\n".format(rows, columns))

    num_matches =0
    num_mismatches = 0
    num_gaps =0
    num_score =0

    matrix[0][0] = 0 

    # filling the first column
    for i in range(1,rows):
        matrix[i][0] = 0
        direction_matrix[i][0] = -1

    # filling the first row   
    for i in range(1, columns):
        matrix[0][i] = 0
        direction_matrix[0][i] = -1

    #filling the rest
    for i in range(1, rows):
        for j in range(1, columns):

            from_left = matrix[i][j-1] + gap_score
            from_top = matrix[i-1][j] + gap_score
            from_top_left = matrix[i-1][j-1] + score[s1[i-1]][s2[j-1]]

            if(from_top_left < 0 and from_left < 0 and from_top < 0):
                maximum = 0
                direction = -1

            elif (from_top_left >= from_left) and (from_top_left >= from_top): 
                maximum = from_top_left
                direction = 0

            elif (from_left >= from_top_left) and (from_left >= from_top): 
                maximum = from_left 
                direction = 1

            else: 
                maximum = from_top 
                direction = 2

            matrix[i][j] = maximum
            direction_matrix[i][j] = direction

        # if(i%100 == 0):
        #         print("DONE FOR {}".format(i))

    result.write("\nSCORING MATRIX \n\n")
    result.write(str(DataFrame(matrix)))
    result.write("\n\nDIRECTION MATRIX \n\n")
    result.write(str(DataFrame(direction_matrix)))
    result.write("\n\nKey ::   -1: End the alignment, 0: From top-left, 1: From left, 2: From top")
    
    alignedSequence1 = ""
    alignedSequence2 = ""

    mismatch_list = []

    max_in_array = 0
    position_max = [0,0]

    for i in range(0,rows):
        for j in range(0, columns):

            if(matrix[i][j] > max_in_array):
                max_in_array = matrix[i][j]
                position_max = [i, j]
    
    current_cell_x = position_max[0]
    current_cell_y = position_max[1] 

    total_number =0

    while(True):

        value = direction_matrix[current_cell_x][current_cell_y]
        total_number+=1

        if(value == 0):

            alignedSequence1 = alignedSequence1 + s1[current_cell_x-1]
            alignedSequence2 = alignedSequence2 + s2[current_cell_y-1]
            current_cell_x = current_cell_x -1
            current_cell_y = current_cell_y -1
            

        elif(value == 1):

            alignedSequence1 = alignedSequence1 + "_"
            alignedSequence2 = alignedSequence2 + s2[current_cell_y-1]
            current_cell_y = current_cell_y -1

        elif(value == 2):

            alignedSequence1 = alignedSequence1 + s1[current_cell_x-1]
            alignedSequence2 = alignedSequence2 + "_"
            current_cell_x = current_cell_x -1

        else:
            break


        if(current_cell_x == 0 and current_cell_y == 0):
            break     
    
    result.write("\n\nALIGNED SEQUENCES:\n\n")

    result.write(str(current_cell_x+1))
    result.write("\t")
    result.write(alignedSequence1[::-1])
    result.write("\t")
    result.write(str(position_max[0]))
    result.write("\n")

    result.write(str(current_cell_y+1))
    result.write("\t")
    result.write(alignedSequence2[::-1])
    result.write("\t")
    result.write(str(position_max[1]))

    result.write("\n\nFINAL SCORE: {}".format(matrix[position_max[0]][position_max[1]]))   

#################################################### DRIVER FUNCTION ####################################################
def start():

    print("This Pairwise Alignment code can perfome both:\n1: Needleman-Wunch Algorithm\n2: Smith-Waterman Algorithm\nfor both Protein and DNA sequences")
    print("\nDefault Parameters used:\ni) -10 for gap penalty\nii) Scoring Matrices: dnafull for DNA sequences and blossom62 for Proteins")
    print("\nDo you wish to use the default fasta files for alignment?")
    print("1) Yes\n2) No")
    default = int(input("Enter your choice: "))

    if(default == 1):
        print("\nUsing Default files:\nDNA: DNA1.fasta (Accession ID: AF358259.1), DNA2.fasta (Accession ID: L03306.1)")
        print("Protein: Protein1.fasta (Accession ID: Q9BYF1), Protein2.fasta (AccessionID: Q5EGZ1)")
        print("\nProcessing inputs and calculating alignments")

        a = "./inputs/DNA1.fasta"
        b = "./inputs/DNA2.fasta"
        c = "./inputs/Protein1.fasta"
        d = "./inputs/Protein2.fasta"

        (s1, s2) = getsequences(a,b)
        (s3, s4) = getsequences(c,d)
        
        print("\nComputing Global Alignment for DNA seqeunces .....")
        getGlobalAlignment(s1, s2, "./scoring_matrix/dnafull.json", -10, "dna")
        print("Done: Check output in GlobalAlignmentDNA.txt")

        print("\nComputing Local Alignment for DNA seqeunces .....")
        getLocalAlignment(s1, s2, "./scoring_matrix/dnafull.json", -10, "dna")
        print("Done: Check output in LocalAlignmentDNA.txt")

        print("\nComputing Global Alignment for Protein seqeunces .....")
        getGlobalAlignment(s3, s4, "./scoring_matrix/blossom62.json", -10, "protein")
        print("Done: Check output in GlobalAlignmentProtein.txt")

        print("\nComputing Local Alignment for Protein seqeunces .....")
        getLocalAlignment(s3, s4, "./scoring_matrix/blossom62.json", -10, "protein")
        print("Done: Check output in LocalAlignmentProtein.txt")

        print("\nBye!")

    else:

        print("\nEnter the 4 files (2 DNA sequences, 2 Protein sequences)\n")
        a = input("First DNA sequence: ")
        b = input("Second DNA sequence: ")
        c = input("Fist Protein sequence: ")
        d = input("Second Protein sequence: ")
        print("\nProcessing inputs and calculating alignments")

        (s1, s2) = getsequences(a,b)
        (s3, s4) = getsequences(c,d)

        print("\nComputing Global Alignment for DNA seqeunces .....")
        getGlobalAlignment(s1, s2, "./scoring_matrix/dnafull.json", -10, "dna")
        print("Done: Check output in GlobalAlignmentDNA.txt")

        print("\nComputing Local Alignment for DNA seqeunces .....")
        getLocalAlignment(s1, s2, "./scoring_matrix/dnafull.json", -10, "dna")
        print("Done: Check output in LocalAlignmentDNA.txt")

        print("\nComputing Global Alignment for Protein seqeunces .....")
        getGlobalAlignment(s3, s4, "./scoring_matrix/blossom62.json", -10, "protein")
        print("Done: Check output in GlobalAlignmentProtein.txt")

        print("\nComputing Local Alignment for Protein seqeunces .....")
        getLocalAlignment(s3, s4, "./scoring_matrix/blossom62.json", -10, "protein")
        print("Done: Check output in LocalAlignmentProtein.txt")

        print("\nBye!")

        
################################################### RUNNING THE CODE ####################################################

start()