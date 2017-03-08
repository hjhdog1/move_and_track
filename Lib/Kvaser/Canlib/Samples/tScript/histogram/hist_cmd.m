# This file is used to present the result from histogram.t in Matlab or Octave
# It will ask for an index and set that fix in order to reduce the data to three
# dimensions and then make a surface plot for temperature and fuel.
#
# The data file hist.mat should have been read from the sd-card, e.g. with
# the command
# tutil.exe -channel=0 -copyfrom hist.mat hist.mat

1;
load hist.mat;
len = input("What index (1-10)? ");
figure(1);
surf([0,1,2,3,4,5,6,7,8,9],[0,1,2,3,4,5,6,7,8,9],TEMP(:,:,len));
title("TEMP for index len");
figure(2);
surf([0,1,2,3,4,5,6,7,8,9],[0,1,2,3,4,5,6,7,8,9],FUEL(:,:,len));
title("FUEL for index len");
