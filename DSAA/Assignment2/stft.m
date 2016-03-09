%% Calculating the Short-time Fourier Transform of a Signal
% Inspired by - Hristo Zhivomirov

function [stft, f, t] = stft(x, wlen, h, fs)

%Calculate the length of the signal
xlen = size(x,1);
%Calculate the top 1k Fourier co-efficients
nfft = 1e3;

%Calculate the number of rows and columns in STFT - Obvious!
col = 1;
rown = ceil((1+nfft)/2);
coln = 1+fix((xlen-wlen)/h);

% Calculate the Time And Frequency vectors
t = (wlen/2:h:wlen/2+(coln-1)*h)/fs;
f = (0:rown-1)*fs/nfft;

% Initialize the stft matrix
stft = zeros(rown, coln);
% Initialize the Hamming Window
win = hamming(wlen, 'periodic');

%Iterate over all the windows in step sizes of h with window length wlen
for indx = 0:h:xlen-wlen
    %Calculate the fourier spectrum and store it in an array
    X = fft(x(indx+1:indx+wlen).*win, nfft);    
    stft(:, col) = X(1:rown);
    col = col + 1;
end

%Calculate the transpose of the matrix
stft = stft';

end