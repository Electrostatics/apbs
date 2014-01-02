#! /bin/sh
# usage:   amber2charmm filename
cat $1 | sed -e "s/HIE/HSE/g" \
       | sed -e "s/HID/HSD/g" \
       | sed -e "s/2HB /HB1 /g" \
       | sed -e "s/3HB /HB2 /g" \
       | sed -e "s/4HB /HB3 /g" \
       | sed -e "s/2HG /HG1 /g" \
       | sed -e "s/3HG /HG2 /g" \
       | sed -e "s/2HA /HA1 /g" \
       | sed -e "s/3HA /HA2 /g" \
       | sed -e "s/2HE /HE1 /g" \
       | sed -e "s/3HE /HE2 /g" \
       | sed -e "s/4HE /HE3 /g" \
       | sed -e "s/2HZ /HZ1 /g" \
       | sed -e "s/3HZ /HZ2 /g" \
       | sed -e "s/4HZ /HZ3 /g" \
       | sed -e "s/2HD /HD1 /g" \
       | sed -e "s/3HD /HD2 /g" \
       | sed -e "s/4HD /HD3 /g" \
       | sed -e "s/2HE2/HE21/g" \
       | sed -e "s/3HE2/HE22/g" \
       | sed -e "s/2HH1/HH11/g" \
       | sed -e "s/3HH1/HH12/g" \
       | sed -e "s/2HH2/HH21/g" \
       | sed -e "s/3HH2/HH22/g" \
       | sed -e "s/2HD2/HD21/g" \
       | sed -e "s/3HD2/HD22/g" \
       | sed -e "s/4HD2/HD23/g" \
       | sed -e "s/2HD1/HD11/g" \
       | sed -e "s/3HD1/HD12/g" \
       | sed -e "s/4HD1/HD13/g" \
       | sed -e "s/2HG2/HG21/g" \
       | sed -e "s/3HG2/HG22/g" \
       | sed -e "s/4HG2/HG23/g" \
       | sed -e "s/2HG1/HG11/g" \
       | sed -e "s/3HG1/HG12/g" \
       | sed -e "s/4HG1/HG13/g" \
       | sed -e "s/HG  SER/HG1 SER/g" \
       | sed -e "s/ H / HN/g" \
       > xyz789
       mv xyz789 $1
