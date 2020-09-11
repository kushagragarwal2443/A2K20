def countlength(length):
    num_digits= 0
    while length>0:
        length = int(length/10)
        num_digits+=1
    return num_digits

def formword(a, i):
    final_word=""
    b = countlength(i)
    for k in range(a-b):
        final_word+=" "
    final_word+=str(i)
    
    return final_word

def fasta2gb():
    
    fd1 = input("Enter Fasta file: ")
    fd2 = input("Enter Genbank Flat file: ")
#     fd1 = "sequence.fasta"
#     fd2 = "a.gb"
    file = open(fd1, "r")
    sequence=""
    sequence_name=""
    for line in file:
        if(line[0]==">"):
            sequence_name = line[1:-1]
        else:
            line = line.lower()
            sequence+=line[:-1]
            
    length = len(sequence)
    
    file2 = open(fd2, "w")
    
    ID = sequence_name.split(" ")[0]
    desc = sequence_name[len(ID)+1: ]
    
    file2.write("LOCUS          " + str(ID) + "    " + str(length) + " bp\n")
    file2.write("DESCRIPTION    " + str(desc)+"\n")
    file2.write("ACCESSION      " + str(ID))
    file2.write("\nORIGIN")
    
    string_size = countlength(length)
    
    for i in range(0,len(sequence),10):
        
        word = formword(string_size,i+1)
        if(i%60==0):
            file2.write("\n\t"+str(word)+" "+sequence[i:i+10]+" ")
        else:
            file2.write(sequence[i:i+10]+" ")
    file2.write("\n//")
    
def get_locus(line):
    
    vals = line.split(" ")
    for val in vals:
        if(val!="LOCUS" and val!=""):
            loc = val
            break
    return loc
 
def get_def(line):

    loc=""
    vals = line.split(" ")
    for val in vals:
        if(val!="DEFINITION" and val!=""):
            loc+=val+" "
            
    return loc.strip()

def gb2fasta():
    
    fd1 = input("Enter Genbank Flat file: ")
    fd2 = input("Enter Fasta file: ")
#     fd1 = "sequence.gb"
#     fd2 = "a.fasta"
    file = open(fd1, "r")
    flag=0
    
    for line in file:
        if(line.startswith("LOCUS")):
            locus = get_locus(line[:-1])
        if(line.startswith("DEFINITION")):
            defin = get_def(line[:-1])
        if(line.startswith("ORIGIN")):
            flag =1
            break
    
    sequence=""
    for line in file:
        for char in line:
            if(char=='a' or char == 'g' or char=='t' or char=='c'):
                sequence+=char.upper()
    
    file2 = open(fd2, "w")
    file2.write(">"+locus+" "+defin+"\n")
    for i in range(0,len(sequence), 70):
        file2.write(sequence[i:i+70]+"\n")

def driver():
    
    print("This is the code for Q1, By Kushagra Agarwal (2018113012)")
    print("A flat file (sequence.gb) and a fasta file (sequence.fasta) is present in this directory for ease of use")
    print("\nYou have 3 options:")
    print("1. Convert a FASTA file to a GB Flat file")
    print("2. Convert a GB Flat file to a FASTA file")
    print("3. Exit")
    
    choice = input("\nEnter your choice: ")
    
    if(int(choice)==1):
        print("Converting a FASTA file to a GB Flat file. You can use 'sequence.fasta' as input for FASTA file")
        fasta2gb()
        print("Done")
    elif(int(choice)==2):
        print("Converting a GB Flat file to a FASTA file. You can use 'sequence.gb' as input for GB Flat file")
        gb2fasta()
        print("Done")
    else:
        print("Bye!")
        
driver()

