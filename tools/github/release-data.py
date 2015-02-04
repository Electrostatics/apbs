import urllib2
import json
import requests
import sys
from optparse import OptionParser
#from pprint import pprint
import codecs
codecs.register(lambda name: codecs.lookup('utf-8') if name == 'cp65001' else None)
"""was using this to see what I had currently, could be useful later"""

parser = OptionParser()


parser.add_option("-l", "--list",
                  dest="list", action='store_true', default=False,
                  help="list available releases")

parser.add_option("--linux",
                  dest="linux",action='store_true', default=False,
                  help="find the linux release data")
                  
parser.add_option("--osx",
                  dest="osx",action='store_true', default=False,
                  help="find the osx release data")
parser.add_option("--windows",                                   
                  dest="windows",action='store_true', default=False,
                  help="find the windows release data")
                  
parser.add_option("--src",
                  dest="src",action='store_true', default=False,
                  help="find the src release data")
                                    
parser.add_option('-a',"--all",
                  dest="all",action='store_true', default=True,
                  help="find all release data (default)")


parser.add_option("--pdb2pqr-2.0.0",
                  dest="pdb2pqr-2.0.0",action='store_true', default=False,
                  help="find the pdb2pqr 2.0.0 release data") 

parser.add_option("--pdb2pqr-1.9.0",
                  dest="pdb2pqr-1.9.0",action='store_true', default=False,
                  help="find the pdb2pqr 1.9.0 release data")                   
                  
                  
(options,args) = parser.parse_args()


os_version = ['linux', 'osx', 'src', 'windows']
version_num = ['pdb2pqr-2.0.0','pdb2pqr-1.9.0']


data = urllib2.urlopen("https://api.github.com/repos/Electrostatics/apbs-pdb2pqr/releases")
html = data.read()
listofreleases = json.loads(html)

    
#determines if options other than all are selected
counter = 0
for thing in os_version:
    if getattr(options, thing) == True or options.list == True:
        counter += 1
        if counter > 0:
            options.all = False


os = []
release_number = []    
for version in version_num:
    if getattr(options, version):
        a = version.split("-")
        os.append(a[0])
        release_number.append(a[1])
    
b = set(os)
c = set(release_number)

if len(b) == 0:
    if len(c) == 0:
        for version in version_num:
            a = version.split("-")
            os.append(a[0])
            release_number.append(a[1])
            b = set(os)
            c = set(release_number)
            bool2 = True
elif len(c) != 0:
    bool2 = False
        
    
    
bool = True   
    
#main code body, iterates over the command line inputs
for thing in os_version:
    #print getattr(options, thing) == True
    if getattr(options, thing) == True:
        print "\n \n" + thing.upper() + ' RELEASES'
        for number in listofreleases:
            releasename = number.get("name")                
            listofassets = number.get("assets")
            print "\n" + releasename
            print number.get("created_at") + "\n"
            for info in listofassets:
                for d in b:
                    for e in c:
                        if d and e in info.get("name"):
                            if thing in info.get("name"):
                                print info.get("name")
                                count = info.get("download_count")
                                print "Download Count = " + str(count)
                                continue
                        else:
                            if bool2 == True:
                                continue
                            else:
                                print "No releases here"
                                bool = False
                                break        
                    break
                if bool == False:
                    bool = True
                    break
                
                    
                        
                           

            
#the else case, prints all releases                                 
if options.all == True:
    print "\n \n" + "ALL RELEASES"
    for number in listofreleases:
        releasename = number.get("name")
        print "\n" + releasename
        listofassets = number.get("assets")
        print number.get("created_at") + "\n"
        for info in listofassets:
            print info.get("name")
            count = info.get("download_count")
            print "Download Count = " + str(count)
 
#lists releases available 
if options.list == True:
    print "\n" + "RELEASES LIST"
    for number in listofreleases:            
        releasename = number.get("name")  
        print "\n" + releasename
                            
