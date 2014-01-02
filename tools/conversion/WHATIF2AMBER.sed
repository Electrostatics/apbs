# Convert all bases to ribo form (i.e., not deoxy)
s/ U / RU /g
s/ G / RG /g
s/ C / RC /g
s/ A / RA /g

# Convert U 'H5' to H5''
s/ 'H5' RU / H5'' RU /g
s/ 'HO2 RU / H2'' RU /g

# phosphate index G
s/ OP1 RG / O1P RG /g
s/ OP2 RG / O2P RG /g

# (52) Convert G 'H5' to H5'' 
s/ 'H5' RG / H5'' RG /g
s/ 'HO2 RG / H2'' RG /g

# phosphate index A
s/ OP1 RA / O1P RA /g
s/ OP2 RA / O2P RA /g

# (119) Convert A 'H5' to H5'' 
s/ 'H5' RA / H5'' RA /g
s/ 'HO2 RA / H2'' RA /g 

# (233) phosphate index U
s/ OP1 RU / O1P RU /g
s/ OP2 RU / O2P RU /g

# (420) phosphate index C
s/ OP1 RC / O1P RC /g
s/ OP2 RC / O2P RC /g

# Convert C 'H5' to H5''
s/ 'H5' RC / H5'' RC /g
s/ 'HO2 RC / H2'' RC /g

# (48740) 3' end terminal H
s/ 'HO3 RA / H3T TE3 /g
s/ 'HO3 RC / H3T TE3 /g
s/ 'HO3 RG / H3T TE3 /g
s/ 'HO3 RU / H3T TE3 /g

# (not sure how to deal with terminal O3')
# (need to find all terminal 'HO3 and modify all matching O3' by hand)

# (48743) protein label changes VAL
s/ 1HG1 VAL / HG11 VAL /g
s/ 2HG1 VAL / HG12 VAL /g
s/ 3HG1 VAL / HG13 VAL /g
s/ 1HG2 VAL / HG21 VAL /g
s/ 2HG2 VAL / HG22 VAL /g
s/ 3HG2 VAL / HG23 VAL /g
s/ RC VAL / C VAL /g
s/ H1 VAL / HT1 NTE /g
s/ H2 VAL / HT2 NTE /g
s/ H3 VAL / HT3 NTE /g

# (need to change N VAL to N NTE by hand for all terminal VAL's)
# nonterminal VAL H

s/ H VAL / HN VAL /g

# (48761) protein label changes LYS
s/ RC LYS / C LYS /g
s/ H LYS / HN LYS /g
s/ 2HB LYS / HB1 LYS /g
s/ 3HB LYS / HB2 LYS /g
s/ 2HG LYS / HG1 LYS /g
s/ 3HG LYS / HG2 LYS /g
s/ 2HD LYS / HD1 LYS /g
s/ 3HD LYS / HD2 LYS /g
s/ 2HE LYS / HE1 LYS /g
s/ 3HE LYS / HE2 LYS /g

# (48783) protein label changes GLU
s/ RC GLU / C GLU /g
s/ H GLU / HN GLU /g
s/ 2HB GLU / HB1 GLU /g
s/ 3HB GLU / HB2 GLU /g
s/ 2HG GLU / HG1 GLU /g
s/ 3HG GLU / HG2 GLU /g

# (48798) protein label changes LEU
s/ RC LEU / C LEU /g
s/ H LEU / HN LEU /g
s/ 2HB LEU / HB1 LEU /g
s/ 3HB LEU / HB2 LEU /g
s/ 1HD1 LEU / HD11 LEU /g
s/ 2HD1 LEU / HD12 LEU /g
s/ 3HD1 LEU / HD13 LEU /g
s/ 1HD2 LEU / HD21 LEU /g
s/ 2HD2 LEU / HD22 LEU /g
s/ 3HD2 LEU / HD23 LEU /g

# (48851) protein label changes ALA
s/ RC ALA / C ALA /g
s/ H ALA / HN ALA /g
s/ 1HB ALA / HB1 ALA /g
s/ 2HB ALA / HB2 ALA /g
s/ 3HB ALA / HB3 ALA /g

