import telnetlib, json, sys;

verbose = False;
if(str(sys.argv[-1]) in ["v", "-v", "-verbose", "--verbose"]):  #verbosity must be last argument
    verbose = True;
    sys.argv.pop(); #remove verbosity argument to simplify nested if statements below

if(len(sys.argv) > 1):
    if(len(sys.argv) > 2):
        tn = telnetlib.Telnet(str(sys.argv[1]), sys.argv[2]); #user inputs ip and port
    else:
        tn = telnetlib.Telnet(str(sys.argv[1]), 36330); #user inputs ip only
else:
    tn = telnetlib.Telnet("127.0.0.1", 36330); #user inputs nothing

tn.write(b"queue-info\n"); #run FAHClient command to get info

i = 0;
outs = [];
while(True): #reading line by line bypasses all the fluff at the beginning and end of the buffer
    line = tn.read_until(b"\n");
    #print("line:", i, line);
    
    if(b"id" in line): #TODO: case where there are multiple work units
        if(b",\n" in line):
            line = line.strip(b",\n"); #strip newline and comma off non last work unit(s)
        else:
            line = line.strip(b"\n"); #strip newline off last work unit
        
        outs.append( json.loads(line) ); #read info from telnet and convert to dict
    if(b"---" in line): #exit condition. is this always met?
        break;
    
    i+=1;

tn.close();

if(outs):
    for out in outs:
        if(verbose):
            print(out);
        else:
            print("id:", out["id"], "\ncore:", out["core"],
                  "\npercentdone:", out["percentdone"], "\neta:", out["eta"], "\ncreditestimate:", out["creditestimate"]);
        print();
else:
    print("no work units found");
