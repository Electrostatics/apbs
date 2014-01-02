%
% read the pqr data
PQRdata=importdata(pqr_str);

atomdata=PQRdata.data;
atomN=length(atomdata(:,1));
atomP=atomdata(:,2:4);
atomC=atomdata(:,5);
atomR=atomdata(:,6);

% evalaution fo the center of the grid
% if the center of teh grid is not based on the same molecule (pqr_str) I am calculating the
% potential, I have to read the position of the atoms of the corresponding
% molecule (pqr_cent_str)
if strcmp(pqr_cent_str, pqr_str)==0 
% read the pqr data
PQRdata2=importdata(pqr_cent_str);
atomdatacent=PQRdata2.data;
%atomN=length(atomdata(:,1));
atomPcent=atomdatacent(:,2:4);
%atomC=atomdata(:,5);
%atomR=atomdata(:,6);
else
atomPcent=atomP;
end

% let's evaluate the center of the grid with respect to the molecule
% (pqr_cent_str)
xcent=(min(atomPcent(:,1))+max(atomPcent(:,1)))/2.;
ycent=(min(atomPcent(:,2))+max(atomPcent(:,2)))/2.;
zcent=(min(atomPcent(:,3))+max(atomPcent(:,3)))/2.;