# (48861) protein label changes GLY
s/ RC GLY / C GLY /g
s/ H GLY / HN GLY /g
s/ 2HA GLY / HA1 GLY /g
s/ 3HA GLY / HA2 GLY /g

# (48882) protein label changes HIS
# tricky because there are 3 protonation states of histidine; amber
# dinstiguishes between them and WHATIF doesn't.  probably have to 
# figure out by hand what's what, and change HIS to HSD/HSE/HSP.

# shortcut (hopefully): change all HIS to HSE, then look for "HD1 HSE"
# which will be a part of any HIS for which this will be incorrect, then change # only the incorrect ones.

# (already used) s/ HIS / HSE /g
 
s/ RC HSD / C HSD /g
s/ H HSD / HN HSD /g
s/ 2HB HSD / HB1 HSD /g
s/ 3HB HSD / HB2 HSD /g

s/ RC HSE / C HSE /g
s/ H HSE / HN HSE /g
s/ 2HB HSE / HB1 HSE /g
s/ 3HB HSE / HB2 HSE /g

s/ RC HSP / C HSP /g
s/ H HSP / HN HSP /g
s/ 2HB HSP / HB1 HSP /g
s/ 3HB HSP / HB2 HSP /g

# (48901) protein label changes PHE
s/ RC PHE / C PHE /g
s/ H PHE / HN PHE /g
s/ 2HB PHE / HB1 PHE /g
s/ 3HB PHE / HB2 PHE /g

# (48960) protein label changes ARG
s/ RC ARG / C ARG /g
s/ H ARG / HN ARG /g
s/ 2HB ARG / HB1 ARG /g
s/ 3HB ARG / HB2 ARG /g
s/ 2HG ARG / HG1 ARG /g
s/ 3HG ARG / HG2 ARG /g
s/ 2HD ARG / HD1 ARG /g
s/ 3HD ARG / HD2 ARG /g
s/ 1HH1 ARG / HH11 ARG /g
s/ 2HH1 ARG / HH12 ARG /g
s/ 1HH2 ARG / HH21 ARG /g
s/ 2HH2 ARG / HH22 ARG /g

# (49028) protein label changes TRP
s/ RC TRP / C TRP /g
s/ H TRP / HN TRP /g
s/ HB3 TRP / HB1 TRP /g

# (49052) protein label changes ASN
s/ RC ASN / C ASN /g
s/ H ASN / HN ASN /g
s/ 2HB ASN / HB1 ASN /g
s/ 3HB ASN / HB2 ASN /g
s/ 1HD2 ASN / HD21 ASN /g
s/ 2HD2 ASN / HD22 ASN /g

# (49068) protein label changes PRO
s/ RC PRO / C PRO /g
s/ 2HB PRO / HB1 PRO /g
s/ 3HB PRO / HB2 PRO /g
s/ 2HG PRO / HG1 PRO /g
s/ 3HG PRO / HG2 PRO /g
s/ 2HD PRO / HD1 PRO /g
s/ 3HD PRO / HD2 PRO /g

# (49156) protein label changes TYR
s/ RC TYR / C TYR /g
s/ H TYR / HN TYR /g
s/ HB3 TYR / HB1 TYR /g

# (49156) protein label changes ILE
s/ RC ILE / C ILE /g
s/ H ILE / HN ILE /g
s/ CD1 ILE / CD ILE /g
s/ 2HG1 ILE / HG11 ILE /g
s/ 3HG1 ILE / HG12 ILE /g
s/ 1HG2 ILE / HG21 ILE /g
s/ 2HG2 ILE / HG22 ILE /g
s/ 3HG2 ILE / HG23 ILE /g
s/ 1HD1 ILE / HD1 ILE /g
s/ 2HD1 ILE / HD2 ILE /g
s/ 3HD1 ILE / HD3 ILE /g

# (49361) protein label changes ASP
s/ RC ASP / C ASP /g
s/ H ASP / HN ASP /g
s/ 2HB ASP / HB1 ASP /g
s/ 3HB ASP / HB2 ASP /g

