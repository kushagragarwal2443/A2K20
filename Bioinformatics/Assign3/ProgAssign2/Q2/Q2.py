def get_features(filename, outputfilename):
    
    '''This function parses the features_table out from the genbank flat file'''
    
    file= open(filename, "r")
    flag = 0
    features_table=""
    for line in file:
        if(line.startswith("FEATURES")):
            #start extracting when FEATURES found
            flag = 1
        if(line.startswith("ORIGIN")):
            #Stop extracting once ORIGIN reached
            flag=0
        if(flag==1):
            features_table+=line 

    file2 = open(outputfilename, "w")
    file2.write(features_table)
    
def driver():
    
    '''This is the driver function for the code'''
    
    print("This is Q2 from Programming Assignment 3 by Kushagra Agarwal 2018113012")
    print("This code parses the features table out from the genbank flat files for NM_175762/FJ959345")
    print("Choose which File to use: ")
    print("1 for NM_175762")
    print("2 for FJ959345")
    choice = int(input("Enter your choice: "))
    if(choice==1):
        inputfile = "NM_175762.gb"
    elif(choice==2):
        inputfile = "FJ959345.gb"
    else:
        print("Invalid choice of inputfile. Try again!")
        return
    outputfilename = input("Enter output file name: ")
    get_features(inputfile, outputfilename)
    print("Done executing! Bye")
    
driver()

