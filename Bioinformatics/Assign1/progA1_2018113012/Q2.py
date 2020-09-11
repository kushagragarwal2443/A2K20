def ref_sequence():
    
    '''This function creates a string from the reference genome fasta file and returns'''
    
    file = open("cov2_sequence.fasta", 'r')
    sequence =""
    
    for line in file:
        if(line[0]!= ">"):
            sequence+=line[:-1] # last character is newline hence omitted
            
    return sequence.lower() # Convert to lowercase

def split():
    
    '''This function finds the split sites G|AATTC '''
    
    recog_site = "gaattc" # Recognition site for EcoR1
    sequence = ref_sequence()
    split_sites = []
    
    # We need to add 1 and 29903 to the split sites as the open ends also give us fragments
    split_sites.append(1)
    
    for i in range(len(sequence)-5):
        
        sixmer = sequence[i] + sequence[i+1] + sequence[i+2] + sequence[i+3] + sequence[i+4] + sequence[i+5]
        if(sixmer == recog_site): 
            split_sites.append(i+2) # +1 cause we started with index 0 and +1 because cut site is after G
            
    split_sites.append(len(sequence)) # 29903 
            
    return split_sites

def lengths_bp():
    
    '''This function computes the size in base pairs of all fragments formed'''
    
    split_sites = split() # Get all cuts
    length_sequences = []
    
    for i in range(1, len(split_sites)):
        
        if(i == len(split_sites)-1): 
            length_bp = split_sites[i] - split_sites[i-1] + 1 # last base also included hence an extra +1
        else:
            length_bp = split_sites[i] - split_sites[i-1]
            
        length_sequences.append(length_bp)
        
    return length_sequences

def physical_map():
    
    '''Presents the results in a format similar to REBsites'''
    
    split_sites = split()
    lengths  = lengths_bp()
    
    for i in range(len(lengths)):
        
        if(i == len(lengths)-1): # Last base included hence an extra +1 ( absence of -1)
            print(str(split_sites[i]) +" - " + str(split_sites[i+1]) + "\t\tSize(bp): " + str(lengths[i]))
        else:
            print(str(split_sites[i]) +" - " + str(split_sites[i+1]-1) + "\t\tSize(bp): " + str(lengths[i]))
        
physical_map()