# (49394) protein label changes GLN
s/ RC GLN / C GLN /g
s/ H GLN / HN GLN /g
s/ 2HB GLN / HB1 GLN /g
s/ 3HB GLN / HB2 GLN /g
s/ 2HG GLN / HG1 GLN /g
s/ 3HG GLN / HG2 GLN /g
s/ 1HE2 GLN / HE21 GLN /g
s/ 2HE2 GLN / HE22 GLN /g

# (49431) protein label changes THR
s/ RC THR / C THR /g
s/ H THR / HN THR /g
s/ 1HG2 THR / HG21 THR /g
s/ 2HG2 THR / HG22 THR /g
s/ 3HG2 THR / HG23 THR /g

# (49445) protein label changes MET
s/ RC MET / C MET /g
s/ H MET / HN MET /g
s/ 2HB MET / HB1 MET /g
s/ 3HB MET / HB2 MET /g
s/ 2HG MET / HG1 MET /g
s/ 3HG MET / HG2 MET /g
s/ 1HE MET / HE1 MET /g
s/ 2HE MET / HE2 MET /g
s/ 3HE MET / HE3 MET /g

# (50445) protein label changes SER
s/ RC SER / C SER /g
s/ H SER / HN SER /g
s/ 2HB SER / HB1 SER /g
s/ 3HB SER / HB2 SER /g
s/ HG SER / HG1 SER /g

# (56041) protein label changes CYS
s/ RC CYS / C CYS /g
s/ H CYS / HN CYS /g
s/ 2HB CYS / HB1 CYS /g
s/ 3HB CYS / HB2 CYS /g
s/ HG CYS / HG1 CYS /g

# (52623) errors on O' and O''      
# these seen to occur at the ends of amino acid chains (?)
# so they may be the unlinked O's of the peptide bond.
# can change O' to OT2 etc. but C atoms may need to be changed by hand.
# terminal COO group properties are independent of amino acid, with the
# exception of MET.  but there aren't any C-terminal MET's, so i'm leaving
# that out for now.

s/ RC CTE / C CTE /g

s/ O' ARG / OT1 CTE /g
s/ O' GLU / OT1 CTE /g
s/ O' VAL / OT1 CTE /g
s/ O' GLY / OT1 CTE /g
s/ O' ALA / OT1 CTE /g
s/ O' TRP / OT1 CTE /g
s/ O' THR / OT1 CTE /g
s/ O' SER / OT1 CTE /g
s/ O' LYS / OT1 CTE /g

s/ O'' O / OT2 CTE /g
s/ O'' O2 / OT2 CTE /g
s/ O'' ARG / OT2 CTE /g
s/ O'' ALA / OT2 CTE /g
s/ O'' TRP / OT2 CTE /g
s/ O'' SER / OT2 CTE /g
s/ O'' LYS / OT2 CTE /g
s/ O'' GLY / OT2 CTE /g

# N-terminal end: replace N with N NTE (prob. have to do by hand)
# replace H1 with HT1 NTE with exceptions PRO, SER.

s/ H1 GLY / HT1 NTE /g
s/ H2 GLY / HT2 NTE /g
s/ H3 GLY / HT3 NTE /g
s/ H1 ASP / HT1 NTE /g
s/ H2 ASP / HT2 NTE /g
s/ H3 ASP / HT3 NTE /g
s/ H1 MET / HT1 NTE /g
s/ H2 MET / HT2 NTE /g
s/ H3 MET / HT3 NTE /g
s/ H1 ALA / HT1 NTE /g
s/ H2 ALA / HT2 NTE /g
s/ H3 ALA / HT3 NTE /g
s/ H1 GLU / HT1 NTE /g
s/ H2 GLU / HT2 NTE /g
s/ H3 GLU / HT3 NTE /g
s/ H1 LYS / HT1 NTE /g
s/ H2 LYS / HT2 NTE /g
s/ H3 LYS / HT3 NTE /g
s/ H1 ARG / HT1 NTE /g
s/ H2 ARG / HT2 NTE /g
s/ H3 ARG / HT3 NTE /g

# weird glutamate protonation 2HE (55204)
# just leave it for now and add fake param to amber.

# N-terminal PRO (73791)
# reconfig to PRON (by hand)
s/ H2 PRO / HN1 PRON /g
s/ H3 PRO / HN2 PRON /g

