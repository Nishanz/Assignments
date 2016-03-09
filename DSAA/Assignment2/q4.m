clc;clear;
%% Phase 1 - Stitching files using xcorr function
% - By Ameya Prabhu

% Read the input files
[inp1,fs_inp1]=audioread('1.wav');
[inp2,fs_inp2]=audioread('2.wav');
[inp3,fs_inp3]=audioread('3.wav');
[inp4,fs_inp4]=audioread('4.wav');
[inp5,fs_inp5]=audioread('5.wav');

% Convert 2 channel input to 1 channel for the sake of simplicity
inp1 = inp1(:,1);
inp2 = inp2(:,1);
inp3 = inp3(:,1);
inp4 = inp4(:,1);
inp5 = inp5(:,1);

% Apply heavy Gaussian filtering to the input signal
gaussFilter = gausswin(29);
gaussFilter = gaussFilter / sum(gaussFilter);
inp1(:,1) = conv(inp1(:,1), gaussFilter,'same');
inp2(:,1) = conv(inp2(:,1), gaussFilter,'same');
inp3(:,1) = conv(inp3(:,1), gaussFilter,'same');
inp4(:,1) = conv(inp4(:,1), gaussFilter,'same');
inp5(:,1) = conv(inp5(:,1), gaussFilter,'same');

end1_beg = (size(inp1,1)-fs_inp1*4);
end2_beg = (size(inp2,1)-fs_inp2*4);
end3_beg = (size(inp3,1)-fs_inp3*4);
end4_beg = (size(inp4,1)-fs_inp4*4);
end5_beg = (size(inp5,1)-fs_inp5*4);

endclip = zeros(176400,5);
startclip = zeros(176400,5);

endclip(:,1) = inp1(end1_beg+1:size(inp1,1));
endclip(:,2) = inp2(end2_beg+1:size(inp2,1));
endclip(:,3) = inp3(end3_beg+1:size(inp3,1));
endclip(:,4) = inp4(end4_beg+1:size(inp4,1));
endclip(:,5) = inp5(end5_beg+1:size(inp5,1));

startclip(:,1) = inp1(1:fs_inp1*4);
startclip(:,2) = inp2(1:fs_inp2*4);
startclip(:,3) = inp3(1:fs_inp3*4);
startclip(:,4) = inp4(1:fs_inp4*4);
startclip(:,5) = inp5(1:fs_inp5*4);

% Plot the xcorr matrix.
acorv=zeros(5,5);
lag=zeros(5,5);

for i =1:5
    for j=1:5
        [acor,lag] = xcorr(endclip(:,i),startclip(:,j));
        acorv(i,j)=max(abs(acor));
    end
end

% Display the first part of the output - Order of files according to our
% xcorr Matrices
%
disp(acorv);
disp('In this matrix, rows are the endings of the clips and the columns being suitable startings of the next.');
disp('We can clearly see the transition between 3 -> 5 -> 1 -> 2 -> 4 being the correct sequence of the tones.');

disp('Now, we stitch the tunes together!');

%% Phase 2 - Stitching the parts to give the combined signal
% Calculate lags for all the waveforms

I = acorv(3,5);
lagDiff = lag(ceil(I));
lag35 = lagDiff;

I = acorv(5,1);
lagDiff = lag(ceil(I));
lag51 = lagDiff;

I = acorv(1,2);
lagDiff = lag(ceil(I));
lag12 = lagDiff;

I = acorv(2,4);
lagDiff = lag(ceil(I));
lag24 = lagDiff;

finalsignal = [inp3(1:size(inp3,1)+lag35);inp5(1:size(inp5,1)+lag51);inp1(1:size(inp1,1)+lag12);inp2(1:size(inp2,1)+lag24);inp4(:)];
audiowrite('q4_stitched_signal.wav',finalsignal,fs_inp1);
